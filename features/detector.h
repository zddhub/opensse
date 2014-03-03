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
    virtual void detect(const cv::Mat& image, KeyPoints_t& keypoints) const = 0;
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
