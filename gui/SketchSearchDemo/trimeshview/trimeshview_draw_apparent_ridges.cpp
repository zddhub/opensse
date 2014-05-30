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
Tweaks by zdd again

Original by Tilke Judd
Tweaks by Szymon Rusinkiewicz

apparentridge.cc
Compute apparent ridges.

Implements method of
  Judd, T., Durand, F, and Adelson, E.
  Apparent Ridges for Line Drawing,
  ACM Trans. Graphics (Proc. SIGGRAPH), vol. 26, no. 3, 2007.
*/

#include <stdio.h>
#include "trimeshview.h"
using namespace std;


// Compute largest eigenvalue and associated eigenvector of a
// symmetric 2x2 matrix.  Solves characteristic equation.
// Inputs: three elements of matrix (upper-left, diag, lower-right)
// Outputs: largest (in magnitude) eigenvector/value
static void largest_eig_2x2(float m1, float m12, float m2, vec2 &e1, float &l1)
{
    l1 = 0.5f * (m1 + m2);
    // The result of the below sqrt is positive, so to get the largest
    // eigenvalue we add it if we were positive already, else subtract
    if (l1 > 0.0f)
            l1 += sqrt(sqr(m12) + 0.25f * sqr(m2-m1));
    else
            l1 -= sqrt(sqr(m12) + 0.25f * sqr(m2-m1));

    // Find corresponding eigenvector
    e1 = vec2(m2 - l1, -m12);
    normalize(e1);
}


// Compute principal view-dependent curvatures and directions at vertex i.
// ndotv = cosine of angle between normal and view direction
// (u,v) = coordinates of w (projected view) in principal coordinates
// Pass in u^2, u*v, and v^2, since those are readily available.
// Fills in q1 and t1 (using the paper's notation).
// Note that the latter is expressed in the (pdir1,pdir2) coordinate basis
void TriMeshView::compute_viewdep_curv(const TriangleMesh *mesh, int i, float ndotv,
                          float u2, float uv, float v2,
                          float &q1, vec2 &t1)
{
    // Find the entries in Q = S * P^-1
    //                       = S + (sec theta - 1) * S * w * w^T
    float sectheta_minus1 = 1.0f / fabs(ndotv) - 1.0f;
    float Q11 = mesh->curv1[i] * (1.0f + sectheta_minus1 * u2);
    float Q12 = mesh->curv1[i] * (       sectheta_minus1 * uv);
    float Q21 = mesh->curv2[i] * (       sectheta_minus1 * uv);
    float Q22 = mesh->curv2[i] * (1.0f + sectheta_minus1 * v2);

    // Find the three entries in the (symmetric) matrix Q^T Q
    float QTQ1  = Q11 * Q11 + Q21 * Q21;
    float QTQ12 = Q11 * Q12 + Q21 * Q22;
    float QTQ2  = Q12 * Q12 + Q22 * Q22;

    // Compute eigenstuff
    largest_eig_2x2(QTQ1, QTQ12, QTQ2, t1, q1);
}


// i+1 and i-1 modulo 3
#define NEXT(i) ((i)<2 ? (i)+1 : (i)-2)
#define PREV(i) ((i)>0 ? (i)-1 : (i)+2)


// Compute D_{t_1} q_1 - the derivative of max view-dependent curvature
// in the principal max view-dependent curvature direction.
void TriMeshView::compute_Dt1q1(const TriangleMesh *mesh, int i, float ndotv,
                   const vector<float> &q1, const vector<vec2> &t1,
                   float &Dt1q1)
{
    const point &v0 = mesh->vertices[i];
    float this_viewdep_curv = q1[i];
    vec world_t1 = t1[i][0] * mesh->pdir1[i] + t1[i][1] * mesh->pdir2[i];
    vec world_t2 = mesh->normals[i] CROSS world_t1;
    float v0_dot_t2 = v0 DOT world_t2;

    Dt1q1 = 0.0f;
    int n = 0;

    int naf = mesh->adjacentfaces[i].size();
    for (int j = 0; j < naf; j++) {
        // We're in a triangle adjacent to the vertex of interest.
        // The current vertex is v0 - let v1 and v2 be the other two
        int f = mesh->adjacentfaces[i][j];
        int ind = mesh->faces[f].indexof(i);
        int i1 = mesh->faces[f][NEXT(ind)];
        int i2 = mesh->faces[f][PREV(ind)];
        const point &v1 = mesh->vertices[i1];
        const point &v2 = mesh->vertices[i2];

        // Find the point p on the segment between v1 and v2 such that
        // its vector from v0 is along t1, i.e. perpendicular to t2.
        // Linear combination: p = w1*v1 + w2*v2, where w2 = 1-w1
        float v1_dot_t2 = v1 DOT world_t2;
        float v2_dot_t2 = v2 DOT world_t2;
        float w1 = (v2_dot_t2 - v0_dot_t2) / (v2_dot_t2 - v1_dot_t2);

        // If w1 is not in [0..1) then we're not interested.
        // Incidentally, the computation of w1 can result in infinity,
        // but the comparison should do the right thing...
        if (w1 < 0.0f || w1 >= 1.0f)
                continue;

        // Construct the opposite point
        float w2 = 1.0f - w1;
        point p = w1 * v1 + w2 * v2;

        // And interpolate to find the view-dependent curvature at that point
        float interp_viewdep_curv = w1 * q1[i1] + w2 * q1[i2];

        // Finally, take the *projected* view-dependent curvature derivative
        float proj_dist = (p - v0) DOT world_t1;
        proj_dist *= fabs(ndotv);
        Dt1q1 += (interp_viewdep_curv - this_viewdep_curv) / proj_dist;
        n++;

        // To save time, quit as soon as we have two estimates
        // (that's all we're going to get, anyway)
        if (n == 2) {
                Dt1q1 *= 0.5f;
                return;
        }
    }
}


// Draw part of an apparent ridge/valley curve on one triangle face.
// v0,v1,v2 are the indices of the 3 vertices; this function assumes that the
// curve connects points on the edges v0-v1 and v1-v2
// (or connects point on v0-v1 to center if to_center is true)
void TriMeshView::draw_segment_app_ridge(int v0, int v1, int v2,
                            float emax0, float emax1, float emax2,
                            float kmax0, float kmax1, float kmax2,
                            const vec &tmax0, const vec &tmax1, const vec &tmax2,
                            float thresh, bool to_center, bool do_test)
{
    // Interpolate to find ridge/valley line segment endpoints
    // in this triangle and the curvatures there
    float w10 = fabs(emax0) / (fabs(emax0) + fabs(emax1));
    float w01 = 1.0f - w10;
    point p01 = w01 * triMesh->vertices[v0] + w10 * triMesh->vertices[v1];
    float k01 = fabs(w01 * kmax0 + w10 * kmax1);

    point p12;
    float k12;
    if (to_center) {
            // Connect first point to center of triangle
            p12 = (triMesh->vertices[v0] +
                   triMesh->vertices[v1] +
                   triMesh->vertices[v2]) / 3.0f;
            k12 = fabs(kmax0 + kmax1 + kmax2) / 3.0f;
    } else {
            // Connect first point to second one (on next edge)
            float w21 = fabs(emax1) / (fabs(emax1) + fabs(emax2));
            float w12 = 1.0f - w21;
            p12 = w12 * triMesh->vertices[v1] + w21 * triMesh->vertices[v2];
            k12 = fabs(w12 * kmax1 + w21 * kmax2);
    }

    // Don't draw below threshold
    k01 -= thresh;
    if (k01 < 0.0f)
            k01 = 0.0f;
    k12 -= thresh;
    if (k12 < 0.0f)
            k12 = 0.0f;

    // Skip lines that you can't see...
    if (k01 == 0.0f && k12 == 0.0f)
            return;

    // Perform test: do the tmax-es point *towards* the segment? (Fig 6)
    if (do_test) {
            // Find the vector perpendicular to the segment (p01 <-> p12)
            vec perp = trinorm(triMesh->vertices[v0],
                               triMesh->vertices[v1],
                               triMesh->vertices[v2]) CROSS (p01 - p12);
            // We want tmax1 to point opposite to perp, and
            // tmax0 and tmax2 to point along it.  Otherwise, exit out.
            if ((tmax0 DOT perp) <= 0.0f ||
                (tmax1 DOT perp) >= 0.0f ||
                (tmax2 DOT perp) <= 0.0f)
                    return;
    }

    // Fade lines
    if (true) {
            k01 /= (k01 + thresh);
            k12 /= (k12 + thresh);
    } else {
            k01 = k12 = 1.0f;
    }

    // Draw the line segment
    glColor4f(currcolor[0], currcolor[1], currcolor[2], k01);
    glVertex3fv(p01);
    glColor4f(currcolor[0], currcolor[1], currcolor[2], k12);
    glVertex3fv(p12);
}


// Draw apparent ridges in a triangle
void TriMeshView::draw_face_app_ridges(int v0, int v1, int v2,
                          const vector<float> &ndotv, const vector<float> &q1,
                          const vector<vec2> &t1, const vector<float> &Dt1q1,
                          bool do_bfcull, bool do_test, float thresh)
{
#if 0
    // Backface culling is turned off: getting contours from the
    // apparent ridge definition requires us to process faces that
    // may be (just barely) backfacing...
    if (likely(do_bfcull &&
               ndotv[v0] <= 0.0f && ndotv[v1] <= 0.0f && ndotv[v2] <= 0.0f))
            return;
#endif

    // Trivial reject if this face isn't getting past the threshold anyway
    const float &kmax0 = q1[v0];
    const float &kmax1 = q1[v1];
    const float &kmax2 = q1[v2];
    if (kmax0 <= thresh && kmax1 <= thresh && kmax2 <= thresh)
            return;

    // The "tmax" are the principal directions of view-dependent curvature,
    // flipped to point in the direction in which the curvature
    // is increasing.
    const float &emax0 = Dt1q1[v0];
    const float &emax1 = Dt1q1[v1];
    const float &emax2 = Dt1q1[v2];
    vec world_t1_0 = t1[v0][0] * triMesh->pdir1[v0] +
                     t1[v0][1] * triMesh->pdir2[v0];
    vec world_t1_1 = t1[v1][0] * triMesh->pdir1[v1] +
                     t1[v1][1] * triMesh->pdir2[v1];
    vec world_t1_2 = t1[v2][0] * triMesh->pdir1[v2] +
                     t1[v2][1] * triMesh->pdir2[v2];
    vec tmax0 = Dt1q1[v0] * world_t1_0;
    vec tmax1 = Dt1q1[v1] * world_t1_1;
    vec tmax2 = Dt1q1[v2] * world_t1_2;

    // We have a "zero crossing" if the tmaxes along an edge
    // point in opposite directions
    bool z01 = ((tmax0 DOT tmax1) <= 0.0f);
    bool z12 = ((tmax1 DOT tmax2) <= 0.0f);
    bool z20 = ((tmax2 DOT tmax0) <= 0.0f);

    if (z01 + z12 + z20 < 2)
            return;

    // Draw line segment
    if (!z01) {
        draw_segment_app_ridge(v1, v2, v0,
                               emax1, emax2, emax0,
                               kmax1, kmax2, kmax0,
                               tmax1, tmax2, tmax0,
                               thresh, false, do_test);
    } else if (!z12) {
        draw_segment_app_ridge(v2, v0, v1,
                               emax2, emax0, emax1,
                               kmax2, kmax0, kmax1,
                               tmax2, tmax0, tmax1,
                               thresh, false, do_test);
    } else if (!z20) {
        draw_segment_app_ridge(v0, v1, v2,
                               emax0, emax1, emax2,
                               kmax0, kmax1, kmax2,
                               tmax0, tmax1, tmax2,
                               thresh, false, do_test);
    } else {
        // All three edges have crossings -- connect all to center
        draw_segment_app_ridge(v1, v2, v0,
                               emax1, emax2, emax0,
                               kmax1, kmax2, kmax0,
                               tmax1, tmax2, tmax0,
                               thresh, true, do_test);
        draw_segment_app_ridge(v2, v0, v1,
                               emax2, emax0, emax1,
                               kmax2, kmax0, kmax1,
                               tmax2, tmax0, tmax1,
                               thresh, true, do_test);
        draw_segment_app_ridge(v0, v1, v2,
                               emax0, emax1, emax2,
                               kmax0, kmax1, kmax2,
                               tmax0, tmax1, tmax2,
                               thresh, true, do_test);
    }
}


// Draw apparent ridges of the mesh
void TriMeshView::draw_mesh_app_ridges(const vector<float> &ndotv, const vector<float> &q1,
                          const vector<vec2> &t1, const vector<float> &Dt1q1,
                          bool do_bfcull, bool do_test, float thresh)
{
    const int *t = &triMesh->tstrips[0];
    const int *stripend = t;
    const int *end = t + triMesh->tstrips.size();

    // Walk through triangle strips
    while (1) {
            if (unlikely(t >= stripend)) {
                    if (unlikely(t >= end))
                            return;
                    // New strip: each strip is stored as
                    // length followed by indices
                    stripend = t + 1 + *t;
                    // Skip over length plus first two indices of
                    // first face
                    t += 3;
            }

            draw_face_app_ridges(*(t-2), *(t-1), *t,
                                 ndotv, q1, t1, Dt1q1,
                                 do_bfcull, do_test, thresh);
            t++;
    }
}

void TriMeshView::draw_apparent_ridges(const vector<float> &ndotv, const vector<float> &q1,
                                       const vector<vec2> &t1, const vector<float> &Dt1q1,
                                       float thresh)
{
    glLineWidth(2.5);
    glBegin(GL_LINES);
    draw_mesh_app_ridges(ndotv, q1, t1, Dt1q1, true,
            true, thresh);
    glEnd();
}
