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
#ifndef TFIDF_H
#define TFIDF_H

#include "common/types.h"

namespace sse {

class InvertedIndex;

class TF_interface {
public:
    virtual float operator() (const InvertedIndex &index, uint termId) const = 0;
};

class IDF_interface {
public:
    virtual float operator() (const InvertedIndex &index, uint termId, uint listId, uint docId) const = 0;
};

class TF_simple : public TF_interface {
public:
    float operator() (const InvertedIndex &index, uint termId) const;
};

class IDF_simple : public IDF_interface {
public:
    float operator() (const InvertedIndex &index, uint termId, uint listId, uint docId) const;
};

} //namespace sse

#endif // TFIDF_H
