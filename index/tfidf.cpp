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
