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
#include <iostream>

using namespace std;

#include "common/types.h"
#include "common/distance.h"
#include "features/galif.h"
#include "quantize/quantizer.h"
#include "index/invertedindex.h"
#include "index/tfidf.h"
#include "io/reader_writer.h"

#include <boost/shared_ptr.hpp>
#include <boost/lexical_cast.hpp>
using namespace sse;

void usage() {
    cout << "Usage: test_similarity image1 image2 k n peak fu wv feature_size vocabulary"<<endl;
}

int main(int argc, char **argv)
{
    if(argc < 10) {
        usage();
        exit(-1);
    }

    cv::Mat image1 = cv::imread(argv[1]);
    cv::Mat image2 = cv::imread(argv[2]);

    uint k;//numofOrient
    uint n;//titles
    try {
        k = boost::lexical_cast<uint>(argv[3]);
        n = boost::lexical_cast<uint>(argv[4]);
    } catch (boost::bad_lexical_cast&) {
        std::cerr << "bad parameter value: "<< argv[3] <<endl;
        std::cerr << "bad parameter value: "<< argv[4] <<endl;
    }

    double peak;
    double fu;
    double wv;
    double size;

    try {
        peak = boost::lexical_cast<double>(argv[5]);
        fu = boost::lexical_cast<double>(argv[6]);
        wv = boost::lexical_cast<double>(argv[7]);
        size = boost::lexical_cast<double>(argv[8]);
    } catch (boost::bad_lexical_cast&) {
        std::cerr << "bad parameter value: "<< argv[5] <<endl;
        std::cerr << "bad parameter value: "<< argv[6] <<endl;
        std::cerr << "bad parameter value: "<< argv[7] <<endl;
        std::cerr << "bad parameter value: "<< argv[8] <<endl;
    }

    double line_width = fu/256.0;
    double lambda = fu/wv;

    Galif *galif = new Galif(
            256,
            k,
            n,
            peak, line_width, lambda,
            size, true,
            "l2", "grid",
            625);

    KeyPoints_t keypoints1;
    Features_t features1;
    galif->compute(image1, keypoints1, features1);

    KeyPoints_t keypoints2;
    Features_t features2;
    galif->compute(image2, keypoints2, features2);

    Vocabularys_t vocabulary;
    read(argv[9], vocabulary);

    //QuantizerHard
    Quantizer_fn quantizer = sse::QuantizerHard<Vec_f32_t, L2norm_squared<Vec_f32_t> >();

    Vec_f32_t hist1, hist2;
    quantize(features1, vocabulary, hist1, quantizer);
    quantize(features2, vocabulary, hist2, quantizer);

    InvertedIndex index(vocabulary.size());
    index.addSample(hist1);
    TF_simple tf;
    IDF_simple idf;
    index.createIndex(tf, idf);

    std::vector<ResultItem_t> results;
    index.query(hist2, tf, idf, 1, results);

    float similarity = results[0].first;
    cout << similarity<<endl;

    delete galif;
    return 0;
}

