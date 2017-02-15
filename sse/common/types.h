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
#ifndef TYPES_H
#define TYPES_H

#include <vector>
#include <string>
#include <set>
#include <cmath>

#include <opencv2/core/core.hpp>
#include <opencv2/opencv.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgcodecs/imgcodecs.hpp>

#include <boost/shared_ptr.hpp>
#include <boost/make_shared.hpp>
#include <boost/function.hpp>
#include <boost/bind.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>

namespace sse {

using std::vector;
using std::string;
using std::set;

using cv::Mat;
using boost::property_tree::ptree;

typedef unsigned int uint;

typedef int64_t Index_t;
typedef std::vector<Index_t> Vec_Index_t;

typedef std::vector<float> Vec_f32_t;
typedef std::vector<Vec_f32_t> KeyPoints_t;
typedef std::vector<Vec_f32_t> Features_t;
typedef std::vector<Vec_f32_t> Vocabularys_t;
typedef std::vector<Vec_f32_t> Samples_t; //files has been quantized.

typedef std::pair<float, Index_t> ResultItem_t;

typedef boost::property_tree::ptree PropertyTree_t;


// Returns the value that is stored in the property_tree under path.
// If path does not exist, the default value is returned.
template<class T>
inline T parse(const PropertyTree_t &p, const std::string &path, const T &defaultValue)
{
    T value = p.get(path, defaultValue);
    return value;
}

} //namespace sse

#endif // TYPES_H
