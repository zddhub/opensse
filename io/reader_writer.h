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
#ifndef READER_WRITER_H
#define READER_WRITER_H

#include "common/types.h"

#include <fstream>

namespace sse {

typedef boost::function<void (int, int, const std::string&)> Callback_fn;

//Callback function, show read or write progress
void print(int index, int total, const std::string& info);

void write(const std::vector<std::vector<Vec_f32_t> > &data, const std::string &filename,
           Callback_fn callback = Callback_fn());

void read(const std::string &filename, std::vector<std::vector<Vec_f32_t> > &data,
          Callback_fn callback = Callback_fn()) ;

//Read Features_t in one dimensional ready for cluster
//Note that: firt read filesize
void readSamplesForCluster(const std::string &filename, Features_t &samples,
                           Callback_fn callback = Callback_fn()) ;

//Read and read
void write(const std::vector<Vec_f32_t> &vv, const std::string &filename,
           Callback_fn callback = Callback_fn());

void read(const std::string &filename, std::vector<Vec_f32_t> &vv,
          Callback_fn callback = Callback_fn());

//Read and read features vector: std::vector<Features_t>
//append method can save memory
void read(std::ifstream &in, std::vector<Vec_f32_t> &vv,
          Callback_fn callback = Callback_fn());

void write(const std::vector<Vec_f32_t> &vv, std::ofstream &out,
           Callback_fn callback = Callback_fn());


} //namespace sse

#endif // READER_WRITER_H
