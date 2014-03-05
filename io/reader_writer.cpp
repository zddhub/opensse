#include "reader_writer.h"

#include <fstream>

namespace sse {

void write(const std::vector<std::vector<Vec_f32_t> > &data, const std::string &filename, Callback_fn callback)
{
    std::ofstream out(filename.c_str());
    out << data.size() <<std::endl;

    for(uint n = 0; n < data.size(); n++) {
        out << data[n].size() <<std::endl;
        out << data[n][0].size() <<std::endl;
        for(uint i = 0; i < data[n].size(); i++) {
            for(uint j = 0; j < data[n][i].size(); j++) {
                out << data[n][i][j] << " ";
            }
            out << std::endl;
        }

        if(callback)
            callback(n, data.size());
    }
    out.close();
}

void read(const std::string &filename, std::vector<std::vector<Vec_f32_t> > &data, Callback_fn callback)
{
    std::ifstream in(filename.c_str());

    uint size = 0;
    in >> size;
    for(uint n = 0; n < size; n++) {
        uint row = 0;
        in >> row;
        uint col = 0;
        in >> col;

        std::vector<Vec_f32_t> vv;

        for(uint i = 0; i < row; i++) {
            Vec_f32_t vf;
            float temp;
            for(uint j = 0; j < col; j++) {
                in >> temp;
                vf.push_back(temp);
            }
            vv.push_back(vf);
        }
        data.push_back(vv);

        if(callback)
            callback(n, size);
    }

    in.close();
}

} //namespace sse
