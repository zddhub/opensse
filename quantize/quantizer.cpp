#include "quantizer.h"

namespace sse {

//Quantize one image
void quantize(const Features_t &features, const Vocabularys_t &vocabulary,
              Vec_f32_t &vf, Quantizer_fn &quantizer)
{
    Vocabularys_t quantized_samples;
    quantize_samples_parallel(features, vocabulary, quantized_samples, quantizer);

    build_histvw(quantized_samples, vocabulary.size(), vf, false);
}

void quantize_samples_parallel(const Features_t &samples, const Vocabularys_t &vocabulary,
                               Vocabularys_t &quantized_samples, Quantizer_fn &quantizer)
{
    quantized_samples.resize(samples.size());

    //for each word compute distances to each entry in the vocabulary ...
#pragma omp parallel for
    for(uint i = 0; i < samples.size(); i++) {
        quantizer(samples[i], vocabulary, quantized_samples[i]);
    }
}

void build_histvw(const Vocabularys_t &quantized_samples, uint vocabulary_size, Vec_f32_t &histvw,
                  bool normalize, const KeyPoints_t &keypoints, int res)
{
    assert(res > 0);
    assert(vocabulary_size > 0);

    if(res > 1) {
        assert(keypoints.size() == quantized_samples.size());
    }

    //size_t vocabularySize = quantized_features[0].size();

    // length of the vector is number of cells x histogram length
    // i.e. it actually stores one histogram per cell

    histvw.resize(res*res*vocabulary_size, 0);

    for(uint i = 0; i < quantized_samples.size(); i++) {
        assert(quantized_samples[i].size() == vocabulary_size);

        // in the case of res = 1, offset will be zero and
        // we only have a single histogram (no pyramid) and
        // thus the offset into this overall histogram will be zero
        int offset = 0;

        // ----------------------------------------------------------------
        // Special path for building a spatial pyramid
        //
        // If the user has chosen res = 1 we do not care about the content
        // of the positions vector as they are only accessed for res > 1
        if(res > 1) {
            int x = static_cast<int>(keypoints[i][0] * res);
            int y = static_cast<int>(keypoints[i][1] * res);

            if(x == res) x--;
            if(y == res) y--;

            // generate a linear index from 2D (x,y) index
            int idx = y*res + x;
            assert (idx >= 0 && idx < res*res);

            // identify the spatial histogram we want to add to
            offset = vocabulary_size*idx;
        }
        // -----------------------------------------------------------------

        // Build up histogram by adding the quantized feature to the
        // intermediate histogram. Offset defines the spatial bin we add into
        for(uint j = 0; j < vocabulary_size; j++) {
            histvw[offset+j] += quantized_samples[i][j];
        }
    }


    // for the soft features we should normalize by the number of samples
    // but we do not really want that for the hard quantized features...
    // follow the approach by Chatterfield et al.
    // The second check is to avoid division by zero. In case an empty quantized_features
    // vector is passed in, the result will be an all zero histogram
    if(normalize && quantized_samples.size() > 0) {
        uint numSamples = quantized_samples.size();
        for(uint i = 0; i < histvw.size(); i++) {
            histvw[i] /= numSamples;
        }
    }
}

}

