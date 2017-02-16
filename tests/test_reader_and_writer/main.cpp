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

int main()
{
    //see io/reader_writer.cpp
    cout << "Test reader and writer: " << endl;

    sse::Vocabularys_t v;
//    sse::read("vocabulary_r", v);
//    sse::write(v, "vocabulary_w");

    sse::InvertedIndex index;
    index.load("indexfile_r");
    index.save("indexfile_w");

    return 0;
}

