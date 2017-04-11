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
#include "filelist.h"

#include <fstream>
#include <algorithm>

namespace sse {

void FileList::randomSample(uint numOfSamples, uint seed)
{
    if(numOfSamples >= _files.size())
        return;

    std::vector<size_t> indices(_files.size());
    for (size_t i = 0; i < indices.size(); i++) indices[i] = i;

    std::random_shuffle(indices.begin(), indices.end());
    indices.resize(numOfSamples);
    std::sort(indices.begin(), indices.end());

    std::vector<std::string> new_files(numOfSamples);
    for (size_t i = 0; i < new_files.size(); i++) new_files[i] = _files[indices[i]];
    _files = new_files;
}

const std::vector<std::string>& FileList::filenames() const
{
    return _files;
}

uint FileList::size() const
{
    return _files.size();
}

const std::string& FileList::getRelativeFilename(uint index) const
{
    assert(index < _files.size());
    return _files[index];
}

std::string FileList::getFilename(uint index) const
{
    return getRelativeFilename(index);
}

void FileList::load(const std::string &filename)
{
    std::ifstream in(filename.c_str());

    std::vector<std::string> files;
    std::string line;
    while(getline(in, line)) {
        files.push_back(line);
    }
    in.close();
    _files = files;
}

void FileList::store(const std::string &filename) const
{
    std::ofstream out(filename.c_str());
    for(int i = 0; i < _files.size(); i++) {
        out << _files[i] << std::endl;
    }
    out.close();
}

}

