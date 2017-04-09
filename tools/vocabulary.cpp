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

#include "opensse/opensse.h"
#include "vocabulary/kmeans.h"

using namespace sse;

void usages()
{
    cout << "Usages: sse vocabulary -f features -n numclusters -o output" <<endl
         << "  This command generates \033[4mnumclusters\033[0m vocabulary using \033[4mfeatures\033[0m" <<endl
         << "  The options are as follows:" <<endl
         << "  -f\t \033[4mfeatures\033[0m file" <<endl
         << "  -n\t the number of cluster centers"<<endl
         << "  -o\t \033[4moutput\033[0m file" <<endl;
}

int main(int argc, char* argv[])
{
    if(argc != 7) {
        usages();
        exit(1);
    }

    int maxiter = 20;
    double minChangesfraction = 0.01;

    uint numclusters = atoi(argv[4]);
    std::cout << numclusters <<endl;

    Features_t samples;
    readSamplesForCluster(argv[2], samples, print, "read samples");

    typedef Kmeans<Features_t, L2norm_squared<Vec_f32_t> > Cluster;

    cout << "cluster ..." <<endl;
    Vocabularys_t centers;
    Cluster cluster(samples, numclusters);
    cluster.run(maxiter, minChangesfraction);
    centers = cluster.centers();

    write(centers, argv[6], print, "write vocabulary");

    return 0;
}

