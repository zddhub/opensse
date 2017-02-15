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

#include "util.h"

namespace sse {

void normalizeKeypoints(const KeyPoints_t &keypoints, const cv::Size &imageSize, KeyPoints_t &keypointsNormalized)
{
    Vec_f32_t p(2);
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
