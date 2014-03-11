#include "reader_writer.h"

#include <fstream>

namespace sse {

void print(int index, int total, const std::string& info)
{
    if(index +1 != total)
        std::cout << info.c_str() << " " << index+1 << "/" << total <<"\r"<<std::flush;
    else
        std::cout << info.c_str() << " " << index+1 << "/" << total <<"."<<std::endl;
}

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
            callback(n, data.size(), "");
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
            callback(n, size, "");
    }

    in.close();
}

void readSamplesForCluster(const std::string &filename, Features_t &samples, Callback_fn callback)
{
    std::ifstream in(filename.c_str());

    //See: read below function in reader_writer.cpp
    //void write(const std::vector<Vec_f32_t> &vv, std::ofstream &out,
    //Callback_fn callback = Callback_fn());
    //
    uint filesize  = 0;
    in >> filesize;

    uint size = 0;
    in >> size;

    for(uint n = 0; n < size; n++) {
        uint row = 0;
        in >> row;
        uint col = 0;
        in >> col;

         for(uint i = 0; i < row; i++) {
            Vec_f32_t vf;
            float temp;
            for(uint j = 0; j < col; j++) {
                in >> temp;
                vf.push_back(temp);
            }
            samples.push_back(vf);
        }

        if(callback)
            callback(n, size, "");
    }

    in.close();
}

void write(const std::vector<Vec_f32_t> &vv, const std::string &filename, Callback_fn callback)
{
    std::ofstream out(filename.c_str());
    out << vv.size() <<std::endl;
    out << vv[0].size() <<std::endl;

    uint row = vv.size();
    uint col = vv[0].size();
    for(uint i = 0; i < row; i++) {
        for(uint j = 0; j < col; j++) {
            out << vv[i][j] << " ";
        }
        out << std::endl;
        if(callback)
            callback(i, row, "");
    }
    out.close();
}

void read(const std::string &filename, std::vector<Vec_f32_t> &vv, Callback_fn callback)
{
    std::ifstream in(filename.c_str());
    uint row = 0;
    uint col = 0;
    in >> row;
    in >> col;

    for(uint i = 0; i < row; i++) {
        Vec_f32_t vf;
        for(uint j = 0; j < col; j++) {
            float temp;
            in >> temp;
            vf.push_back(temp);
        }
        vv.push_back(vf);

        if(callback)
            callback(i, row, "");
    }

    in.close();
}

void write(const std::vector<Vec_f32_t> &vv, std::ofstream &out, Callback_fn callback)
{
    out << vv.size() <<std::endl;
    out << vv[0].size() <<std::endl;

    uint row = vv.size();
    uint col = vv[0].size();
    for(uint i = 0; i < row; i++) {
        for(uint j = 0; j < col; j++) {
            out << vv[i][j] << " ";
        }
        out << std::endl;
        if(callback)
            callback(i, row, "");
    }
}

void read(std::ifstream &in, std::vector<Vec_f32_t> &vv, Callback_fn callback)
{
    uint row = 0;
    uint col = 0;
    in >> row;
    in >> col;

    for(uint i = 0; i < row; i++) {
        Vec_f32_t vf;
        for(uint j = 0; j < col; j++) {
            float temp;
            in >> temp;
            vf.push_back(temp);
        }
        vv.push_back(vf);

        if(callback)
            callback(i, row, "");
    }
}

} //namespace sse
