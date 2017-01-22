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
#ifndef SKETCHRECOGNIZER_H
#define SKETCHRECOGNIZER_H

#include "common/types.h"
#include "opencv2/ml/ml.hpp"

namespace sse {

class SketchRecognizer
{
public:
    SketchRecognizer();
    void train(const std::string &sketchs, const std::string &labels, const std::string &svmfile);
    float predict(const std::string &sketch, std::string &label);
    void load(const std::string &svmfile, const std::string &labels, const std::string &vocabulary);

protected:
    cv::Ptr<cv::ml::SVM> _svm;
    std::vector<std::string> _labels;
    Vocabularys_t _vocabulary;

private:
    void readSketchsMat(const std::string &sketchs, cv::Mat &sketchsMat);
    void readLabelsMat(const std::string &labels, cv::Mat &labelsMat);
};

} //namespace sse

#endif // SKETCHRECOGNIZER_H
