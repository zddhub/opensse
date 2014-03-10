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
