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
#include "invertedindex.h"

#include <queue>
#include <stack>
#include <fstream>

namespace sse {

InvertedIndex::InvertedIndex(uint vocabularySize)
    :_numOfWords(vocabularySize)
{
    init(_numOfWords);
}

void InvertedIndex::init(uint numOfWords)
{
    _ft.clear();
    _invertedList.clear();
    _weightList.clear();
    _uniqueTerms.clear();

    _ft.resize(numOfWords, 0);
    _invertedList.resize(numOfWords);
    _weightList.resize(numOfWords);

    _numOfDocuments = 0;
}

void InvertedIndex::addSample(const Vec_f32_t &sample)
{
    assert(sample.size() == _numOfWords);

    for(uint t= 0; t < sample.size(); t++) {
       //sample[t] > 0.0
       float f_dt = sample[t];
       if(f_dt > 0) {
            _ft[t]++;
            _invertedList[t].push_back(std::make_pair(_numOfDocuments, f_dt));

            _uniqueTerms.insert(t);
       }
    }

    _numOfDocuments ++;
}

void InvertedIndex::createIndex(const TF_interface &tf, const IDF_interface &idf)
{
    assert(_weightList.size() == _invertedList.size());

    // prepare for l2 normalization
    vector<float> documentLengths(_numOfDocuments, 0);

    for(uint termId = 0; termId < _numOfWords; termId++) {
        uint listSizeOfTerm = _invertedList[termId].size();
        _weightList[termId].resize(listSizeOfTerm);

        for(uint listId = 0; listId < listSizeOfTerm; listId ++) {
            uint docId = _invertedList[termId][listId].first;

            float _tf = tf(*this, termId);
            float _idf = idf(*this, termId, listId, docId);

            float weight = _tf*_idf;
            _weightList[termId][listId] = weight;

            // prepare for l2 normalization
            documentLengths[docId] += weight * weight;
        }
    }

    //l2 normalization
    for(uint i = 0; i < _numOfDocuments; i++) {
        documentLengths[i] = std::sqrt(documentLengths[i]);
    }

    for(uint termId = 0; termId < _numOfWords; termId++) {
        uint listSizeOfTerm = _invertedList[termId].size();
        _weightList[termId].resize(listSizeOfTerm);

        for(uint listId = 0; listId < listSizeOfTerm; listId ++) {
            uint docId = _invertedList[termId][listId].first;
            _weightList[termId][listId] /= documentLengths[docId];
        }
    }
}

void InvertedIndex::query(const Vec_f32_t &sample, const TF_interface &tf, const IDF_interface &idf,
                          uint numOfResults, std::vector<ResultItem_t> &results)
{
    numOfResults = std::min(numOfResults, _numOfDocuments);

    results.clear();
    results.reserve(numOfResults);

    // get query tf-idf weight
    InvertedIndex indexSample(_numOfWords);
    indexSample.addSample(sample);
    indexSample.createIndex(tf, idf);

    // accumulators A
    std::vector<float> A(_numOfDocuments, 0);
    const std::set<uint>& uniqueTerms = indexSample.uniqueTerms();
    std::set<uint>::const_iterator it = uniqueTerms.begin();
    for(; it != uniqueTerms.end(); ++it) {
        uint termId = *it;
        float wqt = indexSample.weightList()[termId][0];

        const std::vector<std::pair<uint, float> > &term_list = _invertedList[termId];
        const std::vector<float> &weight_list = _weightList[termId];

        for(uint listId = 0; listId < term_list.size(); listId++) {
            uint docId = term_list[listId].first;
            float wdt = weight_list[listId];
            A[docId] += wdt * wqt;
        }
    }

    std::priority_queue<ResultItem_t, std::vector<ResultItem_t>, std::greater<ResultItem_t> > queue;

    for(uint i = 0; i < _numOfDocuments; i++) {
        queue.push(ResultItem_t(A[i], i));
        if(queue.size() > numOfResults) {
            queue.pop();
        }
    }

    assert(queue.size() <= numOfResults);

    for(uint i = 0; i < numOfResults; i++) {
        results.push_back(queue.top());
        queue.pop();
    }

    std::reverse(results.begin(), results.end());
}

//many views
void InvertedIndex::query(const Vec_f32_t &sample, const TF_interface &tf, const IDF_interface &idf,
                          uint numOfResults, uint numOfViews, std::vector<ResultItem_t> &results)
{
    uint _numOfResults = std::min(numOfResults*numOfViews, _numOfDocuments);

    results.clear();
    results.reserve(numOfResults);

    // get query tf-idf weight
    InvertedIndex indexSample(_numOfWords);
    indexSample.addSample(sample);
    indexSample.createIndex(tf, idf);

    // accumulators A
    std::vector<float> A(_numOfDocuments, 0);
    const std::set<uint>& uniqueTerms = indexSample.uniqueTerms();
    std::set<uint>::const_iterator it = uniqueTerms.begin();
    for(; it != uniqueTerms.end(); ++it) {
        uint termId = *it;
        float wqt = indexSample.weightList()[termId][0];

        const std::vector<std::pair<uint, float> > &term_list = _invertedList[termId];
        const std::vector<float> &weight_list = _weightList[termId];

        for(uint listId = 0; listId < term_list.size(); listId++) {
            uint docId = term_list[listId].first;
            float wdt = weight_list[listId];
            A[docId] += wdt * wqt;
        }
    }

    std::priority_queue<ResultItem_t, std::vector<ResultItem_t>, std::greater<ResultItem_t> > queue;

    for(uint i = 0; i < _numOfDocuments; i++) {
        queue.push(ResultItem_t(A[i], i));
        if(queue.size() > _numOfResults) {
            queue.pop();
        }
    }

    assert(queue.size() <= _numOfResults);
    assert(numOfViews > 0);

    std::vector<uint> flags(_numOfDocuments);
    uint index = 0;

    std::stack<ResultItem_t> stack;
    for(uint i = 0; i < _numOfResults; i++) {
        stack.push(queue.top());
        queue.pop();
    }

    for(uint i = 0; i < _numOfResults; i++) {
        if(!flags[stack.top().second / numOfViews]) {
            results.push_back(stack.top());
            flags[stack.top().second / numOfViews] = 1;
            index ++;
            if(index == numOfResults)
                break;
        }
        stack.pop();
    }
}

void InvertedIndex::load(const std::string &filename)
{
    std::ifstream in(filename.c_str());
    in >> _numOfWords;
    init(_numOfWords);

    in >> _numOfDocuments;

    for(uint i = 0; i < _ft.size(); i++) {
        in >> _ft[i];
    }

    for(uint i = 0; i < _numOfWords; i++) {
        uint sizeOfLists;
        in >> sizeOfLists;
        _invertedList[i].resize(sizeOfLists);
        _weightList[i].resize(sizeOfLists);
        for(uint j = 0; j < _invertedList[i].size(); j++) {
            uint docId = 0;
            float f_dt = 0.0;
            in >> docId; in >> f_dt;
            _invertedList[i][j] = std::make_pair(docId, f_dt);
            in >> _weightList[i][j];
        }
    }
    in.close();
}

void InvertedIndex::save(const std::string &filename)
{
    std::ofstream out(filename.c_str());

    out << _numOfWords <<std::endl;
    out << _numOfDocuments <<std::endl;

    for(uint i = 0; i < _ft.size(); i++) {
        out << _ft[i] << " ";
    }
    out << std::endl;

    for(uint i = 0; i < _numOfWords; i++) {
        out << _invertedList[i].size() <<std::endl;
        for(uint j = 0; j < _invertedList[i].size(); j++) {
            out << _invertedList[i][j].first << " " << _invertedList[i][j].second << " ";
            out << _weightList[i][j] << " ";
        }
        out << std::endl;
    }

    out.close();
}

} //namespace sse
