#include <iostream>
#include <fstream>

using namespace std;

#include "features/galif.h"
#include <boost/shared_ptr.hpp>

int main()
{
    cout << "Test galif: " << endl;

//    Galif(uint width, uint numOrients, uint tiles,
//          double peakFrequency, double lineWidth, double lambda,
//          double featureSize, bool isSmoothHist,
//          const std::string& normalizeHist,
//          const std::string& detectorName,
//          uint numOfSamples);

    //make_shared limited your to a maximux of 9 arguments
    boost::shared_ptr<sse::Galif> galif = boost::make_shared< sse::Galif >(
                256, 4, 4,
                0.1, 0.02, 0.3,
                0.1, true,
                "l2");

//    sse::Galif *galif = new sse::Galif(
//                256, 4, 4,
//                0.1, 0.02, 0.3,
//                0.1, true,
//                "l2", "grid",
//                625);

    sse::KeyPoints_t keypoints;
    sse::Features_t features;
    cv::Mat image = cv::imread("sketch.png");

    galif->compute(image, keypoints, features);

    ofstream ko("keypoints");
    ko << keypoints.size() <<endl;
    for(uint i = 0; i < keypoints.size(); i++) {
        for(uint j = 0; j < keypoints[i].size(); j++) {
            ko << keypoints[i][j] << " ";
        }
        ko << endl;
    }
    ko.close();

    ofstream kf("features");
    kf << features.size() <<endl;
    kf << features[0].size() <<endl;
    for(uint i = 0; i < features.size(); i++) {
        for(uint j = 0; j < features[i].size(); j++) {
            kf << features[i][j] << " ";
        }
        kf << endl << endl;
    }
    kf.close();

    return 0;
}

