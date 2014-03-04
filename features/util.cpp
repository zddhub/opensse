
#include "util.h"

namespace sse {

void normalizeKeypoints(const KeyPoints_t &keypoints, const cv::Size &imageSize, KeyPoints_t &keypointsNormalized)
{
    vec_f32_t p(2);
    for (size_t i = 0; i < keypoints.size(); i++) {
        p[0] = keypoints[i][0] / imageSize.width;
        p[1] = keypoints[i][1] / imageSize.height;
        keypointsNormalized.push_back(p);
    }
}

void filterEmptyFeatures(const Features_t &features, const KeyPoints_t &keypoints, const vector<Index_t> &emptyFeatures,
                         Features_t &featuresFiltered, KeyPoints_t &keypointsFiltered)
{
    assert(features.size() == keypoints.size());
    assert(features.size() == emptyFeatures.size());

    for (size_t i = 0; i < emptyFeatures.size(); i++) {

        if (!emptyFeatures[i]) {
            featuresFiltered.push_back(features[i]);
            keypointsFiltered.push_back(keypoints[i]);
        }
    }
}

} //namespace sse
