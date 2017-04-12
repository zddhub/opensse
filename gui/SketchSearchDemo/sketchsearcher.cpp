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
#include "sketchsearcher.h"
using namespace sse;

SketchSearcher::SketchSearcher(Json &config)
    : _indexFile(config.getValue("searcher$indexfile", "/tmp/SketchSearchDemo/data/model_indexfile"))
    , _vocabularyFile(config.getValue("searcher$vocabulary", "/tmp/SketchSearchDemo/data/vocabulary"))
    , _fileList(config.getValue("searcher$filelist", "/tmp/SketchSearchDemo/data/model_filelist"))
    , _numOfResults(convert<uint>(config.getValue("searcher$results_num", "25"), UINT))
    , _numOfViews(convert<uint>(config.getValue("searcher$views_num", "1"), UINT))
{
    index = new InvertedIndex();
    index->load(_indexFile);

    read(_vocabularyFile, vocabulary);

    galif = new Galif();

    quantizer = QuantizerHard<Vec_f32_t, L2norm_squared<Vec_f32_t> >();

    files = new FileList;
    files->load(_fileList);
}

SketchSearcher::~SketchSearcher()
{
    delete index;
    delete galif;
    delete files;
}

void SketchSearcher::query(const std::string &fileName, QueryResults &results)
{
    //extract features
    KeyPoints_t keypoints;
    Features_t features;
    cv::Mat image = cv::imread(fileName.c_str());

    galif->compute(image, keypoints, features);

    //quantize
    Vec_f32_t query;
    quantize(features, vocabulary, query, quantizer);

    TF_simple tf;
    IDF_simple idf;

    std::vector<ResultItem_t> _results;
    //std::cout<< "query: " <<_results.size()<<std::endl;

    if(_numOfViews == 1)
    {
        index->query(query, tf, idf, _numOfResults, _results);
    }
    else {
        index->query(query, tf, idf, _numOfResults, _numOfViews, _results);
    }

    results.resize(_results.size());

    for(uint i = 0; i < _results.size(); i++) {

        results[i].ratio = _results[i].first;
        results[i].imageIndex = _results[i].second;
        results[i].imageName = files->getFilename(_results[i].second);
    }
}
