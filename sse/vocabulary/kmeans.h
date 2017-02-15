/*************************************************************************
 * Copyright (c) 2014 Zhang Dongdong
 * All rights reserved.
 * 
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 * 
 * http://www.apache.org/licenses/LICENSE-2.0
 * 
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
**************************************************************************/
#ifndef KMEANS_H
#define KMEANS_H

#include <vector>
#include <algorithm>
#include <iostream>
#include <set>

#include "common/distance.h"
#include "common/types.h"

#include <boost/random.hpp>
#include <boost/thread/thread.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/thread/locks.hpp>

#include "kmeans_init.h"

namespace sse {

/**
 * @brief Standard kmeans clustering
 */
template <class collection_t, class dist_fn>
class Kmeans
{
    typedef boost::mutex               mutex_t;
    typedef boost::lock_guard<mutex_t> locker_t;

    typedef typename collection_t::value_type sample_t;

public:

    /**
     * @brief Standard k-means clustering given a distance function
     * @param collection Datastructure containing the samples to be clustered, typically a vector<vector<float> >, where the 'inner'
     * vector<float> would be a single sample.
     * @param numclusters Number of clusters to use.
     * @param initalgorithm Algorithm used to estimate the inital cluster centers
     * @param distfn Distance function used for comparing two samples.
     */
    Kmeans(const collection_t& collection, std::size_t numclusters, KmeansInitAlgorithm initalgorithm = KmeansInitRandom, const dist_fn& distfn = dist_fn())
     : _collection(collection), _distfn(distfn), _centers(numclusters), _clusters(collection.size())
    {
        // get initial centers
        std::vector<std::size_t> initindices;
        if (initalgorithm == KmeansInitPlusPlus)
        {
            kmeans_init_plusplus(initindices, collection, numclusters, distfn);
        }
        else
        {
            kmeans_init_random(initindices, collection, numclusters);
        }

        for (std::size_t i = 0; i < initindices.size(); i++) _centers[i] = collection[initindices[i]];
    }

    /**
     * @brief Perform k-means clustering on the dataset provided in the constructor
     *
     * Iterates until at least one of the following two criteria are met:
     * - maximum number of iterations reached
     * - the fraction of samples that changed clusters is below minchangesfraction
     * @param maxiteration Maximum number of iterations
     * @param minchangesfraction Fraction of changes, if less changes happen in a certain iteration, clustering is done.
     */
    void run(std::size_t maxiteration, double minchangesfraction)
    {
        using namespace boost;

        // main iteration
        std::size_t iteration = 0;
        for (;;)
        {
            if (maxiteration > 0 && iteration == maxiteration) break;

            std::size_t changes = 0;

            // distribute items on clusters in parallel
            thread_group pool;
            std::size_t idx = 0;
            mutex_t mtx;
            for (std::size_t i = 0; i < thread::hardware_concurrency(); i++)
            {
                pool.create_thread(bind(&Kmeans::distribute_samples, this, boost::ref(idx), boost::ref(changes), boost::ref(mtx)));
            }
            pool.join_all();

            iteration++;

            std::cout << "changes: " << changes << std::endl;

            if (changes <= std::ceil(_collection.size() * minchangesfraction)) break;

            // compute new centers
            std::vector<std::size_t> clustersize(_centers.size(), 0);
            for (std::size_t i = 0; i < _collection.size(); i++)
            {
                std::size_t k = _clusters[i];

                // if it is the first assignment for that cluster, then zero the center
                if (clustersize[k] == 0) std::fill(_centers[k].begin(), _centers[k].end(), 0);

                add_operation(_centers[k], _collection[i]);
                clustersize[k]++;
            }

            // assign new centers
            std::vector<std::size_t> invalid, valid;
            for (std::size_t i = 0; i < _centers.size(); i++)
            {
                if (clustersize[i] > 0)
                {
                    div_operation(_centers[i], clustersize[i]);
                    valid.push_back(i);
                }
                else
                {
                    invalid.push_back(i);
                }
                //std::cout << "clustersize " << i << ": " << clustersize[i] << std::endl;
            }

            // fix invalid centers, i.e. those with no members
            while (!invalid.empty() && !valid.empty())
            {
                std::size_t current = invalid.back();
std::cout << "handle invalid clusters: " << invalid.size() << std::endl;
                // compute for each valid cluster the variance
                // of distances to all members and get the
                // most distant member
                std::vector<double> maxdist(_centers.size(), 0.0);
                std::vector<double> variance(_centers.size(), 0.0);
                std::vector<std::size_t> farthest(_centers.size());
                for (std::size_t i = 0; i < valid.size(); i++)
                {
                    std::size_t c = valid[i];


                    for (std::size_t k = 0; k < _collection.size(); k++)
                    {
                        if (_clusters[k] != c) continue;

                        double d = _distfn(_collection[k], _centers[c]);
                        if (d > maxdist[c])
                        {
                            maxdist[c] = d;
                            farthest[c] = k;
                        }
                        variance[c] += d*d;
                    }
                    variance[c] /= clustersize[c];
                }

                // get cluster with highest variance and make
                // the farthest member of that cluster the
                // new center
                std::size_t c = std::distance(variance.begin(), std::max_element(variance.begin(), variance.end()));
                _centers[current] = _collection[farthest[c]];
                _clusters[farthest[c]] = current;

std::cout << "reassign " << current << " to sample " << farthest[c] << " of cluster " << c << std::endl;

                valid.pop_back();
                invalid.pop_back();
            }

std::cout << "iteration " << iteration << std::endl;
        }

std::cout << "kmeans iterations: " << iteration << std::endl;
    }


    // Run clustering, using theoretically unlimited number of iterations. Clustering will
    // stop when the fraction of changes falls below 0.01
    void run_default()
    {
        this->run(std::numeric_limits<std::size_t>::max(), 0.01);
    }

    // Vector of cluster membership: clusters[i] = j means that the sample with index i
    // is a member of cluster j
    const std::vector<std::size_t>& clusters() const
    {
        return _clusters;
    }

    // Vector of cluster centers
    const std::vector<sample_t>& centers() const
    {
        return _centers;
    }


    // Convenience function generating a clustering table:
    // table[i][j] = k means that the sample with index k belongs to cluster i.
    template <class index_t>
    void make_cluster_table(std::vector<std::vector<index_t> >& table)
    {
        table.resize(_centers.size());
        for (std::size_t i = 0; i < _clusters.size(); i++) table[_clusters[i]].push_back(i);
    }

    template <class T>
    static void add_operation(T& lhs, const T& rhs)
    {
        for (std::size_t i = 0; i < lhs.size(); i++) lhs[i] += rhs[i];
    }

    template <class T>
    static void div_operation(T& lhs, double rhs)
    {
        for (std::size_t i = 0; i < lhs.size(); i++) lhs[i] /= rhs;
    }

    private:

    void distribute_samples(std::size_t& index, std::size_t& changes, mutex_t& mutex)
    {
        std::vector<double> dists(_centers.size());
        std::size_t currentchanges = 0;

        for (;;)
        {
            std::size_t i;

            {
                locker_t locker(mutex);
                if (index == _collection.size()) break;
                i = index++;
            }

            // compute distance of current point to every center
            std::transform(_centers.begin(), _centers.end(), dists.begin(), boost::bind(_distfn, boost::ref(_collection[i]), boost::arg<1>()));

            // find the minimum distance, i.e. the nearest center
            std::size_t c = std::distance(dists.begin(), std::min_element(dists.begin(), dists.end()));

            // update cluster membership
            {
                locker_t locker(_mutex);

                if (_clusters[i] != c)
                {
                    _clusters[i] = c;
                    currentchanges++;
                }
//if (i % 1000 == 0) std::cout << "kmeans distribute: " << i << std::endl;
            }
        }

        {
            locker_t locker(mutex);
            changes += currentchanges;
        }
    }

    const collection_t& _collection;
    const dist_fn&      _distfn;

    std::vector<sample_t>    _centers;
    std::vector<std::size_t> _clusters;

    boost::mutex        _mutex;
};

} //namespace sse

#endif // KMEANS_H
