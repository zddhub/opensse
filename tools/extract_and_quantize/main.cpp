#include <iostream>
#include <fstream>
using namespace std;

#include "common/types.h"
#include "common/distance.h"
#include "io/reader_writer.h"
#include "io/filelist.h"
#include "quantize/quantizer.h"
#include "features/galif.h"

using namespace sse;

void usages() {
    cout << "Usages: " <<endl
         << "  extract_and_quantize -d rootDir -f filelist -v vocabulary -o output" <<endl
         << "  rootDir: \t image root directory" <<endl
         << "  filelist: \t image file list" <<endl
         << "  vocabulary: \t vocabulary" <<endl
         << "  output: \t output samples" <<endl;
}

int main(int argc, char *argv[])
{
    if(argc != 9) {
        usages();
        exit(1);
    }

    FileList files;
    files.setRootDir(argv[2]);

    files.load(argv[4]);

    Vocabularys_t vocabulary;
    read(argv[6], vocabulary, boost::bind(&print, _1, _2, "read vocabulary"));

    //QuantizerHard
    Quantizer_fn quantizer = QuantizerHard<Vec_f32_t, L2norm_squared<Vec_f32_t> >();

    PropertyTree_t params;
    boost::shared_ptr<Galif> galif = boost::make_shared<Galif>(params);


    ofstream fout(argv[8]);
    fout << files.size() <<endl;
    fout << vocabulary.size() <<endl;
    //Vocabularys_t samples;
    //samples.resize(vecFeatures.size());
    for(Index_t i = 0; i < files.size(); i++) {
        //Vec_f32_t vf;
        Vec_f32_t sample;
        cv::Mat image = cv::imread(files.getFilename(i));
        KeyPoints_t keypoints;
        Features_t features;
        galif->compute(image, keypoints, features);
        quantize(features, vocabulary, sample, quantizer);
        for(Index_t j = 0; j < sample.size(); j++) {
            fout << sample[j] << " ";
        }
        fout << endl;
        //quantize(vecFeatures[i], vocabulary, samples[i], quantizer);
        cout << "quantize " << i+1 << "/" << files.size() <<"\r"<<flush;
    }
    cout << "quantize " << files.size() << "/" << files.size() <<"."<<endl;

    //write(samples, argv[6], boost::bind(&print, _1, _2, "save representation"));
    fout.close();
    return 0;
}

