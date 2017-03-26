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

#include "opensse/opensse.h"
#include "common/boost_related.h"

using namespace sse;

void usages() {
    cout << "Usages: sse extract -d rootDir -f filelist -o output" <<endl
         << "  This command extracts Galif descriptors under \033[4mrootdir\033[0m" <<endl
         << "  The options are as follows:" <<endl
         << "  -d\t root directory" <<endl
         << "  -f\t image file list" <<endl
         << "  -o\t \033[4moutput\033[0m prefix" <<endl;
}

int main(int argc, char *argv[])
{
    if(argc != 7) {
        usages();
        exit(1);
    }

    FileList files;
    files.setRootDir(argv[2]);

    files.load(argv[4]);

    PropertyTree_t parameters;
    Galif *galif = new Galif(
        parse<uint>(parameters, "feature.image_width", 256),
        parse<uint>(parameters, "feature.num_Orients", 4),
        parse<uint>(parameters, "feature.tiles", 4),
        parse<double>(parameters, "feature.peak_frequency", 0.1),
        parse<double>(parameters, "feature.line_width", 0.02),
        parse<double>(parameters, "feature.lambda", 0.3),
        parse<double>(parameters, "feature.feature_size", 0.1),
        parse<bool>(parameters, "feature.is_smooth_hist", true),
        parse<std::string>(parameters, "feature.normalize_hist", "l2"),
        parse<std::string>(parameters, "feature.detector.name", "grid"),
        parse<uint>(parameters, "feature.detector.num_of_samples", 625)
    );

    std::vector<KeyPoints_t> vecKeypoints;
    std::vector<Features_t> vecFeatures;

    //Not keep keypoints and features, save memory.
    std::string kp_file = std::string(argv[6]) + "keypoints";
    ofstream kp_out(kp_file.c_str());
    std::string ft_file = std::string(argv[6]) + "features";
    ofstream ft_out(ft_file.c_str());

    kp_out << files.size() << endl;
    ft_out << files.size() << endl;
    for(uint i = 0; i < files.size(); i++) {
        KeyPoints_t keypoints;
        Features_t features;
        cv::Mat image = cv::imread(files.getFilename(i));
        galif->compute(image, keypoints, features);
        //vecKeypoints.push_back(keypoints);
        //vecFeatures.push_back(features);
        write(keypoints, kp_out);
        write(features, ft_out);
        cout << "Extract descriptors " << i+1 << "/" << files.size() <<"\r" << flush;
    }
    cout << "Extract descriptors "<< files.size() << "/" << files.size() <<  "." <<endl;

    kp_out.close();
    ft_out.close();

    //write(vecKeypoints, std::string(argv[6]) + "keypoints", boost::bind(&print, _1, _2, "save keypoints"));
    //write(vecFeatures, std::string(argv[6]) + "features", boost::bind(&print, _1, _2, "save features"));

    return 0;
}

