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

//SketchSearcher::SketchSearcher(const PropertyTree_t &parameters)
//    : _indexFile(parse<std::string>(parameters, "searcher.indexfile", "./data/indexfile"))
//    , _vocabularyFile(parse<std::string>(parameters, "searcher.vocabulary", "./data/vocabulary"))
//    , _rootdir(parse<std::string>(parameters, "searcher.rootdir", "/home/zdd/Database/sketches"))
//    , _fileList(parse<std::string>(parameters, "searcher.filelist", "./data/filelist"))
//    , _numOfResults(parse<uint>(parameters, "searcher.results_num", 25))
SketchSearcher::SketchSearcher(const PropertyTree_t &parameters)
    : _indexFile(parse<std::string>(parameters, "searcher.indexfile", "/tmp/SketchSearchDemo/data/model_indexfile"))
    , _vocabularyFile(parse<std::string>(parameters, "searcher.vocabulary", "/tmp/SketchSearchDemo/data/vocabulary"))
    , _rootdir(parse<std::string>(parameters, "searcher.rootdir", "/tmp/SketchSearchDemo/"))
    , _fileList(parse<std::string>(parameters, "searcher.filelist", "/tmp/SketchSearchDemo/data/model_filelist"))
    , _numOfResults(parse<uint>(parameters, "searcher.results_num", 25))
    , _numOfViews(parse<uint>(parameters, "searcher.views_num", 1))
{
    index = boost::make_shared<InvertedIndex>();
    index->load(_indexFile);

    read(_vocabularyFile, vocabulary);

    PropertyTree_t defaultParams;

    galif = boost::make_shared<Galif>(defaultParams);

    quantizer = QuantizerHard<Vec_f32_t, L2norm_squared<Vec_f32_t> >();

    files = boost::make_shared<FileList>(_rootdir);
    files->load(_fileList);

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
