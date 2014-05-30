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
