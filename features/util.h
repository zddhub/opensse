#ifndef UTIL_H
#define UTIL_H

#include "common/types.h"

namespace sse {

// Normalizes keypoint coordinates into range [0, 1] x [0, 1] to have them stored independently of image size
void normalizeKeypoints(const KeyPoints_t &keypoints, const cv::Size &imageSize, KeyPoints_t &keypointsNormalized);

// Removes all empty features, i.e. those that only contains zeros
void filterEmptyFeatures(const Features_t &features, const KeyPoints_t &keypoints, const vector<Index_t> &emptyFeatures,
                         Features_t &featuresFiltered, KeyPoints_t &keypointsFiltered);

} //namespace sse

#endif // UTIL_H
