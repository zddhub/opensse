#include <iostream>

using namespace std;

#include "common/types.h"
#include "vocabulary/kmeans.h"

#include <boost/lexical_cast.hpp>

#include "io/reader_writer.h"

using namespace sse;

void usages()
{
    cout << "Usages: " <<endl
         << "  generate_vocabulary -f features -n numclusters -o output" <<endl
         << "  features: \t features file" <<endl
         << "  numclusters: \t the number of cluster centers"<<endl
         << "  output: \t output file" <<endl;
}

int main(int argc, char* argv[])
{
    if(argc != 7) {
        usages();
        exit(1);
    }

    int maxiter = 20;
    double minChangesfraction = 0.01;

    uint numclusters = 0;
    try {
        numclusters = boost::lexical_cast<uint>(argv[4]);
    } catch (boost::bad_lexical_cast&) {
        std::cerr << "bad parameter value: "<< argv[4] <<endl;
    }

    Features_t samples;
    readSamplesForCluster(argv[2], samples, boost::bind(&print, _1, _2, "read samples"));

    typedef Kmeans<Features_t, L2norm_squared<Vec_f32_t> > Cluster;

    cout << "cluster ..." << "\r";
    Vocabularys_t centers;
    Cluster cluster(samples, numclusters);
    cluster.run(maxiter, minChangesfraction);
    centers = cluster.centers();

    write(centers, argv[6], boost::bind(&print, _1, _2, "write vocabulary"));

    return 0;
}

