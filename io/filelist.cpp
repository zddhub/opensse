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

#include <QDir>
#include <QDirIterator>

#include <boost/random.hpp>



namespace sse {

FileList::FileList(const std::string &rootDir)
{
    setRootDir(rootDir);
}

void FileList::lookupDir(const std::vector<std::string> &nameFilters, callback_fn callback)
{
    QStringList filters;
    for(uint i = 0; i < nameFilters.size(); i++) {
        filters.push_back(QString::fromStdString(nameFilters[i]));
    }

    std::vector<std::string> files;
    QDir root(_rootDir.c_str());

    QDirIterator it(root.absolutePath(),
                    filters,
                    QDir::Files | QDir::NoDotAndDotDot,
                    QDirIterator::Subdirectories);

    for(uint i = 0; it.hasNext(); i++) {
        QString p = it.next();
        QString r = root.relativeFilePath(p);
        files.push_back(r.toStdString());
        if(callback) {
            callback(i, r.toStdString());
        }
    }

    _files = files;
}

void FileList::randomSample(uint numOfSamples, uint seed)
{
    if(numOfSamples >= _files.size())
        return;

    std::vector<size_t> indices(_files.size());
    for (size_t i = 0; i < indices.size(); i++) indices[i] = i;

    typedef boost::mt19937 rng_t;
    typedef boost::uniform_int<size_t> uniform_t;
    rng_t rng(seed);
    boost::variate_generator<rng_t&, uniform_t> random(rng, uniform_t(0, indices.size() - 1));

    std::random_shuffle(indices.begin(), indices.end(), random);
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
    return _rootDir + "/" + getRelativeFilename(index);
}

const std::string& FileList::getRootDir() const
{
    return _rootDir;
}

void FileList::setRootDir(const std::string &rootDir)
{
    QDir dir(QString::fromStdString(rootDir));
    if(!dir.exists())
    {
        throw std::runtime_error("FileList rootdir <" + rootDir + "> does not exist.");
    }
    _rootDir = rootDir;
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

