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

using namespace sse;

void usages() {
    cout << "Usages: sse extract -f filelist -o output" <<endl
         << "  This command extracts feature descriptors of images" <<endl
         << "  The options are as follows:" <<endl
         << "  -f\t image \033[4mfilelist\033[0m" <<endl
         << "  -o\t \033[4moutput\033[0m" <<endl;
}

int main(int argc, char *argv[])
{
    if(argc != 5) {
        usages();
        exit(1);
    }

    FileList files;
    files.load(argv[2]);

    Galif *galif = new Galif(
        256, // width
        4, // numOrients
        4, // tiles
        0.1, // peakFrequency
        0.02, // lineWidth
        0.3, // lambda
        0.1, // featureSize
        true, // isSmoothHist
        "l2", // normalizeHist
        "grid", // detectorName,
        625 // numOfSamples
    );

    std::vector<KeyPoints_t> vecKeypoints;
    std::vector<Features_t> vecFeatures;

    // Don't keep keypoints save memory.
    // std::string kp_file = std::string(argv[4]) + "keypoints";
    // ofstream kp_out(kp_file.c_str());
    std::string ft_file = std::string(argv[4]);
    ofstream ft_out(ft_file.c_str());

    // kp_out << files.size() << endl;
    ft_out << files.size() << endl;
    for(uint i = 0; i < files.size(); i++) {
        KeyPoints_t keypoints;
        Features_t features;
        cv::Mat image = cv::imread(files.getFilename(i));
        galif->compute(image, keypoints, features);
        // write(keypoints, kp_out);
        write(features, ft_out);
        cout << "Extract descriptors " << i+1 << "/" << files.size() <<"\r" << flush;
    }
    cout << "Extract descriptors "<< files.size() << "/" << files.size() <<  "." <<endl;

    // kp_out.close();
    ft_out.close();

    return 0;
}

