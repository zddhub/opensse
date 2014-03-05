#ifndef READER_WRITER_H
#define READER_WRITER_H

#include "common/types.h"

namespace sse {

typedef boost::function<void (int, int)> Callback_fn;

void write(const std::vector<std::vector<Vec_f32_t> > &data, const std::string &filename,
           Callback_fn callback = Callback_fn());

void read(const std::string &filename, std::vector<std::vector<Vec_f32_t> > &data,
          Callback_fn callback = Callback_fn()) ;

} //namespace sse

#endif // READER_WRITER_H
