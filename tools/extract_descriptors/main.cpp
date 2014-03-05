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

void writeKeypoints(int index, int total)
{
    if(index +1 != total)
        cout << "Save keypoints " << index+1 << "/" << total <<"\r"<<std::flush;
    else
        cout << "Save keypoints " << index+1 << "/" << total <<"."<<std::endl;
}

void writeFeatures(int index, int total)
{
    if(index +1 != total)
        cout << "Save Features " << index+1 << "/" << total <<"\r"<<std::flush;
    else
        cout << "Save Features " << index+1 << "/" << total <<"."<<std::endl;
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

    write(vecKeypoints, std::string(argv[6]) + "keypoints", writeKeypoints);
    write(vecFeatures, std::string(argv[6]) + "features", writeFeatures);

    return 0;
}

