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
