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

#include "io/filelist.h"
#include <boost/lexical_cast.hpp>

void usages() {
    cout << "Usages:" <<endl
         << "  generate_filelist -d rootdir -f filter -o filelist [-r num_sample]" <<endl
         << "  rootdir:\t file root directory" <<endl
         << "  filter:\t file name filter, eg: \"*.png\" "<<endl
         << "  num_sample:\t num of samples" <<endl;
}

int main(int argc, char *argv[])
{
    if(argc != 7 && argc != 9) {
        usages();
        exit(1);
    }

    //check input
    //check_input_valid(argv);

    sse::FileList files(argv[2]);
    std::vector<std::string> filters;
    filters.push_back(argv[4]);
    files.lookupDir(filters);

    if(argc == 9) {
        uint value;
        try {
            value = boost::lexical_cast<uint>(argv[8]);
        } catch (boost::bad_lexical_cast&) {
            std::cerr << "bad parameter value: "<< argv[8] <<endl;
        }

        files.randomSample(value, time(0));
    }

    files.store(argv[6]);

    return 0;
}

