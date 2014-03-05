#include <iostream>

using namespace std;

#include "io/filelist.h"
#include "io/reader_writer.h"

#include "features/galif.h"

using namespace sse;

void usages() {
    cout << "Usages: " <<endl
         << "  extract_descriptors -d rootDir -f filelist -o output" <<endl
         << "  rootDir: \t image root directory" <<endl
         << "  filelist: \t image file list" <<endl
         << "  output: \t output prefix" <<endl;
}

int main(int argc, char *argv[])
{
    if(argc != 7) {
        usages();
        exit(1);
    }

    FileList files;
    files.setRootDir(argv[2]);

    files.load(argv[4]);

    PropertyTree_t params;
    boost::shared_ptr<Galif> galif = boost::make_shared<Galif>(params);

    std::vector<KeyPoints_t> vecKeypoints;
    std::vector<Features_t> vecFeatures;
    for(uint i = 0; i < files.size(); i++) {
        KeyPoints_t keypoints;
        Features_t features;
        cv::Mat image = cv::imread(files.getFilename(i));
        galif->compute(image, keypoints, features);
        vecKeypoints.push_back(keypoints);
        vecFeatures.push_back(features);
        cout << "Extract descriptors " << i+1 << "/" << files.size() <<"\r" << flush;
    }
    cout << "Extract descriptors "<< files.size() << "/" << files.size() <<  "." <<endl;

    write(vecKeypoints, std::string(argv[6]) + "keypoints", boost::bind(&print, _1, _2, "save keypoints"));
    write(vecFeatures, std::string(argv[6]) + "features", boost::bind(&print, _1, _2, "save features"));

    return 0;
}

