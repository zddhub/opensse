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
/*
Szymon Rusinkiewicz
Princeton University

TriMesh_pointareas.cc
Compute the area "belonging" to each vertex or each corner
of a triangle (defined as Voronoi area restricted to the 1-ring of
a vertex, or to the triangle).

for more voronoi area, see
Meyer M., Discrete differential geometry operators for triangulated 2-manifolds
*/


#include "trianglemesh.h"

// Compute per-vertex point areas
void TriangleMesh::need_pointareas()
{
        if (pointareas.size() == vertices.size())
                return;
        need_faces();

        dprintf("Computing point areas... ");

        int nf = faces.size(), nv = vertices.size();
        pointareas.clear();
        pointareas.resize(nv);
        cornerareas.clear();
        cornerareas.resize(nf);

#pragma omp parallel for
        for (int i = 0; i < nf; i++) {
                // Edges
                vec e[3] = { vertices[faces[i][2]] - vertices[faces[i][1]],
                             vertices[faces[i][0]] - vertices[faces[i][2]],
                             vertices[faces[i][1]] - vertices[faces[i][0]] };

                // Compute corner weights
                float area = 0.5f * len(e[0] CROSS e[1]);
                float l2[3] = { len2(e[0]), len2(e[1]), len2(e[2]) };
                float ew[3] = { l2[0] * (l2[1] + l2[2] - l2[0]),
                                l2[1] * (l2[2] + l2[0] - l2[1]),
                                l2[2] * (l2[0] + l2[1] - l2[2]) };
                if (ew[0] <= 0.0f) {
                        cornerareas[i][1] = -0.25f * l2[2] * area /
                                            (e[0] DOT e[2]);
                        cornerareas[i][2] = -0.25f * l2[1] * area /
                                            (e[0] DOT e[1]);
                        cornerareas[i][0] = area - cornerareas[i][1] -
                                            cornerareas[i][2];
                } else if (ew[1] <= 0.0f) {
                        cornerareas[i][2] = -0.25f * l2[0] * area /
                                            (e[1] DOT e[0]);
                        cornerareas[i][0] = -0.25f * l2[2] * area /
                                            (e[1] DOT e[2]);
                        cornerareas[i][1] = area - cornerareas[i][2] -
                                            cornerareas[i][0];
                } else if (ew[2] <= 0.0f) {
                        cornerareas[i][0] = -0.25f * l2[1] * area /
                                            (e[2] DOT e[1]);
                        cornerareas[i][1] = -0.25f * l2[0] * area /
                                            (e[2] DOT e[0]);
                        cornerareas[i][2] = area - cornerareas[i][0] -
                                            cornerareas[i][1];
                } else {
                        float ewscale = 0.5f * area / (ew[0] + ew[1] + ew[2]);
                        for (int j = 0; j < 3; j++)
                                cornerareas[i][j] = ewscale * (ew[(j+1)%3] +
                                                               ew[(j+2)%3]);
                }
#pragma omp atomic
                pointareas[faces[i][0]] += cornerareas[i][0];
#pragma omp atomic
                pointareas[faces[i][1]] += cornerareas[i][1];
#pragma omp atomic
                pointareas[faces[i][2]] += cornerareas[i][2];
        }

        dprintf("Done.\n");
}

