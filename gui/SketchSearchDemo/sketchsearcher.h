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
};

#endif // SKETCHSEARCHER_H
