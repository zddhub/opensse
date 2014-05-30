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

#include "searchengine.h"
#include "sketchsearcher.h"

#include <QFile>
#include <QDir>

void writeResults(const std::string &queryfile, const std::string &resultdir, const QueryResults &results) {
    //cout << queryfile<<endl;
    int start = queryfile.rfind('/') < 0 ? 0 : queryfile.rfind('/');
    int end = queryfile.rfind('.');
    std::string filename = queryfile.substr(start + 1,end - start-1);
    QDir directory;
    directory.mkpath(resultdir.c_str());
    //cout << "start: " << start <<" end: " <<end << " filename: " << filename <<flush<<endl;

    std::string root = resultdir;
    std::string filedir = root.append("/");
    std::string filepath = filedir.append(filename);
    ofstream fout(filepath.c_str());
    fout << filename<<endl;

    for(uint i = 0; i < results.size(); i++) {
        //2012 use is
        //cout << "cmp: " << (results[i].imageName.find("Extended") == std::string::npos ? 1 : 0) <<" " <<results[i].imageName<<endl;
//        if(results[i].imageName.find("Extended") == std::string::npos)
//        {
//            continue;
//        }
        //cout << results[i].imageName<<endl;
        int st = results[i].imageName.rfind('/') < 0 ? 0 : results[i].imageName.rfind('/');
        int ed = results[i].imageName.rfind("view");

        std::string temp = results[i].imageName.substr(0, st);
        st = temp.rfind('/') < 0 ? 0 : temp.rfind('/');
        std::string name = temp.substr(st+1, ed - st -1);
        //fout << results[i].imageName<<endl;
        fout << name << " " <<results[i].ratio <<endl;
    }
}

void usages() {
    cout << "Usages: " <<endl
         << "  test_search -p paramsfile -i searchfilelist -r searchfileroot -o resultdir" <<endl;
}

int main(int argc, char **argv)
{
    if(argc < 8) {
        usages();
        exit(1);
    }

    cout << "init engine ...\r" <<flush;
    sse::PropertyTree_t params;
    boost::property_tree::read_json(argv[2], params);
    SearchEngine *searchEngine = new SketchSearcher(params);
    cout << "init engine done" <<endl;

    sse::FileList filelist;
    filelist.setRootDir(argv[6]);
    filelist.load(argv[4]);

    for(uint i = 0; i < filelist.size(); i++) {

        QueryResults results;
        searchEngine->query(filelist.getFilename(i), results);

        writeResults(filelist.getFilename(i), argv[8], results);

        cout << "search " << i+1 << "/"<<filelist.size() << "\r" << flush;
    }

    cout << "search " << filelist.size() << "/"<<filelist.size() << " done."<< endl;

    delete searchEngine;
    return 0;
}

