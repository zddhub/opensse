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
#ifndef TRIANGLEMESH_H
#define TRIANGLEMESH_H

#include "Vec.h"
#include <vector>
#include "Color.h"

class TriangleMesh
{
public:
    //
    //基本数据类型
    //
    //Face
    struct Face {
        int v[3];

        Face() {}
        Face(const int &v0, const int &v1, const int &v2)
                { v[0] = v0; v[1] = v1; v[2] = v2; }
        Face(const int *v_)
                { v[0] = v_[0]; v[1] = v_[1]; v[2] = v_[2]; }
        int &operator[] (int i) { return v[i]; }
        const int &operator[] (int i) const { return v[i]; }
        operator const int * () const { return &(v[0]); }
        operator const int * () { return &(v[0]); }
        operator int * () { return &(v[0]); }
        int indexof(int v_) const
        {
                return (v[0] == v_) ? 0 :
                       (v[1] == v_) ? 1 :
                       (v[2] == v_) ? 2 : -1;
        }
    };

    struct BSphere {
        point center;
        float r;
        bool valid;
        BSphere() : valid(false)
                {}
    };

    //
    // Enums
    //
    enum TstripRep { TSTRIP_LENGTH, TSTRIP_TERM };

    //顶点和面片
    std::vector<point> vertices;
    std::vector<Face> faces;

    std::vector<uvec3> colors;

    void need_faces()
    {
        if(!faces.empty())
            return;
        if(!tstrips.empty())
            unpack_tstrips();
    }

    //模型边界：球体
    BSphere bsphere;
    void need_bsphere();

    //连通性
    // Connectivity structures:
    //  For each vertex, all neighboring vertices
    std::vector< std::vector<int> > neighbors;
    //  For each vertex, all neighboring faces
    std::vector< std::vector<int> > adjacentfaces;
    //  For each face, the three faces attached to its edges
    //  (for example, across_edge[3][2] is the number of the face
    //   that's touching the edge opposite vertex 2 of face 3)
    //  across_edge[3][2]表示三角形面片编号(faces[across_edge[3][2]]为该三角形面片)
    //  该三角形与第4(下标为3)个三角形(faces[3])有公共边，公共边为faces[3]中下标为[2]的顶点所对的边
    std::vector<Face> across_edge;

    void need_neighbors();
    void need_adjacentfaces();
    void need_across_edge();

    //
    // Compute all this stuff...
    //
    //三角形带，将所有面都化成三角形
    std::vector<int> tstrips;

    void need_tstrips();
    void convert_strips(TstripRep rep);
    void unpack_tstrips();

    //法线
    std::vector<vec> normals;
    void need_normals();


    //主曲率和方向
    std::vector<vec> pdir1, pdir2;
    std::vector<float> curv1, curv2;

    std::vector<float> pointareas;
    std::vector<vec> cornerareas;
    void need_pointareas();
    void need_curvatures();

    //曲率的微分
    std::vector< Vec<4,float> > dcurv;
    void need_dcurv();

    //统计信息
    float feature_size();

protected:
    static bool read_helper(const char *filename, TriangleMesh *mesh);
public:
    static TriangleMesh *read(const char *filename);

    //
    // Debugging
    //

    // Debugging printout, controllable by a "verbose"ness parameter
    static int verbose;
    static void set_verbose(int);
    static void (*dprintf_hook)(const char *);
    static void set_dprintf_hook(void (*hook)(const char *));
    static void dprintf(const char *format, ...);

    // Same as above, but fatal-error printout
    static void (*eprintf_hook)(const char *);
    static void set_eprintf_hook(void (*hook)(const char *));
    static void eprintf(const char *format, ...);
};

#endif // TRIANGLEMESH_H
