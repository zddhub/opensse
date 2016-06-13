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
#include "galif.h"

#include "util.h"

namespace sse {

template <class T>
void generate_gabor_filter(cv::Mat_<T> &image, double peakFrequency, double theta, double sigmaX, double sigmaY)
{
    const uint w = image.size().width;
    const uint h = image.size().height;
    const double step_u = 1.0 / static_cast<double>(w);
    const double step_v = 1.0 / static_cast<double>(h);
    const double cos_theta = std::cos(theta);
    const double sin_theta = std::sin(theta);

    const double sigmaXSquared = sigmaX*sigmaX;
    const double sigmaYSquared = sigmaY*sigmaY;

    image.setTo(cv::Scalar(0));

    for(int ny = -1; ny <= 1; ny++) {
        for(int nx = -1; nx <=1; nx++) {
            double v = ny;
            for(uint y = 0; y < h; y++) {
                double u = nx;
                for(uint x = 0; x < w; x++) {
                    double ur = u*cos_theta - v*sin_theta;
                    double vr = u*sin_theta + v*cos_theta;

                    double temp = ur - peakFrequency;
                    double value = std::exp(-2*M_PI*M_PI*(temp*temp*sigmaXSquared + vr*vr*sigmaYSquared));
                    image(y, x) += value;

                    u += step_u;
                }
                v += step_v;
            }
        }
    }
}


Galif::Galif(uint width, uint numOrients, uint tiles,
             double peakFrequency, double lineWidth, double lambda,
             double featureSize, bool isSmoothHist,
             const std::string &normalizeHist,
             const std::string &detectorName,
             uint numOfSamples)
    : _width(width), _numOrients(numOrients), _tiles(tiles)
    , _peakFrequency(peakFrequency), _lineWidth(lineWidth), _lambda(lambda)
    , _featureSize(featureSize), _isSmoothHist(isSmoothHist)
    , _normalizeHist(normalizeHist), _detectorName(detectorName)
{
    _detector = boost::make_shared<GridDetector>(numOfSamples);

    double sigmaX = _lineWidth * _width;
    double sigmaY = _lambda * sigmaX;

    // pad the image by 3*sigma_max, this avoids any boundary effects
    // afterwards increase size to something that fft is working efficiently on
    int paddedSize = cv::getOptimalDFTSize(_width + 3*std::max(sigmaX, sigmaY));
    //std::cout << "galo padded size: " << paddedSize << std::endl;

    _filterSize = cv::Size(paddedSize, paddedSize);

    for(uint i = 0; i < _numOrients; i++) {
        cv::Mat_<std::complex<double> > filter(_filterSize);
        double theta = i * M_PI / _numOrients;

        generate_gabor_filter(filter, _peakFrequency, theta, sigmaX, sigmaY);

        filter(0, 0) = 0;
        _gaborFilter.push_back(filter);
    }
//#define __DEBUG__
#ifdef __DEBUG__
    //output the filters
    for(uint i = 0; i < _numOrients; i++) {
        char filename[64];
        sprintf(filename, "filter_%d.png", i);
        const cv::Mat_<std::complex<double> >& filter = _gaborFilter[i];

        //compute magnitude of response
        cv::Mat mag(filter.size(), CV_32FC1);

        for(int r = 0; r < mag.rows; r++) {
            for(int c = 0; c < mag.cols; c++) {
                const std::complex<double>& v = filter(r, c);
                float m = std::sqrt(v.real() * v.real() + v.imag() * v.imag());
                mag.at<float>(r, c) = m * 255;

            }
        }
        cv::imwrite(filename, mag);
    }
#endif
}

Galif::Galif(const PropertyTree_t &parameters)
    : _width(parse<uint>(parameters, "feature.image_width", 256))
    , _numOrients(parse<uint>(parameters, "feature.num_Orients", 4))
    , _tiles(parse<uint>(parameters, "feature.tiles", 4))
    , _peakFrequency(parse<double>(parameters, "feature.peak_frequency", 0.1))
    , _lineWidth(parse<double>(parameters, "feature.line_width", 0.02))
    , _lambda(parse<double>(parameters, "feature.lambda", 0.3))
    , _featureSize(parse<double>(parameters, "feature.feature_size", 0.1))
    , _isSmoothHist(parse<bool>(parameters, "feature.is_smooth_hist", true))
    , _normalizeHist(parse<std::string>(parameters, "feature.normalize_hist", "l2"))
    , _detectorName(parse<std::string>(parameters, "feature.detector.name", "grid"))
{
    uint numOfSamples = parse<uint>(parameters, "feature.detector.num_of_samples", 625);
    _detector = boost::make_shared<GridDetector>(numOfSamples);

    double sigmaX = _lineWidth * _width;
    double sigmaY = _lambda * sigmaX;

    // pad the image by 3*sigma_max, this avoids any boundary effects
    // afterwards increase size to something that fft is working efficiently on
    int paddedSize = cv::getOptimalDFTSize(_width + 3*std::max(sigmaX, sigmaY));
    //std::cout << "galo padded size: " << paddedSize << std::endl;

    _filterSize = cv::Size(paddedSize, paddedSize);

    for(uint i = 0; i < _numOrients; i++) {
        cv::Mat_<std::complex<double> > filter(_filterSize);
        double theta = i * M_PI / _numOrients;

        generate_gabor_filter(filter, _peakFrequency, theta, sigmaX, sigmaY);

        filter(0, 0) = 0;
        _gaborFilter.push_back(filter);
    }

#ifdef __DEBUG__
    //output the filters
    for(uint i = 0; i < _numOrients; i++) {
        char filename[64];
        sprintf(filename, "filter_%d.png", i);
        const cv::Mat_<std::complex<double> >& filter = _gaborFilter[i];

        //compute magnitude of response
        cv::Mat mag(filter.size(), CV_32FC1);
        cv::Mat mag_c3(filter.size(), CV_32FC3);
        for(int r = 0; r < mag.rows; r++) {
            for(int c = 0; c < mag.cols; c++) {
                const std::complex<double>& v = filter(r, c);
                float m = std::sqrt(v.real() * v.real() + v.imag() * v.imag());
                mag.at<float>(r, c) = m * 255;

                cv::Vec3f &bgr = mag_c3.at<cv::Vec3f>(r,c);
                bgr.val[0] = 255-m*255;
                bgr.val[1] = 255;
                bgr.val[2] = 255-m*255;
//                bgr.val[0] = m*255;
//                bgr.val[1] = 255;
//                bgr.val[2] = m*255;
            }
        }
        cv::imwrite(filename, mag);

        char filename_color[64];
        sprintf(filename_color, "filter_%d_color.png", i);
        cv::imwrite(filename_color, mag_c3);
    }
#endif
}

/**
 * @brief Galif::compute
 * @param image : input image (3-channel image, make sure  image.type() == CV_8UC3)
 * @param keypoints : output, has been normalized in range [0,1]x[0,1], so that they are independent of image size
 * @param features : output, Galif features
 */
void Galif::compute(const cv::Mat &image, KeyPoints_t &keypoints, Features_t &features) const
{
    // --------------------------------------------------------------
    // prerequisites:
    //
    // this generator expects a 3-channel image, with
    // each channel containing exactly the same pixel values
    //
    // the image must have a white background with black sketch lines
    // --------------------------------------------------------------
    assert(image.type() == CV_8UC3);

    cv::Mat gray;
    cv::cvtColor(image, gray, CV_RGB2GRAY);

    assert(gray.type() == CV_8UC1);

    // scale image to desired size
    cv::Mat scaled;
    scale(gray, scaled);

    // detect keypoints on the scaled image
    // the keypoint cooredinates lie in the domain defined by
    // the scaled image size, i.e. if the image has been scaled
    // to 256x256, keypoint coordinates lie in [0,255]x[0,255]
    KeyPoints_t _keypoints;
    detect(scaled, _keypoints);

    //extract local features at the given keypoints
    Features_t _features;
    std::vector<Index_t> emptyFeatures;
    extract(scaled, _keypoints, _features, emptyFeatures);

    assert(_features.size() == _keypoints.size());
    assert(emptyFeatures.size() == _keypoints.size());

    // normalize keypoints to range [0,1]x[0,1] so they are
    // independent of image size
    KeyPoints_t keypointsNormalized;
    normalizeKeypoints(_keypoints, scaled.size(), keypointsNormalized);

    // remove features that are empty, i.e. that contain
    // no sketch stroke within their area
    filterEmptyFeatures(_features, keypointsNormalized, emptyFeatures, features, keypoints);
    assert(features.size() == keypoints.size());

    //if no sketch stroke in image, set one feature histogram all [0].
    if(features.size() == 0) {
        Vec_f32_t histogram(_tiles * _tiles * _numOrients, 0.0f);
        Vec_f32_t zero(2, 0.0f);
        features.push_back(histogram);
        keypoints.push_back(zero);
    }
}

double Galif::scale(const cv::Mat &image, cv::Mat &scaled) const
{
    //uniformly scale the image such that it has no side that is larger than the filter's size
    //Note: _width actually means the maximum desired image side length
    double scaling_factor = (image.size().width > image.size().height)
            ? static_cast<double>(_width) / image.size().width
            : static_cast<double>(_width) / image.size().height;

    cv::resize(image, scaled, cv::Size(0, 0), scaling_factor, scaling_factor, cv::INTER_AREA);
    return scaling_factor;
}

void Galif::detect(const cv::Mat &image, KeyPoints_t &keypoints) const
{
    assert(image.type() == CV_8UC1);
    assertImageSize(image);

    _detector->detect(image, keypoints);
}

void Galif::extract(const cv::Mat &image, const KeyPoints_t &keypoints, Features_t &features, Vec_Index_t &emptyFeatures) const
{
    assert(image.type() == CV_8UC1);
    assertImageSize(image);

    // copy input image centered onto a white background image with
    // exactly the size of our gabor filters
    // WARNING: white background assumed!!!
    cv::Mat_<std::complex<double> > src(_filterSize, 1.0);
    cv::Mat_<unsigned char> inverted = cv::Mat_<unsigned char>::zeros(_filterSize);
    for (int r = 0; r < image.rows; r++) {
        for (int c = 0; c < image.cols; c++)
        {
            // this should set the real part to the desired value
            // in the range [0,1] and the complex part to 0
            src(r, c) = static_cast<double>(image.at<unsigned char>(r, c)) * (1.0/255.0);
            inverted(r, c) = 255 - image.at<unsigned char>(r, c);
        }
    }

    cv::Mat_<int> integral;
    cv::integral(inverted, integral, CV_32S);

    // just a sanity check that the complex part
    // is correctly default initialized to 0
    assert(src(0,0).imag() == 0);

    // filter scaled input image by directional filter bank
    // transform source to frequency domain
    cv::Mat_<std::complex<double> > src_ft(_filterSize);
    cv::dft(src, src_ft);

    // apply each filter
    std::vector<cv::Mat> responses;
    for (uint i = 0; i < _numOrients; i++) {
        // convolve in frequency domain (i.e. multiply spectrums)
        cv::Mat_<std::complex<double> > dst_ft(_filterSize);

        // it remains unclear what the 4th parameter stands for
        // OpenCV 2.8 doc: "The same flags as passed to dft() ; only the flag DFT_ROWS is checked for"
        cv::mulSpectrums(src_ft, _gaborFilter[i], dst_ft, 0);

        // transform back to spatial domain
        cv::Mat_<std::complex<double> > dst;
        cv::dft(dst_ft, dst, cv::DFT_INVERSE | cv::DFT_SCALE);

        // compute magnitude of response
        cv::Mat mag(image.size(), CV_32FC1);
        for (int r = 0; r < mag.rows; r++) {
            for (int c = 0; c < mag.cols; c++) {
                const std::complex<double>& v = dst(r, c);
                float m = std::sqrt(v.real() * v.real() + v.imag() * v.imag());
                mag.at<float>(r, c) = m;
#ifdef __DEBUG__
                mag.at<float>(r, c) = 1.0-m;
#endif //__DEBUG__
            }
        }
#ifdef __DEBUG__
        char filename[64];
        sprintf(filename, "reponse_%d.png", i);
        cv::imwrite(filename, mag*255);
#endif //__DEBUG__

        responses.push_back(mag);
    }

    // local region size is relative to image size
    int featureSize = std::sqrt(image.size().area() * _featureSize);

    // if not multiple of _tiles then round up
    if (featureSize % _tiles)
    {
        featureSize += _tiles - (featureSize % _tiles);
    }

    int tileSize = featureSize / _tiles;
    float halfTileSize = (float) tileSize / 2;

    for (uint i = 0; i < _numOrients; i++) {
        // copy response image centered into a new, larger image that contains an empty border
        // of size tileSize around all sides. This additional  border is essential to be able to
        // later compute values outside of the original image bounds
        cv::Mat framed = cv::Mat::zeros(image.rows + 2*tileSize, image.cols + 2*tileSize, CV_32FC1);
        cv::Mat image_rect_in_frame = framed(cv::Rect(tileSize, tileSize, image.cols, image.rows));
        responses[i].copyTo(image_rect_in_frame);

        if (_isSmoothHist)
        {
            int kernelSize = 2 * tileSize + 1;
            float gaussBlurSigma = tileSize / 3.0;

            // TODO: border type?
            cv::GaussianBlur(framed, framed, cv::Size(kernelSize, kernelSize), gaussBlurSigma, gaussBlurSigma);
        }
        else
        {
            int kernelSize = tileSize;

            // TODO: border type?
            cv::boxFilter(framed, framed, CV_32F, cv::Size(kernelSize, kernelSize), cv::Point(-1, -1), false);
        }

        // response have now size of image + 2*tileSize in each dimension
        responses[i] = framed;
    }

    // will contain a 1 at each index where the underlying patch in the
    // sketch is completely empty, i.e. contains no stroke, 0 at all other
    // indices. Therefore it is essential that this vector has the same size
    // as the keypoints and features vector
    emptyFeatures.resize(keypoints.size(), 0);

    // collect filter responses for each keypoint/region
    for (uint i = 0; i < keypoints.size(); i++) {
        const Vec_f32_t& keypoint = keypoints[i];

        // create histogram: row <-> tile, column <-> histogram of directional responses
        Vec_f32_t histogram(_tiles * _tiles * _numOrients, 0.0f);

        // define region
        cv::Rect rect(keypoint[0] - featureSize/2, keypoint[1] - featureSize/2, featureSize, featureSize);

        cv::Rect isec = rect & cv::Rect(0, 0, src.cols, src.rows);

        // adjust rect position by frame width
        rect.x += tileSize;
        rect.y += tileSize;

        // check if patch contains any strokes of the sketch
        int patchsum = integral(isec.tl())
                + integral(isec.br())
                - integral(isec.y, isec.x + isec.width)
                - integral(isec.y + isec.height, isec.x);

        if (patchsum == 0)
        {
            // skip this patch. It contains no strokes.
            // add empty histogram, filled with zeros,
            // will be (optionally) filtered in a later descriptor computation step
            features.push_back(histogram);
            emptyFeatures[i] = 1;
            continue;
        }

        const int ndims[3] = { (int)_tiles, (int)_tiles, (int)_numOrients };
        cv::Mat_<float> hist(3, ndims, 0.0f);

        for (uint k = 0; k < responses.size(); k++) {
            for (int y = rect.y + halfTileSize; y < rect.br().y; y += tileSize) {
                for (int x = rect.x + halfTileSize; x < rect.br().x; x += tileSize) {
                    // check for out of bounds condition
                    // NOTE: we have added a frame with the size of a tile
                    if (y < 0 || x < 0 || y >= responses[k].rows || x >= responses[k].cols)
                    {
                        continue;
                    }

                    // get relative coordinates in current patch
                    int ry = y - rect.y;
                    int rx = x - rect.x;

                    // get tile indices
                    int tx = rx / tileSize;
                    int ty = ry / tileSize;

                    assert(tx >= 0 && ty >= 0);
                    assert(static_cast<uint>(tx) < _tiles && static_cast<uint>(ty)  < _tiles);

                    hist(ty, tx, k) = responses[k].at<float>(y, x);
                }
            }
        }

        std::copy(hist.begin(), hist.end(), histogram.begin());

        if (_normalizeHist == "l2")
        {
            float sum = 0;
            for (size_t i = 0; i < histogram.size(); i++) sum += histogram[i]*histogram[i];
            sum = std::sqrt(sum)  + std::numeric_limits<float>::epsilon(); // + eps avoids div by zero
            for (size_t i = 0; i < histogram.size(); i++) histogram[i] /= sum;
        }
        else if (_normalizeHist == "lowe")
        {
            cv::Mat histwrap(histogram);
            cv::Mat tmp;
            cv::normalize(histwrap, tmp, 1, 0, cv::NORM_L1);
            tmp = cv::min(tmp, 0.2);
            cv::normalize(histwrap, histwrap, 1, 0, cv::NORM_L1);
            histogram = histwrap;
        }

        // do not normalize if user has explicitly asked for that
        else if (_normalizeHist == "none") {}

        // let the user know about the wrong parameter
        else throw std::runtime_error("unsupported histogram normalization method passed (" + _normalizeHist + ")." + "Allowed methods are : lowe, l2, none." );

        // add histogram to the set of local features for that image
        features.push_back(histogram);
    }
}

void Galif::assertImageSize(const cv::Mat &image) const
{
    //image size must be larger than filter size
    assert((image.size().width <= _filterSize.height) && (image.size().height <= _filterSize.width));
}

} //namespace sse


