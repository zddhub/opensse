#include "invertedindex.h"


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

    _ft.resize(numOfWords, 0);
    _invertedList.resize(numOfWords);
    _weightList.resize(numOfWords);

    _numOfDocuments = 0;
}

void InvertedIndex::addSample(const Vec_f32_t &sample)
{
    assert(sample.size() == _numOfWords);

    for(int t= 0; t < sample.size(); t++) {
       //sample[t] > 0.0
       float f_dt = sample[t];
       if(f_dt > 0) {
            _ft[t]++;
            _invertedList[t].push_back(std::make_pair(_numOfDocuments, f_dt));
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
