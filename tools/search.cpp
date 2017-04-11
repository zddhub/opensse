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
using namespace std;

#include <fstream>

#include "opensse/opensse.h"
using namespace sse;

void usages()
{
    cout << "Usages: sse search -i indexfile -v vocabulary -d rootdir -f filelist -n resultsnum -o output" <<endl
         << "OpenSSE search tool in command line"
         << "  The options are as follows:" <<endl
         << "  -i\t inverted index file" <<endl
         << "  -v\t \033[4mvocabulary\033[0m file"<<endl
         << "  -d\t \033[4mrootdir\033[0m path"<<endl
         << "  -f\t \033[4mfilelist\033[0m"<<endl
         << "  -n\t the number of results"<<endl
         << "  -o\t \033[4moutput\033[0m file" <<endl;
}

int main(int argc, char *argv[])
{
    if(argc != 13) {
        usages();
        exit(1);
    }
    InvertedIndex index;
    index.load(argv[2]);

    Vocabularys_t vocabulary;
    read(argv[4], vocabulary, print, "read vocabulary");

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

    QuantizerHard<Vec_f32_t, L2norm_squared<Vec_f32_t> > quantizer = QuantizerHard<Vec_f32_t, L2norm_squared<Vec_f32_t> >();

    TF_simple tf;
    IDF_simple idf;

    FileList files;
    files.load(argv[8]);

    uint numOfResults = atoi(argv[10]);

    cout << ">> open sketch search :"<<endl;
    cout << ">> input absolute path, like \"/Users/zdd/zddhub.png\""<<endl;
    cout << ">> input q exit"<<endl;
    cout << ">> good luck!"<<endl;
    char filename[100];
    while(true) {
        cout << ">> ";
        cin >> filename;

        if(filename[0] == 'q' && filename[1] == '\0')
            break;

        //extract features
        KeyPoints_t keypoints;
        Features_t features;
        cv::Mat image = cv::imread(filename);

        galif->compute(image, keypoints, features);

        //quantize
        Vec_f32_t query;
        quantize(features, vocabulary, query, quantizer);

        std::vector<ResultItem_t> results;
        index.query(query, tf, idf, numOfResults, results);

        for(uint i = 0; i < results.size(); i++) {
            cout << results[i].first << " " << files.getFilename(results[i].second).c_str()<<endl;
        }
    }
    return 0;
}

