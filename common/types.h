#ifndef TYPES_H
#define TYPES_H

#define __DEBUG__

#include <vector>
#include <string>
#include <cmath>

#include <opencv2/core/core.hpp>
#include <opencv2/opencv.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>

#include <boost/shared_ptr.hpp>
#include <boost/make_shared.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>

namespace sse {

using std::vector;
using std::string;

using cv::Mat;

using boost::property_tree::ptree;

typedef unsigned int uint;

typedef int64_t Index_t;
typedef std::vector<Index_t> Vec_Index_t;

typedef std::vector<float> vec_f32_t;
typedef std::vector<vec_f32_t> KeyPoints_t;
typedef std::vector<vec_f32_t> Features_t;

typedef boost::property_tree::ptree PropertyTree_t;

} //namespace sse

#endif // TYPES_H
