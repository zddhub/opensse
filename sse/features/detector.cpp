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
#include "detector.h"

namespace sse {

GridDetector::GridDetector(uint numSamples) :
    _numSamples(numSamples)
{

}

/**
 * @brief GridDetector::detect
 * Keypoints are cross points, when we divide image to square grid.
 *
 * _numSamples, default value is 625
 */
void GridDetector::detect(const cv::Mat &image, KeyPoints_t &keypoints) const
{
    cv::Rect samplingArea(0, 0, image.size().width, image.size().height);

    uint numSample1D = std::ceil(std::sqrt(static_cast<float>(_numSamples)));
    float stepX = samplingArea.width / static_cast<float>(numSample1D+1);
    float stepY = samplingArea.height / static_cast<float>(numSample1D+1);

    for(uint x = 1; x < numSample1D; x++) {
        uint posX = x*stepX;
        for(uint y = 1; y <= numSample1D; y++) {
            uint posY = y*stepY;
            Vec_f32_t p(2);
            p[0] = posX;
            p[1] = posY;
            keypoints.push_back(p);
        }
    }
}

} //namespace sse
