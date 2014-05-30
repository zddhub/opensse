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
#ifndef INVERTEDINDEX_H
#define INVERTEDINDEX_H

#include "common/types.h"
#include "tfidf.h"

namespace sse {

class InvertedIndex
{
public:
    InvertedIndex(uint vocabularySize = 0);
    void addSample(const Vec_f32_t &sample);
    void createIndex(const TF_interface &tf, const IDF_interface &idf);
    void query(const Vec_f32_t &sample, const TF_interface &tf, const IDF_interface &idf,
               uint numOfResults, std::vector<ResultItem_t> &results);
    void query(const Vec_f32_t &sample, const TF_interface &tf, const IDF_interface &idf,
               uint numOfResults, uint numOfViews, std::vector<ResultItem_t> &results);
    void save(const std::string& filename);
    void load(const std::string& filename);

    inline const std::vector<uint>& ft() const { return _ft; }
    inline const std::vector<std::vector<std::pair<uint, float> > >& invertedList() const { return _invertedList; }
    inline const std::vector<std::vector<float> >& weightList() const { return _weightList; }
    inline const std::set<uint>& uniqueTerms() const { return _uniqueTerms; }
    inline uint numOfDocuments() const { return _numOfDocuments; }
private:
    void init(uint numOfWords = 0);

    uint _numOfWords;

    //Document-level inverted index
    //term t : [0, vocabularySize)
    //_ft: a count ft of the documents containing t
    std::vector<uint> _ft;
    //_invertedList: Inverted list for terms:
    // the entry for each term t is composed of a document identifier d and a document frequency f_dt
    std::vector<std::vector<std::pair<uint, float> > > _invertedList;
    // tf-idf weights
    //_weightList[term_id][list_id] = tf-idf(_invertedList[term_id][list_id])
    std::vector<std::vector<float> > _weightList;

    //record index when
    std::set<uint> _uniqueTerms;

    uint _numOfDocuments;
};

}


#endif // INVERTEDINDEX_H
