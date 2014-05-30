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

#include "trimeshview.h"

//绘制边界--没有公共面的边
void TriMeshView::draw_boundaries(bool do_hidden)
{
    triMesh->need_faces();
    triMesh->need_across_edge();

    if (do_hidden) {
        glColor3f(0.6, 0.6, 0.6);
        glLineWidth(1.5);
    } else {
        glColor3f(0.05, 0.05, 0.05);
        glLineWidth(2.5);
    }

    glBegin(GL_LINES);
    for (int i = 0; i < triMesh->faces.size(); i++) {
        for (int j = 0; j < 3; j++) {
            if (triMesh->across_edge[i][j] >= 0)
                    continue;
            int v1 = triMesh->faces[i][(j+1)%3];
            int v2 = triMesh->faces[i][(j+2)%3];
            glVertex3fv(triMesh->vertices[v1]);
            glVertex3fv(triMesh->vertices[v2]);
        }
    }
    glEnd();
}

// Compute gradient of (kr * sin^2 theta) at vertex i
inline vec TriMeshView::gradkr(int i)
{
    vec viewdir = viewpos - triMesh->vertices[i];
    float rlen_viewdir = 1.0f / len(viewdir);
    viewdir *= rlen_viewdir;

    float ndotv = viewdir DOT triMesh->normals[i];
    float sintheta = sqrt(1.0f - sqr(ndotv));
    float csctheta = 1.0f / sintheta;
    float u = (viewdir DOT triMesh->pdir1[i]) * csctheta;
    float v = (viewdir DOT triMesh->pdir2[i]) * csctheta;
    float kr = triMesh->curv1[i] * u*u + triMesh->curv2[i] * v*v;
    float tr = u*v * (triMesh->curv2[i] - triMesh->curv1[i]);
    float kt = triMesh->curv1[i] * (1.0f - u*u) +
               triMesh->curv2[i] * (1.0f - v*v);
    vec w     = u * triMesh->pdir1[i] + v * triMesh->pdir2[i];
    vec wperp = u * triMesh->pdir2[i] - v * triMesh->pdir1[i];
    const Vec<4> &C = triMesh->dcurv[i];

    vec g = triMesh->pdir1[i] * (u*u*C[0] + 2.0f*u*v*C[1] + v*v*C[2]) +
            triMesh->pdir2[i] * (u*u*C[1] + 2.0f*u*v*C[2] + v*v*C[3]) -
            2.0f * csctheta * tr * (rlen_viewdir * wperp +
                                    ndotv * (tr * w + kt * wperp));
    g *= (1.0f - sqr(ndotv));
    g -= 2.0f * kr * sintheta * ndotv * (kr * w + tr * wperp);
    return g;
}

// Find a zero crossing using Hermite interpolation
float TriMeshView::find_zero_hermite(int v0, int v1, float val0, float val1,
                        const vec &grad0, const vec &grad1)
{
    if (unlikely(val0 == val1))
            return 0.5f;

    // Find derivatives along edge (of interpolation parameter in [0,1]
    // which means that e01 doesn't get normalized)
    vec e01 = triMesh->vertices[v1] - triMesh->vertices[v0];
    float d0 = e01 DOT grad0, d1 = e01 DOT grad1;

    // This next line would reduce val to linear interpolation
    //d0 = d1 = (val1 - val0);

    // Use hermite interpolation:
    //   val(s) = h1(s)*val0 + h2(s)*val1 + h3(s)*d0 + h4(s)*d1
    // where
    //  h1(s) = 2*s^3 - 3*s^2 + 1
    //  h2(s) = 3*s^2 - 2*s^3
    //  h3(s) = s^3 - 2*s^2 + s
    //  h4(s) = s^3 - s^2
    //
    //  val(s)  = [2(val0-val1) +d0+d1]*s^3 +
    //            [3(val1-val0)-2d0-d1]*s^2 + d0*s + val0
    // where
    //
    //  val(0) = val0; val(1) = val1; val'(0) = d0; val'(1) = d1
    //

    // Coeffs of cubic a*s^3 + b*s^2 + c*s + d
    float a = 2 * (val0 - val1) + d0 + d1;
    float b = 3 * (val1 - val0) - 2 * d0 - d1;
    float c = d0, d = val0;

    // -- Find a root by bisection
    // (as Newton can wander out of desired interval)

    // Start with entire [0,1] interval
    float sl = 0.0f, sr = 1.0f, valsl = val0, valsr = val1;

    // Check if we're in a (somewhat uncommon) 3-root situation, and pick
    // the middle root if it happens (given we aren't drawing curvy lines,
    // seems the best approach..)
    //
    // Find extrema of derivative (a -> 3a; b -> 2b, c -> c),
    // and check if they're both in [0,1] and have different signs
    float disc = 4 * b - 12 * a * c;
    if (disc > 0 && a != 0) {
        disc = sqrt(disc);
        float r1 = (-2 * b + disc) / (6 * a);
        float r2 = (-2 * b - disc) / (6 * a);
        if (r1 >= 0 && r1 <= 1 && r2 >= 0 && r2 <= 1) {
            float vr1 = (((a * r1 + b) * r1 + c) * r1) + d;
            float vr2 = (((a * r2 + b) * r2 + c) * r2) + d;
            // When extrema have different signs inside an
            // interval with endpoints with different signs,
            // the middle root is in between the two extrema
            if (vr1 < 0.0f && vr2 >= 0.0f ||
                vr1 > 0.0f && vr2 <= 0.0f) {
                // 3 roots
                if (r1 < r2) {
                    sl = r1;
                    valsl = vr1;
                    sr = r2;
                    valsr = vr2;
                } else {
                    sl = r2;
                    valsl = vr2;
                    sr = r1;
                    valsr = vr1;
                }
            }
        }
    }

    // Bisection method (constant number of interations)
    for (int iter = 0; iter < 10; iter++) {
        float sbi = (sl + sr) / 2.0f;
        float valsbi = (((a * sbi + b) * sbi) + c) * sbi + d;

        // Keep the half which has different signs
        if (valsl < 0.0f && valsbi >= 0.0f ||
            valsl > 0.0f && valsbi <= 0.0f) {
            sr = sbi;
            valsr = valsbi;
        } else {
            sl = sbi;
            valsl = valsbi;
        }
    }

    return 0.5f * (sl + sr);
}

// Find a zero crossing between val0 and val1 by linear interpolation
// Returns 0 if zero crossing is at val0, 1 if at val1, etc.
inline float TriMeshView::find_zero_linear(float val0, float val1)
{
        return val0 / (val0 - val1);
}

// Draw part of a zero-crossing curve on one triangle face, but only if
// "test_num/test_den" is positive.  v0,v1,v2 are the indices of the 3
// vertices, "val" are the values of the scalar field whose zero
// crossings we are finding, and "test_*" are the values we are testing
// to make sure they are positive.  This function assumes that val0 has
// opposite sign from val1 and val2 - the following function is the
// general one that figures out which one actually has the different sign.
void TriMeshView::draw_face_isoline2(int v0, int v1, int v2,
                        const vector<float> &val,
                        const vector<float> &test_num,
                        const vector<float> &test_den,
                        bool do_hermite, bool do_test, float fade)
{
    // How far along each edge?
    float w10 = do_hermite ?
            find_zero_hermite(v0, v1, val[v0], val[v1],
                              gradkr(v0), gradkr(v1)) :
            find_zero_linear(val[v0], val[v1]);
    float w01 = 1.0f - w10;
    float w20 = do_hermite ?
            find_zero_hermite(v0, v2, val[v0], val[v2],
                              gradkr(v0), gradkr(v2)) :
            find_zero_linear(val[v0], val[v2]);
    float w02 = 1.0f - w20;

    // Points along edges
    point p1 = w01 * triMesh->vertices[v0] + w10 * triMesh->vertices[v1];
    point p2 = w02 * triMesh->vertices[v0] + w20 * triMesh->vertices[v2];

//    point p1 = triMesh->vertices[v1];
//    point p2 = triMesh->vertices[v2];

    float test_num1 = 1.0f, test_num2 = 1.0f;
    float test_den1 = 1.0f, test_den2 = 1.0f;
    float z1 = 0.0f, z2 = 0.0f;
    bool valid1 = true;

    if (do_test) {
        // Interpolate to find value of test at p1, p2
        test_num1 = w01 * test_num[v0] + w10 * test_num[v1];
        test_num2 = w02 * test_num[v0] + w20 * test_num[v2];
        if (!test_den.empty()) {
                test_den1 = w01 * test_den[v0] + w10 * test_den[v1];
                test_den2 = w02 * test_den[v0] + w20 * test_den[v2];
        }
        // First point is valid iff num1/den1 is positive,
        // i.e. the num and den have the same sign
        valid1 = ((test_num1 >= 0.0f) == (test_den1 >= 0.0f));
        // There are two possible zero crossings of the test,
        // corresponding to zeros of the num and den
        if ((test_num1 >= 0.0f) != (test_num2 >= 0.0f))
                z1 = test_num1 / (test_num1 - test_num2);
        if ((test_den1 >= 0.0f) != (test_den2 >= 0.0f))
                z2 = test_den1 / (test_den1 - test_den2);
        // Sort and order the zero crossings
        if (z1 == 0.0f)
                z1 = z2, z2 = 0.0f;
        else if (z2 < z1)
                swap(z1, z2);
    }

    // If the beginning of the segment was not valid, and
    // no zero crossings, then whole segment invalid
    if (!valid1 && !z1 && !z2)
        return;

    // Draw the valid piece(s)
    int npts = 0;
    if (valid1) {
        glColor4f(currcolor[0], currcolor[1], currcolor[2],
                  test_num1 / (test_den1 * fade + test_num1));
        glVertex3fv(p1);
        npts++;
    }
    if (z1) {
        float num = (1.0f - z1) * test_num1 + z1 * test_num2;
        float den = (1.0f - z1) * test_den1 + z1 * test_den2;
        glColor4f(currcolor[0], currcolor[1], currcolor[2],
                  num / (den * fade + num));
        glVertex3fv((1.0f - z1) * p1 + z1 * p2);
        npts++;
    }
    if (z2) {
        float num = (1.0f - z2) * test_num1 + z2 * test_num2;
        float den = (1.0f - z2) * test_den1 + z2 * test_den2;
        glColor4f(currcolor[0], currcolor[1], currcolor[2],
                  num / (den * fade + num));
        glVertex3fv((1.0f - z2) * p1 + z2 * p2);
        npts++;
    }
    if (npts != 2) {
        glColor4f(currcolor[0], currcolor[1], currcolor[2],
                  test_num2 / (test_den2 * fade + test_num2));
        glVertex3fv(p2);
    }
}

// See above.  This is the driver function that figures out which of
// v0, v1, v2 has a different sign from the others.
void TriMeshView::draw_face_isoline(int v0, int v1, int v2,
                       const vector<float> &val,
                       const vector<float> &test_num,
                       const vector<float> &test_den,
                       const vector<float> &ndotv,
                       bool do_bfcull, bool do_hermite,
                       bool do_test, float fade)
{
    // 去掉背面的线
    if (likely(do_bfcull && ndotv[v0] <= 0.0f &&
               ndotv[v1] <= 0.0f && ndotv[v2] <= 0.0f))
        return;

    //提前拒绝一部分线
    // Quick reject if derivs are negative
    if (do_test) {
        if (test_den.empty()) {
            if (test_num[v0] <= 0.0f &&
                test_num[v1] <= 0.0f &&
                test_num[v2] <= 0.0f)
                    return;
        } else {
            if (test_num[v0] <= 0.0f && test_den[v0] >= 0.0f &&
                test_num[v1] <= 0.0f && test_den[v1] >= 0.0f &&
                test_num[v2] <= 0.0f && test_den[v2] >= 0.0f)
                    return;
            if (test_num[v0] >= 0.0f && test_den[v0] <= 0.0f &&
                test_num[v1] >= 0.0f && test_den[v1] <= 0.0f &&
                test_num[v2] >= 0.0f && test_den[v2] <= 0.0f)
                    return;
        }
    }

    // 找出val值符号不同的点，绘制使ndotv=0的线 零交点线
    // Figure out which val has different sign, and draw
    if (val[v0] < 0.0f && val[v1] >= 0.0f && val[v2] >= 0.0f ||
        val[v0] > 0.0f && val[v1] <= 0.0f && val[v2] <= 0.0f)
            draw_face_isoline2(v0, v1, v2,
                               val, test_num, test_den,
                               do_hermite, do_test, fade);
    else if (val[v1] < 0.0f && val[v2] >= 0.0f && val[v0] >= 0.0f ||
             val[v1] > 0.0f && val[v2] <= 0.0f && val[v0] <= 0.0f)
            draw_face_isoline2(v1, v2, v0,
                               val, test_num, test_den,
                               do_hermite, do_test, fade);
    else if (val[v2] < 0.0f && val[v0] >= 0.0f && val[v1] >= 0.0f ||
             val[v2] > 0.0f && val[v0] <= 0.0f && val[v1] <= 0.0f)
            draw_face_isoline2(v2, v0, v1,
                               val, test_num, test_den,
                               do_hermite, do_test, fade);
}

// Takes a scalar field and renders the zero crossings, but only where
// test_num/test_den is greater than 0.
void TriMeshView::draw_isolines(const vector<float> &val,
                                const vector<float> &test_num,
                                const vector<float> &test_den,
                                const vector<float> &ndotv,
                                bool do_bfcull, bool do_hermite,
                                bool do_test, float fade)
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
        // Draw a line if, among the values in this triangle,
        // at least one is positive and one is negative
        //三个顶点的val中，至少有一正一负
        const float &v0 = val[*t], &v1 = val[*(t-1)], &v2 = val[*(t-2)];
        if (unlikely((v0 > 0.0f || v1 > 0.0f || v2 > 0.0f) &&
                     (v0 < 0.0f || v1 < 0.0f || v2 < 0.0f)))
            draw_face_isoline(*(t-2), *(t-1), *t,
                              val, test_num, test_den, ndotv,
                              do_bfcull, do_hermite, do_test, fade);
        t++;
    }
}

// Draw exterior silhouette of the mesh: this just draws
// thick contours, which are partially hidden by the mesh.
// Note: this needs to happen *before* draw_base_mesh...
void TriMeshView::draw_silhouette(const vector<float> &ndotv)
{
    glDepthMask(GL_FALSE);

    currcolor = vec(0.0, 0.0, 0.0);
    glLineWidth(4);
    glBegin(GL_LINES);
    draw_isolines(ndotv, vector<float>(), vector<float>(), ndotv,
                  false, false, false, 0.0f);
    glEnd();

    // Wide lines are gappy, so fill them in
    glEnable(GL_POINT_SMOOTH);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glPointSize(4);
    glBegin(GL_POINTS);
    draw_isolines(ndotv, vector<float>(), vector<float>(), ndotv,
                  false, false, false, 0.0f);
    glEnd();

    glDisable(GL_POINT_SMOOTH);
    glDisable(GL_BLEND);
    glDepthMask(GL_TRUE);
}

//occluding contours
void TriMeshView::draw_occluding_contours(const vector<float> &ndotv, const vector<float> &kr)
{
    glLineWidth(2.5);
    glBegin(GL_LINES);
    draw_isolines(ndotv, kr, vector<float>(), ndotv,
                  false, false, true, 0.0f);
    glEnd();
}

void TriMeshView::draw_suggestive_contours(const vector<float> &ndotv,
                                           const vector<float> &kr,
                                           const vector<float> &sctest_num,
                                           const vector<float> &sctest_den)
{
    float fade = (true) ? 0.03f / sqr(feature_size) : 0.0f;
    //currcolor = vec(0.5, 0.5, 1.0);
    glLineWidth(2.5);
    glBegin(GL_LINES);
    draw_isolines(kr, sctest_num, sctest_den, ndotv,
                  true, true, true, fade);
    glEnd();
}


//等照度线(Isophotes) 和拓扑线topolines

void TriMeshView::draw_isophotes(const vector<float> &ndotv)
{

}

void TriMeshView::draw_topolines(const vector<float> &ndotv)
{

}
