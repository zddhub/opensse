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
    cout << "Usages: " <<endl
         << "  sse quantize -v vocabulary -f features -o output" <<endl
         << "  vocabulary: \t vocabulary file" <<endl
         << "  features: \t features file" <<endl
         << "  output: \t output file" <<endl;
}

int main(int argc, char *argv[])
{
    if(argc != 7) {
        usages();
        exit(1);
    }

    std::vector<Features_t> vecFeatures;

    ifstream ft_in(argv[4]);
    uint filesize = 0;
    ft_in >> filesize;

//    vecFeatures.resize(filesize);
//    for(int i = 0; i < filesize; i++) {
//        read(ft_in, vecFeatures[i]);
//        cout << "read features " << i+1 << "/" << filesize <<"\r"<<flush;
//    }
//    cout << "read features " << filesize << "/" << filesize <<"."<<endl;

    //read(argv[4], vecFeatures, boost::bind(&print, _1, _2, "read features"));

    Vocabularys_t vocabulary;
    read(argv[2], vocabulary, print, "read vocabulary");

    //QuantizerHard
    Quantizer_fn quantizer = QuantizerHard<Vec_f32_t, L2norm_squared<Vec_f32_t> >();

    ofstream fout(argv[6]);
    fout << filesize <<endl;
    fout << vocabulary.size() <<endl;
    //Vocabularys_t samples;
    //samples.resize(vecFeatures.size());
    for(Index_t i = 0; i < filesize; i++) {
        //Vec_f32_t vf;
        Features_t feature;
        Vec_f32_t sample;
        read(ft_in, feature);
        quantize(feature, vocabulary, sample, quantizer);
        for(Index_t j = 0; j < sample.size(); j++) {
            fout << sample[j] << " ";
        }
        fout << endl;
        //quantize(vecFeatures[i], vocabulary, samples[i], quantizer);
        cout << "quantize " << i+1 << "/" << filesize <<"\r"<<flush;
    }
    cout << "quantize " << filesize << "/" << filesize <<"."<<endl;

    fout.close();
    ft_in.close();
    return 0;
}

