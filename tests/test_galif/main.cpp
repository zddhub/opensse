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
#include <iostream>
#include <fstream>

using namespace std;

#include "features/galif.h"
#include <boost/shared_ptr.hpp>

void usage() {
    cout << "test_galif image keypoints features"<<endl;
}

int main(int argc, char**argv)
{
    cout << "Test galif: ";

    //    sse::Galif *galif = new sse::Galif(
    //                256, 4, 4,
    //                0.1, 0.02, 0.3,
    //                0.1, true,
    //                "l2", "grid",
    //                625);

    //Notice: make_shared limited your to a maximux of 9 arguments,
    //so using ptree transfer parameters

    if(argc < 3)
    {
        usage();
        exit(1);
    }

    sse::PropertyTree_t params;
    boost::property_tree::read_json("params.json", params);
    boost::shared_ptr<sse::Galif> galif = boost::make_shared<sse::Galif>(params);

    sse::KeyPoints_t keypoints;
    sse::Features_t features;
    cv::Mat image = cv::imread(argv[1]);

    galif->compute(image, keypoints, features);

    ofstream ko(argv[2]);
    ko << keypoints.size() <<endl;
    for(uint i = 0; i < keypoints.size(); i++) {
        for(uint j = 0; j < keypoints[i].size(); j++) {
            ko << keypoints[i][j] << " ";
        }
        ko << endl;
    }
    ko.close();

    ofstream kf(argv[3]);
    kf << features.size() <<endl;
    kf << features[0].size() <<endl;
    for(uint i = 0; i < features.size(); i++) {
        for(uint j = 0; j < features[i].size(); j++) {
            kf << features[i][j] << " ";
        }
        kf << endl << endl;
    }
    kf.close();

    cout << "Done."<<endl;

    return 0;
}

