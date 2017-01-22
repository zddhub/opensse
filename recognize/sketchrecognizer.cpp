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
#include "sketchrecognizer.h"
#include "features/galif.h"
#include "quantize/quantizer.h"
#include "io/reader_writer.h"
#include "common/distance.h"
#include <fstream>

namespace sse {

SketchRecognizer::SketchRecognizer()
{
    _svm = cv::ml::SVM::create();
}

void SketchRecognizer::readSketchsMat(const std::string &sketchs, cv::Mat &sketchsMat)
{
    std::ifstream sketchs_in(sketchs.c_str());
    uint sketchsize = 0;
    sketchs_in >> sketchsize;
    uint vocabularySize = 0;
    sketchs_in >> vocabularySize;

    Vec_f32_t sketch(vocabularySize);
    for(uint i = 0; i < sketchsize; i++) {

        cv::Mat temp = cv::Mat::zeros(1, vocabularySize, CV_32FC1);
        for(uint j = 0; j < vocabularySize; j++) {
            sketchs_in >> temp.at<float>(0, j);
        }
        sketchsMat.push_back(temp);
        std::cout << "read sketchs " << i <<"/" << sketchsize << "\r" <<std::flush;
    }
    std::cout << "read sketchs " << sketchsize <<"/" << sketchsize << "\n" <<std::flush;
    sketchs_in.close();
}

void SketchRecognizer::readLabelsMat(const std::string &labels, cv::Mat &labelsMat)
{
    std::ifstream labels_in(labels.c_str());
    uint labels_size = 0;
    labels_in >> labels_size;
    uint class_num = 0;
    labels_in >> class_num;

    _labels.clear();
    char line[1024] = {0};
    labels_in.getline(line, sizeof(line)); //get '\n'

    for(uint i = 0; i < labels_size; i++) {
        labels_in.getline(line, sizeof(line));
        _labels.push_back(line);
        cv::Mat temp = cv::Mat::ones(class_num, 1, CV_32FC1) * i;
        labelsMat.push_back(temp);
    }

    labels_in.close();
}

void SketchRecognizer::load(const std::string &svmfile, const std::string &labels, const std::string &vocabulary)
{
    _svm->load(svmfile.c_str());
    cv::Mat mat;
    readLabelsMat(labels, mat);
    read(vocabulary, _vocabulary);
}

void SketchRecognizer::train(const std::string &sketchs, const std::string &labels, const std::string &svmfile)
{
    cv::Mat sketchsMat;
    readSketchsMat(sketchs, sketchsMat);
    cv::Mat labelsMat;
    readLabelsMat(labels, labelsMat);

    _svm->setType(cv::ml::SVM::C_SVC);
    _svm->setC(0.1);
    _svm->setKernel(cv::ml::SVM::RBF);
    _svm->setTermCriteria(cv::TermCriteria(CV_TERMCRIT_ITER, (int)1e7, 1e-6));
    //params.gamma = 0.125;

    std::cout << "start training ..." <<std::endl;
    cv::Ptr<cv::ml::TrainData> tdata = cv::ml::TrainData::create(sketchsMat, cv::ml::ROW_SAMPLE, labelsMat);
    _svm->train(tdata);
//    _svm->train(sketchsMat);
    _svm->save(svmfile.c_str());
    std::cout << "done" <<std::endl;
}

float SketchRecognizer::predict(const std::string &sketch, std::string &label)
{
    //extract features
    KeyPoints_t keypoints;
    Features_t features;
    cv::Mat image = cv::imread(sketch.c_str());

    PropertyTree_t defaultParams;
    boost::shared_ptr<Galif> galif = boost::make_shared<Galif>(defaultParams);
    galif->compute(image, keypoints, features);

    //quantize
    Quantizer_fn quantizer;
    quantizer = QuantizerHard<Vec_f32_t, L2norm_squared<Vec_f32_t> >();
    Vec_f32_t sample;
    quantize(features, _vocabulary, sample, quantizer);

    //predict
    cv::Mat temp = cv::Mat::zeros(1, sample.size(), CV_32FC1);
    for(uint j = 0; j < sample.size(); j++) {
        temp.at<float>(0, j) = sample[j];
    }
    float response = _svm->predict(temp);
    std::cout << (int)response <<std::endl;

    label = _labels[(int)response];

    return response;
}

} //namespace sse
