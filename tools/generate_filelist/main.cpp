#include <iostream>

using namespace std;

#include "io/filelist.h"
#include <boost/lexical_cast.hpp>

void usages() {
    cout << "Usages:" <<endl
         << "  generate_filelist -d rootdir -f filter -o filelist [-r num_sample]" <<endl
         << "  rootdir:\t file root directory" <<endl
         << "  filter:\t file name filter, eg: \"*.png\" "<<endl
         << "  num_sample:\t num of samples" <<endl;
}

int main(int argc, char *argv[])
{
    if(argc != 7 && argc != 9) {
        usages();
        exit(1);
    }

    //check input
    //check_input_valid(argv);

    sse::FileList files(argv[2]);
    std::vector<std::string> filters;
    filters.push_back(argv[4]);
    files.lookupDir(filters);

    if(argc == 9) {
        uint value;
        try {
            value = boost::lexical_cast<uint>(argv[8]);
        } catch (boost::bad_lexical_cast&) {
            std::cerr << "bad parameter value: "<< argv[8] <<endl;
        }

        files.randomSample(value, time(0));
    }

    files.store(argv[6]);

    return 0;
}

