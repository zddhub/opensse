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
#include "tfidf.h"

#include "common/types.h"
#include "invertedindex.h"

namespace sse {

float TF_simple::operator() (const InvertedIndex &index, uint termId) const
{
    uint ft = index.ft()[termId];
    return std::log(1 + index.numOfDocuments() / static_cast<float>(ft));
}

float IDF_simple::operator() (const InvertedIndex &index, uint termId, uint listId, uint /*docId*/) const
{
    float f_dt = index.invertedList()[termId][listId].second;
    return 1 + std::log(f_dt);
}

} //namespace sse
