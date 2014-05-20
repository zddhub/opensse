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
    boost::shared_ptr<cv::SVM> _svm;
    std::vector<std::string> _labels;
    Vocabularys_t _vocabulary;

private:
    void readSketchsMat(const std::string &sketchs, cv::Mat &sketchsMat);
    void readLabelsMat(const std::string &labels, cv::Mat &labelsMat);
};

} //namespace sse

#endif // SKETCHRECOGNIZER_H
