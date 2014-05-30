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

#include "common/types.h"
#include "io/reader_writer.h"
#include <fstream>

using namespace sse;

void usages() {
    cout << "Usages: " <<endl
         << "  stat_vocab -s samples -o output" <<endl
         << "  samples: \t stat samples file" <<endl
         << "  output: \t output file" <<endl;
}

int main(int argc, char* argv[])
{
    if(argc != 5) {
        usages();
        exit(1);
    }

    Samples_t samples;
    read(argv[2], samples, boost::bind(&print, _1, _2, "read samples "));

    assert(samples.size() > 0);

    uint vocabularySize = samples[0].size();

    ofstream fout(argv[4]);

    cout << "stat ..." << "\r" <<std::flush;
    for(uint i = 0; i < samples.size(); i++) {
        uint count = 0;
        for(uint j = 0; j < samples[i].size(); j++) {
            if(samples[i][j] == 0)
                count++;
        }
        fout << count <<endl;
    }

    fout.close();
    cout << "stat done." <<endl;

    return 0;
}
