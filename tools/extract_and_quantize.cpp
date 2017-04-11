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
    cout << "Usages: sse extract_and_quantize -d rootDir -f filelist -v vocabulary -o output" <<endl
         << "  This command extracts Galif descriptors under \033[4mrootdir\033[0m and quantizes it at the same time" <<endl
         << "  The options are as follows:" <<endl
         << "  -d\t image root directory" <<endl
         << "  -f\t image file list" <<endl
         << "  -v\t \033[4mvocabulary\033[0m" <<endl
         << "  -o\t \033[4moutput\033[0m samples" <<endl;
}

int main(int argc, char *argv[])
{
    if(argc != 9) {
        usages();
        exit(1);
    }

    FileList files;

    files.load(argv[4]);

    Vocabularys_t vocabulary;
    read(argv[6], vocabulary, print, "read vocabulary");

    //QuantizerHard
    QuantizerHard<Vec_f32_t, L2norm_squared<Vec_f32_t> > quantizer = QuantizerHard<Vec_f32_t, L2norm_squared<Vec_f32_t> >();

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

    ofstream fout(argv[8]);
    fout << files.size() <<endl;
    fout << vocabulary.size() <<endl;
    for(Index_t i = 0; i < files.size(); i++) {
        Vec_f32_t sample;
        cv::Mat image = cv::imread(files.getFilename(i));
        KeyPoints_t keypoints;
        Features_t features;
        galif->compute(image, keypoints, features);
        quantize(features, vocabulary, sample, quantizer);
        for(Index_t j = 0; j < sample.size(); j++) {
            fout << sample[j] << " ";
        }
        fout << endl;
        cout << "quantize " << i+1 << "/" << files.size() <<"\r"<<flush;
    }
    cout << "quantize " << files.size() << "/" << files.size() <<"."<<endl;

    fout.close();
    return 0;
}

