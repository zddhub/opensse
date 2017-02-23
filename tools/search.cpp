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

#include <boost/lexical_cast.hpp>

using namespace sse;

void usages()
{
    cout << "Usages: " <<endl
         << "  sse search -i indexfile -v vocabulary -d rootdir -f filelist -n resultsnum -o output" <<endl
         << "  indexfile: \t inverted index file" <<endl
         << "  vocabulary: \t vocabulary file"<<endl
         << "  rootdir: \t rootdir path"<<endl
         << "  filelist: \t filelist"<<endl
         << "  resultsnum: \t the number of results"<<endl
         << "  output: \t output file" <<endl;
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

    Quantizer_fn quantizer = QuantizerHard<Vec_f32_t, L2norm_squared<Vec_f32_t> >();

    TF_simple tf;
    IDF_simple idf;

    FileList files(argv[6]);
    files.load(argv[8]);

    uint numOfResults;
    try {
        numOfResults = boost::lexical_cast<uint>(argv[10]);
    } catch (boost::bad_lexical_cast&) {
        std::cerr << "bad parameter value: "<< argv[10] <<endl;
    }

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

        //cout << "results:"<<endl;

        //ofstream out(argv[12]);
        for(uint i = 0; i < results.size(); i++) {
            //out << results[i].first << " " << files.getFilename(results[i].second).c_str()<<endl;
            cout << results[i].first << " " << files.getFilename(results[i].second).c_str()<<endl;
        }
        //out.close();
    }
    return 0;
}

