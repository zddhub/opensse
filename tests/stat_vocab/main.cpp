#include <iostream>
using namespace std;

#include "common/types.h"
#include "io/reader_writer.h"
#include <fstream>

using namespace sse;

void usages() {
    cout << "Usages: " <<endl
         << "  stat_vocab -s samples -o output" <<endl
         << "  samples: \t stat samples file" <<endl
         << "  output: \t output file" <<endl;
}

int main(int argc, char* argv[])
{
    if(argc != 5) {
        usages();
        exit(1);
    }

    Samples_t samples;
    read(argv[2], samples, boost::bind(&print, _1, _2, "read samples "));

    assert(samples.size() > 0);

    uint vocabularySize = samples[0].size();

    ofstream fout(argv[4]);

    cout << "stat ..." << "\r" <<std::flush;
    for(uint i = 0; i < samples.size(); i++) {
        uint count = 0;
        for(uint j = 0; j < samples[i].size(); j++) {
            if(samples[i][j] == 0)
                count++;
        }
        fout << count <<endl;
    }

    fout.close();
    cout << "stat done." <<endl;

    return 0;
}
