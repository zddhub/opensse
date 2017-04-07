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

#include "opensse/opensse.h"
using namespace sse;

void usages() {
    cout << "Usages: sse index -s samples -o output" <<endl
         << "  This command create index for \033[4msamples\033[0m" <<endl
         << "  The options are as follows:" <<endl
         << "  -s\t \033[4msamples\033[0m file that has been quantized" <<endl
         << "  -o\t \033[4moutput\033[0m file" <<endl;
}

int main(int argc, char* argv[])
{
    if(argc != 5) {
        usages();
        exit(1);
    }

    ifstream samples_in(argv[2]);
    uint samplesize = 0;
    samples_in >> samplesize;
    uint vocabularySize = 0;
    samples_in >> vocabularySize;

    InvertedIndex index(vocabularySize);

    assert(samplesize > 0 && vocabularySize > 0);

    cout << "add sample " << "\r" <<std::flush;
    Vec_f32_t sample(vocabularySize);
    for(uint i = 0; i < samplesize; i++) {

        for(int j = 0; j < vocabularySize; j++) {
            samples_in >> sample[j];
        }
        index.addSample(sample);
        cout << "add sample " << i <<"/" << samplesize << "\r" <<std::flush;
    }
    cout << "add sample " << samplesize <<"/" << samplesize << "\n" <<std::flush;
    cout << "create index ..." << "\r" <<std::flush;
    TF_simple tf;
    IDF_simple idf;
    index.createIndex(tf, idf);
    index.save(argv[4]);

    cout << "create index done." <<endl;

    return 0;
}
