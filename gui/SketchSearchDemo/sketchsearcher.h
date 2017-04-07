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
#ifndef SKETCHSEARCHER_H
#define SKETCHSEARCHER_H

#include "searchengine.h"

#include "opensse/opensse.h"

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
typedef boost::property_tree::ptree PropertyTree_t;
template<class T>
inline T parse(const PropertyTree_t &p, const std::string &path, const T &defaultValue)
{
    T value = p.get(path, defaultValue);
    return value;
}

class SketchSearcher : public SearchEngine
{
public:
    SketchSearcher(const PropertyTree_t &parameters);
    virtual ~SketchSearcher();

    void query(const std::string &fileName, QueryResults &results);

private:
    sse::InvertedIndex *index;
    sse::Galif *galif;
    sse::FileList *files;

    sse::Vocabularys_t vocabulary;
    sse::QuantizerHard<sse::Vec_f32_t, sse::L2norm_squared<sse::Vec_f32_t> > quantizer;

    const std::string _indexFile;
    const std::string _vocabularyFile;
    const std::string _rootdir;
    const std::string _fileList;
    const unsigned int _numOfResults;
    const unsigned int _numOfViews;
};

#endif // SKETCHSEARCHER_H
