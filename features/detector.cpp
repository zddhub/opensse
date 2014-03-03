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
            vec_f32_t p(2);
            p[0] = posX;
            p[1] = posY;
            keypoints.push_back(p);
        }
    }
}

} //namespace sse
