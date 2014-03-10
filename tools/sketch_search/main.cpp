#include <iostream>
using namespace std;

#include <fstream>

#include "common/types.h"
#include "common/distance.h"
#include "features/galif.h"
#include "quantize/quantizer.h"
#include "io/reader_writer.h"
#include "index/invertedindex.h"

using namespace sse;

void usages()
{
    cout << "Usages: " <<endl
         << "  sketch_search -i indexfile -v vocabulary -s sketchpath -o output" <<endl
         << "  indexfile: \t inverted index file" <<endl
         << "  vocabulary: \t vocabulary file"<<endl
         << "  sketchpath: \t sketch path"<<endl
         << "  output: \t output file" <<endl;
}

int main(int argc, char *argv[])
{
    if(argc != 9) {
        usages();
        exit(1);
    }
    InvertedIndex index;
    index.load(argv[2]);

    //extract features
    KeyPoints_t keypoints;
    Features_t features;
    cv::Mat image = cv::imread(argv[6]);

    PropertyTree_t params;
    boost::shared_ptr<Galif> galif = boost::make_shared<Galif>(params);
    galif->compute(image, keypoints, features);


    Vocabularys_t vocabulary;
    read(argv[4], vocabulary, boost::bind(&print, _1, _2, "read vocabulary"));

    //quantize
    Vec_f32_t query;
    Quantizer_fn quantizer = QuantizerHard<Vec_f32_t, L2norm_squared<Vec_f32_t> >();
    quantize(features, vocabulary, query, quantizer);

    std::vector<ResultItem_t> results;
    TF_simple tf;
    IDF_simple idf;
    index.query(query, tf, idf, -1, results);

    //cout << "results:"<<endl;

    ofstream out(argv[8]);
    for(uint i = 0; i < results.size(); i++) {
        out << results[i].first << " " << results[i].second<<endl;
    }
    out.close();
    return 0;
}

