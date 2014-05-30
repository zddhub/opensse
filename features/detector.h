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
#ifndef DETECTOR_H
#define DETECTOR_H

#include "common/types.h"

namespace sse {

/**
 * @brief The Detector class
 * Detect key points from input image
 */
class Detector
{
public:
    virtual void detect(const cv::Mat &image, KeyPoints_t &keypoints) const = 0;
};

class GridDetector : public Detector {
public:
    GridDetector(uint numSamples = 625);
    void detect(const cv::Mat &image, KeyPoints_t &keypoints) const;
private:
    uint _numSamples;
};

} //namespace sse


#endif // DETECTOR_H
