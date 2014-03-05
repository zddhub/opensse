#include <iostream>

using namespace std;

#include "common/types.h"
#include "common/distance.h"
#include "io/reader_writer.h"
#include "quantize/quantizer.h"

using namespace sse;

void usages() {
    cout << "Usages: " <<endl
         << "  quantize -v vocabulary -f features -o output" <<endl
         << "  vocabulary: \t vocabulary file" <<endl
         << "  features: \t features file" <<endl
         << "  output: \t output file" <<endl;
}


//Quantize one image
void quantize(const Features_t &features, const Vocabularys_t &vocabulary,
              Vec_f32_t &vf, Quantizer_fn &quantizer)
{
    Vocabularys_t quantized_samples;
    quantize_samples_parallel(features, vocabulary, quantized_samples, quantizer);

    build_histvw(quantized_samples, vocabulary.size(), vf, false);
}

int main(int argc, char *argv[])
{
    if(argc != 7) {
        usages();
        exit(1);
    }

    std::vector<Features_t> vecFeatures;
    read(argv[4], vecFeatures, boost::bind(&print, _1, _2, "read features"));

    Vocabularys_t vocabulary;
    read(argv[2], vocabulary, boost::bind(&print, _1, _2, "read vocabulary"));

    //QuantizerHard
    Quantizer_fn quantizer = QuantizerHard<Vec_f32_t, L2norm_squared<Vec_f32_t> >();

    Vocabularys_t samples;
    for(Index_t i = 0; i < vecFeatures.size(); i++) {
        Vec_f32_t vf;
        quantize(vecFeatures[i], vocabulary, vf, quantizer);
        samples.push_back(vf);
        cout << "quantize " << i+1 << "/" << vecFeatures.size() <<"\r"<<flush;
    }
    cout << "quantize " << vecFeatures.size() << "/" << vecFeatures.size() <<"."<<endl;

    write(samples, argv[6], boost::bind(&print, _1, _2, "save representation"));

    return 0;
}

