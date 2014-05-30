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

#include "common/types.h"
#include "common/distance.h"
#include "features/galif.h"
#include "quantize/quantizer.h"
#include "io/reader_writer.h"
#include "index/invertedindex.h"
#include "io/filelist.h"

class SketchSearcher : public SearchEngine
{
public:
    SketchSearcher(const sse::PropertyTree_t &parameters);

    void query(const std::string &fileName, QueryResults &results);

private:
    boost::shared_ptr<sse::InvertedIndex> index;
    boost::shared_ptr<sse::Galif> galif;
    boost::shared_ptr<sse::FileList> files;

    sse::Vocabularys_t vocabulary;
    sse::Quantizer_fn quantizer;

    const std::string _indexFile;
    const std::string _vocabularyFile;
    const std::string _rootdir;
    const std::string _fileList;
    const unsigned int _numOfResults;
    const unsigned int _numOfViews;
};

#endif // SKETCHSEARCHER_H
