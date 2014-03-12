//三角网格的连通性


#include "trianglemesh.h"

#include <algorithm>
using namespace std;


//查找每个顶点的直接邻居顶点
void TriangleMesh::need_neighbors()
{
    if(!neighbors.empty())
        return;

    need_faces();
    if(faces.empty())
        return;

    dprintf("Find vertex neighbors...");
    int nv = vertices.size(), nf = faces.size();

    vector<int> numneighbors(nv);
    for(int i = 0; i < nf; i++)
    {
        numneighbors[faces[i][0]]++;
        numneighbors[faces[i][1]]++;
        numneighbors[faces[i][2]]++;
    }

    //vector 的reserve增加了vector的capacity，但是它的size没有改变！而resize改变了vector的capacity同时也增加了它的size！

    neighbors.resize(nv);
    for(int i = 0; i < nv; i++)
    {
        neighbors[i].reserve(numneighbors[i] + 2);//防止边界溢出
    }

    for(int i = 0; i < nf; i++)
    {
        for(int j = 0; j < 3; j++)
        {
            vector<int> &me = neighbors[faces[i][j]];
            int n1 = faces[i][(j+1)%3]; //另外两个顶点
            int n2 = faces[i][(j+2)%3];
            if(find(me.begin(), me.end(), n1) == me.end())
                me.push_back(n1);
            if(find(me.begin(), me.end(), n2) == me.end())
                me.push_back(n2);
        }
    }

    dprintf("Done.\n");
}

//每个顶点邻接的面片
void TriangleMesh::need_adjacentfaces()
{
    if(!adjacentfaces.empty())
        return;

    need_faces();
    if(faces.empty())
        return;

    dprintf("Find adjacent faces...");

    int nv = vertices.size(), nf = faces.size();

    vector<int> numadjacentfaces(nv);
    for(int i = 0; i < nf; i++)
    {
        numadjacentfaces[faces[i][0]]++;
        numadjacentfaces[faces[i][1]]++;
        numadjacentfaces[faces[i][2]]++;
    }

    adjacentfaces.resize(nv);
    for(int i = 0; i < nv; i++)
    {
        adjacentfaces[i].reserve(numadjacentfaces[i]);
    }

    for(int i = 0; i < nf; i++)
    {
        for(int j = 0; j < 3; j++)
        {
            adjacentfaces[faces[i][j]].push_back(i);
        }
    }

    dprintf("Done.\n");
}

// Find the face across each edge from each other face (-1 on boundary)
// If topology is bad, not necessarily what one would expect...//夸张了 :)
//  例如：across_edge[3][2]表示三角形面片编号(faces[across_edge[3][2]]为该三角形面片)
//  该三角形与第4(下标为3)个三角形(faces[3])有公共边，公共边为faces[3]中下标为[2]的顶点所对的边
void TriangleMesh::need_across_edge()
{
    if(!across_edge.empty())
        return;

    need_adjacentfaces();
    if(adjacentfaces.empty())
        return;

    dprintf("Finding across-edge maps... ");

    int nf = faces.size();
    across_edge.resize(nf, Face(-1, -1, -1));

#pragma omp parallel for
    for(int i = 0; i < nf; i++)
    {
        for(int j = 0; j < 3; j++)
        {
            if(across_edge[i][j] != -1)
                continue;
            int v1 = faces[i][(j+1) % 3];
            int v2 = faces[i][(j+2) % 3];
            const vector<int>& a1 = adjacentfaces[v1];
            const vector<int>& a2 = adjacentfaces[v2];
            for(int k1 = 0; k1 < a1.size(); k1++)
            {
                int other = a1[k1];
                if(other == i)
                    continue;
                vector<int>::const_iterator it = find(a2.begin(), a2.end(), other);
                if(it == a2.end())
                    continue;
                int ind = (faces[other].indexof(v1)+1)%3;
                if(faces[other][(ind+1)%3] != v2)
                    continue;
                across_edge[i][j] = other;
                across_edge[other][ind] = i;
                break;
            }
        }
    }

    dprintf("Done.\n");
}
