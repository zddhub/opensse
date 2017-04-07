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
    cout << "Usages: sse quantize -v vocabulary -f features -o output" <<endl
         << "  This command quantizes \033[4mfeatures\033[0m with \033[4mvocabulary\033[0m" <<endl
         << "  The options are as follows:" <<endl
         << "  -v\t \033[4mvocabulary\033[0m file" <<endl
         << "  -f\t \033[4mfeatures\033[0m file" <<endl
         << "  -o\t \033[4moutput\033[0m file" <<endl;
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

    Vocabularys_t vocabulary;
    read(argv[2], vocabulary, print, "read vocabulary");

    //QuantizerHard
    QuantizerHard<Vec_f32_t, L2norm_squared<Vec_f32_t> > quantizer = QuantizerHard<Vec_f32_t, L2norm_squared<Vec_f32_t> >();

    ofstream fout(argv[6]);
    fout << filesize <<endl;
    fout << vocabulary.size() <<endl;

    for(Index_t i = 0; i < filesize; i++) {
        Features_t feature;
        Vec_f32_t sample;
        read(ft_in, feature);
        quantize(feature, vocabulary, sample, quantizer);
        for(Index_t j = 0; j < sample.size(); j++) {
            fout << sample[j] << " ";
        }
        fout << endl;
        cout << "quantize " << i+1 << "/" << filesize <<"\r"<<flush;
    }
    cout << "quantize " << filesize << "/" << filesize <<"."<<endl;

    fout.close();
    ft_in.close();
    return 0;
}

