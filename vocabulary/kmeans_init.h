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
#ifndef KMEANS_INIT_H
#define KMEANS_INIT_H

#include "common/types.h"
#include "common/distance.h"

#include <boost/random.hpp>

namespace sse {

template <class index_t, class collection_t>
void kmeans_init_random(std::vector<index_t>& centers, const collection_t& collection, std::size_t numclusters)
{
    assert(collection.size() >= numclusters);

    centers.resize(collection.size());
    for (std::size_t i = 0; i < centers.size(); i++) centers[i] = i;
    std::random_shuffle(centers.begin(), centers.end());
    centers.resize(numclusters);
}

template <class index_t, class collection_t, class dist_fn>
void kmeans_init_plusplus(std::vector<index_t>& result, const collection_t& collection, std::size_t numclusters, const dist_fn& distfn)
{
    assert(numclusters > 0);
    assert(collection.size() >= numclusters);

    typedef typename collection_t::value_type item_t;
    typedef boost::mt19937                    rng_t;
    typedef boost::uniform_real<double>       unirand_t;

    rng_t rng;

    boost::variate_generator<rng_t&, unirand_t> unirand(rng, unirand_t(0.0, 1.0));

    std::size_t numtrials = 2 + std::log(numclusters);

    // add first cluster, randomly chosen
    std::set<index_t> centers;
    index_t first = unirand() * collection.size();
    centers.insert(first);

    // compute distance between first cluster center and all others
    // and accumulate the distances that gives the current potential
    std::vector<double> dists(collection.size());
    double potential = 0.0;
    for (std::size_t i = 0; i < collection.size(); i++)
    {
        double d = distfn(collection[first], collection[i]);
        dists[i] = d*d;
        potential += dists[i];
    }
    std::cout << "kmeans++ init: numclusters=" << numclusters << " numtrials=" << numtrials << " collection.size=" << collection.size() << " init pot=" << potential << std::endl;

    // iteratively add centers
    for (std::size_t c = 1; c < numclusters; c++)
    {
        double min_potential = std::numeric_limits<double>::max();
        std::size_t best_index = 0;

        for (std::size_t i = 0; i < numtrials; i++)
        {
            std::size_t index;

            // get new center
            double r = unirand() * potential;
            for (index = 0; index < collection.size()-1 && r > dists[index]; index++)
            {
                r -= dists[index];
            }

            while (centers.count(index) > 0) index = (index + 1) % collection.size();

            // recompute potential
            double p = 0.0;
            for (std::size_t k = 0; k < collection.size(); k++)
            {
                double d = distfn(collection[index], collection[k]);
                p += std::min(dists[k], d*d);
            }

            if (p < min_potential)
            {
                min_potential = p;
                best_index = index;
            }
        }

        for (std::size_t i = 0; i < collection.size(); i++)
        {
            double d = distfn(collection[best_index], collection[i]);
            dists[i] = d*d;
        }

        potential = min_potential;

        centers.insert(best_index);

        std::cout << "new center " << c << ": potential=" << potential << " index=" << best_index << std::endl;
    }

    std::copy(centers.begin(), centers.end(), std::back_inserter(result));
}

enum KmeansInitAlgorithm
{
    KmeansInitRandom,
    KmeansInitPlusPlus
};

} //namespace sse

#endif // KMEANS_INIT_H
