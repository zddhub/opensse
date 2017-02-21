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
#ifndef DISTANCE_H
#define DISTANCE_H

#include <cmath>
#include <limits>
#include <iterator>
#include <vector>
#include <assert.h>

namespace sse {

// ----------------------------------------------------------------
// Note: only distance metrics allowed here, i.e. functions that
// describe the distance between two descriptors and thus smaller
// distances denote more similar objects. The dotproduct itself
// e.g. is NOT a distance measure, it is a similarity measure
// ----------------------------------------------------------------

//L1 norm
template <class T, class R = float>
struct L1norm
{
    // stl
    typedef T first_argument_type;
    typedef T second_argument_type;
    typedef R result_type;

    // L1 distance between a and b.
    R operator() (const T& a, const T& b) const
    {
        R s = 0;
        for (typename T::const_iterator ai = a.begin(), bi = b.begin(); ai != a.end(); ++ai, ++bi)
        {
            R d = static_cast<R>(*ai) - static_cast<R>(*bi);
            s += std::abs(d);
        }
        return s;
    }
};

/**
 * @brief Squared L2 (Euclidean) distance function.
 *
 * Using the squared Euclidean distance is a bit faster than the L2 norm as we avoid the call to sqrt.
 */
template <class T, class R = float>
struct L2norm_squared
{
    // stl
    typedef T first_argument_type;
    typedef T second_argument_type;
    typedef R result_type;

    // Squared L2 distance between a and b.
    R operator() (const T& a, const T& b) const
    {
        R s = 0;
        for (typename T::const_iterator ai = a.begin(), bi = b.begin(); ai != a.end(); ++ai, ++bi)
        {
            R d = static_cast<R>(*ai) - static_cast<R>(*bi);
            s += d*d;
        }
        return s;
    }
};

/**
 * @brief L2 (Euclidean) distance function
 */
template <class T, class R = float>
struct L2norm
{
    // stl
    typedef T first_argument_type;
    typedef T second_argument_type;
    typedef R result_type;

    L2norm_squared<T,R> n;

    // L2 (Euclidean) distance between a and b.
    R operator() (const T& a, const T& b) const
    {
        return std::sqrt(n(a,b));
    }
};

/**
 * @brief One minus dot product distance function.
 *
 * 1 - <a,b> distance function. Assumes a and b are two vectors in R^d
 * having \b unit length
 */
template <class T, class R = float>
struct One_minus_dot
{
    // stl
    typedef T first_argument_type;
    typedef T second_argument_type;
    typedef R result_type;

    // Computes 1 - <a,b>. As we assume that both a and b have unit length, the
    // result is guaranteed to lie in [0,2]
    R operator() (const T& a, const T& b) const
    {
        R s = 0;
        for (typename T::const_iterator ai = a.begin(), bi = b.begin(); ai != a.end(); ++ai, ++bi)
        {
            s += static_cast<R>(*ai) * static_cast<R>(*bi);
        }
        return (1.0 - s);
    }
};

// Jensen-Shannon divergence
template <class T, class R = float>
struct Jsd
{
    // stl
    typedef T first_argument_type;
    typedef T second_argument_type;
    typedef R result_type;

    R operator() (const T& a, const T& b) const
    {
        R s = 0;
        for (typename T::const_iterator ai = a.begin(), bi = b.begin(); ai != a.end(); ++ai, ++bi)
        {
            R v0 = *ai;
            R v1 = *bi;
            R n = 2.0 / (v0 + v1);

            s += ((v0 > 0.0) ? v0 * std::log(v0*n):0.0) + ((v1 > 0.0) ? v1 * std::log(v1*n):0.0);
        }
        return s;
    }
};

/**
 * @brief Chi squared distance function.
 *
 * Chi squared distance function between two vectors k and h; d = sum_i[(ai - bi)^2 / (ai+bi)].
 * We avoid division through zero by adding a tiny value to the denominator:
 * -# if ki == hi == 0 then the nominator is zero and the overall result is zero as desired
 * -# if ki or hi != 0 then ki + hi == ki + hi + float_min
 */
template <class T, class R = float>
struct Chi2
{

    // stl
    typedef T first_argument_type;
    typedef T second_argument_type;
    typedef R result_type;

    R operator() (const T& a, const T& b) const
    {
        R s = 0;
        for (typename T::const_iterator ai = a.begin(), bi = b.begin(); ai != a.end(); ++ai, ++bi)
        {
            R v0 = *ai;
            R v1 = *bi;
            R nom = v0-v1;
            R denom = v0+v1+std::numeric_limits<R>::epsilon();
            s += nom*nom / denom;
        }
        return s;
    }
};

/**
 * @brief Distance function from 'Eitz et al. - An evaluation of descriptors for large-scale image retrieval from sketched feature lines'
 *
 * Used to compare two Tensor descriptors generated by a tensor_generator. Note that this distance function
 * is different from all others in that before you can actually use operator() you need to set the member
 * variable mask, a vector<bool> of size a.size()/3 which contains a boolean for each grid cell of the
 * Tensor descriptor. mask[i] = true indicates that cell i is masked out, i.e. this cell is ignored when
 * computing the distance.
 */
template <class T, class R = float>
struct Dist_frobenius
{
    // stl
    typedef T first_argument_type;
    typedef T second_argument_type;
    typedef R result_type;

    R operator() (const T& a, const T& b) const
    {
        assert(a.size() % 3 == 0);
        assert(a.size() == b.size());

        size_t numCells = a.size() / 3;
        assert(mask->size() == numCells);
        size_t index = 0;
        R dist = 0;

        for (size_t i = 0; i < numCells; i++)
        {
            // masking vector provided by query sketch, if
            // a cell in the query is empty (i.e. contains
            // no lines, we ignore it in the distance computation
            if ((*mask)[i]) continue;

            // difference tensor
            R dE = a[index] - b[index]; index++;
            R dF = a[index] - b[index]; index++;
            R dG = a[index] - b[index]; index++;

            // frobenius norm of difference tensor
            dist += std::sqrt(dE*dE + 2*dF*dF + dG*dG);
        }

        // normalization not required since the number of cells that are masked out
        // is defined by the query sketch and thus is constant for all comparison
        // required for one query
        return dist;
    }

    // if you use this distance function, you will need to manually set the correct
    // mask to be used for the query image
    const std::vector<bool>* mask;
};

/**
 * @brief Distance function from 'Lee & Funkhouser - Sketch-Based Search and Composition of 3D Models'.
 */
template <class T, class R = float>
struct Dist_df
{
    // stl
    typedef T first_argument_type;
    typedef T second_argument_type;
    typedef R result_type;


    /**
     * @brief a and b are two descriptors we want to compare. First half of the vectors
     * is expected to contain the distance transform, second half the image/sketch itself.
     *
     * @param a
     * @param b
     * @return Computes <a,dt(b)> + <b,dt(a)>, result range is [0,inf] where 0 means that a and b are equal
     */
    R operator() (const T& a, const T& b) const
    {
        assert(a.size() % 2 == 0);
        assert(a.size() == b.size());

        result_type dist = 0;
        size_t offset = a.size() / 2;
        for (size_t i = 0; i < offset; i++) {
            dist += a[i]*b[i+offset];
            dist += b[i]*a[i+offset];
        }
        return dist;
    }
};

} //namespace sse



#endif // DISTANCE_H
