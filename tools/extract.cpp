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
#include <fstream>
#include <queue>

using namespace std;

#include "opensse/opensse.h"

using namespace sse;

void usages() {
    cout << "Usages: sse extract -f filelist -o output" <<endl
         << "  This command extracts feature descriptors of images" <<endl
         << "  The options are as follows:" <<endl
         << "  -f\t image \033[4mfilelist\033[0m" <<endl
         << "  -o\t \033[4moutput\033[0m" <<endl;
}

std::mutex m;
std::condition_variable cond;
std::queue<Features_t> FeaturesQueue;
bool finished = false;

void extracter(const FileList &files) {

    Galif *galif = new Galif();

    for(uint i = 0; i < files.size(); i++) {
        {
            std::lock_guard<std::mutex> lock(m);

            KeyPoints_t keypoints;
            Features_t features;
            cv::Mat image = cv::imread(files.getFilename(i));

            galif->compute(image, keypoints, features);
            FeaturesQueue.push(features);
        }

        cond.notify_all();
    }
    {
        std::lock_guard<std::mutex> lock(m);
        finished = true;
    }
    cond.notify_all();
}

void writer(const std::string &filename, uint sizes) {
    ofstream ft_out(filename.c_str());
    ft_out << sizes << endl;
    uint i = 0;
    while (true) {
        std::unique_lock<std::mutex> lock(m);
        cond.wait(lock, []{ return finished || !FeaturesQueue.empty(); });
        while (!FeaturesQueue.empty()) {
            write(FeaturesQueue.front(), ft_out);
            cout << "Extract descriptors " << i++ << "/" << sizes << "\r" << flush;
            FeaturesQueue.pop();
        }
        if (finished) break;
    }
    cout << "Extract descriptors "<< i << "/" << sizes <<  "." <<endl;
    ft_out.close();
}

int main(int argc, char *argv[])
{
    if(argc != 5) {
        usages();
        exit(1);
    }

    FileList files;
    files.load(argv[2]);

    std::string ft_file = std::string(argv[4]);

    std::thread t1(extracter, files);
    std::thread t2(writer, ft_file, files.size());
    t1.join();
    t2.join();

    return 0;
}

