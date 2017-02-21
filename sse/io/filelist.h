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
#ifndef FILELIST_H
#define FILELIST_H

#include "../common/types.h"

namespace sse {

class FileList
{
public:
    // Constructor, pass in the desired root directory.
    // The directory passed in must be a valid, existing directory
    FileList(const std::string &rootDir = ".");

    // Subsample given filelist randomly
    void randomSample(uint numOfSamples, uint seed);

    // Vector of all filenames
    const std::vector<std::string>& filenames() const;
    uint size() const;

    //Access relative filename of file i
    //index: [0, size()-1]
    const std::string& getRelativeFilename(uint index) const;
    //Access 'absolute' filename of file i
    //index: [0, size()-1]
    std::string getFilename(uint index) const;

    //get and set rootDir
    const std::string& getRootDir() const;
    void setRootDir(const std::string &rootDir);

    //Load a FileList
    //Note that you Must setRootDir
    void load(const std::string &filename);
    //Store a FileList
    //Note that the root directory is not stored, only
    //the list of filenames relative to the root directory.
    void store(const std::string &filename)const;

private:
    std::vector<std::string> _files;
    std::string _rootDir;
};

} //namespace sse

#endif // FILELIST_H
