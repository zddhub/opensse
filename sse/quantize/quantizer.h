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
#ifndef QUANTIZER_H
#define QUANTIZER_H

#include "../common/types.h"
#include "../common/boost_related.h"

namespace sse {

/**
 * @brief Functor performing hard quantization of a sample against a given codebook of samples
 */
template <class Sample_t, class Dist_fn>
class QuantizerHard
{
public:
    /**
     * @brief Performs hard quantization of \p sample against the passed \p vocabulary.
     *
     * Computes the index of the sample in the vocabulary that has the smallest
     * distance (under the distance functor passed in via the second template parameter) to
     * the sample to be quantized. Note that we actually return a vector that contains a single 1
     * at the corresponding index. This is not really optimal performance-wise, but makes
     * the 'interface' similar to that of quantize_fuzzy such that both functors can be easily
     * exchanged for each other.
     *
     * @param sample Sample to be quantized
     * @param vocabulary Vocabulary
     * @param quantized_sample
     */
    void operator () (const Sample_t& sample, const std::vector<Sample_t>& vocabulary, Vec_f32_t& quantized_sample)
    {
        //quantized_sample.size() == vocabulary.size()
        quantized_sample.resize(vocabulary.size());

        uint closest = 0;
        float minDistance = std::numeric_limits<float>::max();

        Dist_fn dist;

        for(uint i = 0; i < vocabulary.size(); i++) {
            float distance = dist(sample, vocabulary[i]);
            if(distance <= minDistance) {
                closest = i;
                minDistance = distance;
            }
        }

        quantized_sample[closest] = 1;
    }
};

/**
 * @brief Functor performing soft quantization of a sample against a given codebook of samples
 */
template <class Sample_t, class Dist_fn>
class QuantizeFuzzy
{
public:
    QuantizeFuzzy(float sigma) :_sigma(sigma)
    {
        assert(_sigma > 0);
    }

    void operator () (const Sample_t& sample, const std::vector<Sample_t>& vocabulary, Vec_f32_t& quantized_sample)
    {
        //quantized_sample.size() == vocabulary.size()
        quantized_sample.resize(vocabulary.size());

        float sigma2 = 2*_sigma*_sigma;
        float sum = 0;

        for(uint i = 0; i < vocabulary.size(); i++) {
            float d = dist(sample, vocabulary[i]);
            float e = exp(-d*d / sigma2);
            sum += e;
            quantized_sample[i] = e;
        }

        // Normalize such that sum(result) = 1 (L1 norm)
        // The reason is that each local feature contributes the same amount of energy (=1) to the
        // resulting histogram, If we wouldn't normalize, some features (that are close to several
        // entries in the vocabulary) would contribute more energy than others.
        // This is exactly the approach taken by Chatterfield et al. -- The devil is in the details
        for(uint i = 0; i < quantized_sample.size(); i++) {
            quantized_sample[i] /= sum;
        }
    }

private:
    float _sigma;
};


/**
 * @brief 'Base-class' for a quantization function.
 *
 * Instead of creating a common base class we use a boost::function
 * that achieves the same effect, i.e. both quantize_hard
 * and quantize_fuzzy can be assigned to this function type
 */
typedef boost::function<void (const Vec_f32_t&, const std::vector<Vec_f32_t>&, Vec_f32_t&)> Quantizer_fn;

/**
 * @brief Convenience function that quantizes a vector of samples in parallel
 * @param samples Vector of samples to be quantized, each sample is of vector<float>
 * @param vocabulary Vocabulary to quantize the samples against
 * @param quantized_samples A vector of the same size as the \p samples vector with each
 * entry being a vector the size of the \p vocabulary.
 * @param quantizer quantization function to be used
 */
void quantize_samples_parallel(const Features_t &samples, const Vocabularys_t &vocabulary,
                               Vocabularys_t &quantized_samples, Quantizer_fn& quantizer);

// Given a list of quantized samples and corresponding coordinates
// compute the (spatialized) histogram of visual words out of that.
// normalize=true normalizes the resulting histogram by the number
// of samples, this is typically only used in case of a fuzzy histogram!
//
// Note a):
// If you don't want to add any spatial information only pass in the
// first three parameters, this gives a standard BoF histogram
//
// Note b):
// we assume that the keypoints lie in [0,1]x[0,1]
void build_histvw(const Vocabularys_t &quantized_samples, uint vocabulary_size, Vec_f32_t &histvw,
                  bool normalize, const KeyPoints_t &kepoints = KeyPoints_t(), int res = 1);

//Quantize one image with some default parameters
void quantize(const Features_t &features, const Vocabularys_t &vocabulary,
              Vec_f32_t &vf, Quantizer_fn &quantizer);
} //namespace sse



#endif // QUANTIZER_H
