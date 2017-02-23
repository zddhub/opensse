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
#ifndef GALIF_H
#define GALIF_H

#include "feature.h"
#include "detector.h"

namespace sse {

class Galif : public Feature
{
public:
    // Galif(const PropertyTree_t &parameters);
    Galif(uint width, uint numOrients, uint tiles,
          double peakFrequency, double lineWidth, double lambda,
          double featureSize, bool isSmoothHist,
          const std::string& normalizeHist,
          const std::string& detectorName,
          uint numOfSamples);
    void compute(const cv::Mat &image, KeyPoints_t &keypoints, Features_t &features) const;
    double scale(const cv::Mat &image, cv::Mat &scaled) const;
    void detect(const cv::Mat &image, KeyPoints_t &keypoints) const;
    void extract(const cv::Mat &image, const KeyPoints_t &keypoints, Features_t &features, Vec_Index_t &emptyFeatures) const;
private:
    void assertImageSize(const cv::Mat &image) const;

    const uint _width;
    const uint _numOrients;
    const uint _tiles;
    const double _peakFrequency;
    const double _lineWidth;
    const double _lambda;
    const double _featureSize;
    const bool _isSmoothHist;
    const std::string _normalizeHist;
    const std::string _detectorName;

    cv::Size _filterSize;
    std::vector<cv::Mat_<std::complex<double> > > _gaborFilter;
    Detector *_detector;
};

} //namespace sse

#endif // GALIF_H
