
#include "trimeshview.h"


// Draw part of a ridge/valley curve on one triangle face.  v0,v1,v2
// are the indices of the 3 vertices; this function assumes that the
// curve connects points on the edges v0-v1 and v1-v2
// (or connects point on v0-v1 to center if to_center is true)
void TriMeshView::draw_segment_ridge(int v0, int v1, int v2,
                        float emax0, float emax1, float emax2,
                        float kmax0, float kmax1, float kmax2,
                        float thresh, bool to_center)
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

// Draw ridges or valleys (depending on do_ridge) in a triangle v0,v1,v2
// - uses ndotv for backface culling (enabled with do_bfcull)
// - do_test checks for curvature maxima/minina for ridges/valleys
//   (when off, it draws positive minima and negative maxima)
// Note: this computes ridges/valleys every time, instead of once at the
//   start (given they aren't view dependent, this is wasteful)
// Algorithm based on formulas of Ohtake et al., 2004.
void TriMeshView::draw_face_ridges(int v0, int v1, int v2,
                      bool do_ridge,
                      const vector<float> &ndotv,
                      bool do_bfcull, bool do_test, float thresh)
{
    // Backface culling
    if (likely(do_bfcull &&
               ndotv[v0] <= 0.0f && ndotv[v1] <= 0.0f && ndotv[v2] <= 0.0f))
    return;

    // Check if ridge possible at vertices just based on curvatures
    if (do_ridge) {
        if ((triMesh->curv1[v0] <= 0.0f) ||
            (triMesh->curv1[v1] <= 0.0f) ||
            (triMesh->curv1[v2] <= 0.0f))
                return;
    } else {
        if ((triMesh->curv1[v0] >= 0.0f) ||
            (triMesh->curv1[v1] >= 0.0f) ||
            (triMesh->curv1[v2] >= 0.0f))
                return;
    }

    // Sign of curvature on ridge/valley
    float rv_sign = do_ridge ? 1.0f : -1.0f;

    // The "tmax" are the principal directions of maximal curvature,
    // flipped to point in the direction in which the curvature
    // is increasing (decreasing for valleys).  Note that this
    // is a bit different from the notation in Ohtake et al.,
    // but the tests below are equivalent.
    const float &emax0 = triMesh->dcurv[v0][0];
    const float &emax1 = triMesh->dcurv[v1][0];
    const float &emax2 = triMesh->dcurv[v2][0];
    vec tmax0 = rv_sign * triMesh->dcurv[v0][0] * triMesh->pdir1[v0];
    vec tmax1 = rv_sign * triMesh->dcurv[v1][0] * triMesh->pdir1[v1];
    vec tmax2 = rv_sign * triMesh->dcurv[v2][0] * triMesh->pdir1[v2];

    // We have a "zero crossing" if the tmaxes along an edge
    // point in opposite directions
    bool z01 = ((tmax0 DOT tmax1) <= 0.0f);
    bool z12 = ((tmax1 DOT tmax2) <= 0.0f);
    bool z20 = ((tmax2 DOT tmax0) <= 0.0f);

    if (z01 + z12 + z20 < 2)
            return;

    if (do_test) {
            const point &p0 = triMesh->vertices[v0],
                        &p1 = triMesh->vertices[v1],
                        &p2 = triMesh->vertices[v2];

            // Check whether we have the correct flavor of extremum:
            // Is the curvature increasing along the edge?
            z01 = z01 && ((tmax0 DOT (p1 - p0)) >= 0.0f ||
                          (tmax1 DOT (p1 - p0)) <= 0.0f);
            z12 = z12 && ((tmax1 DOT (p2 - p1)) >= 0.0f ||
                          (tmax2 DOT (p2 - p1)) <= 0.0f);
            z20 = z20 && ((tmax2 DOT (p0 - p2)) >= 0.0f ||
                          (tmax0 DOT (p0 - p2)) <= 0.0f);

            if (z01 + z12 + z20 < 2)
                    return;
    }

    // Draw line segment
    const float &kmax0 = triMesh->curv1[v0];
    const float &kmax1 = triMesh->curv1[v1];
    const float &kmax2 = triMesh->curv1[v2];
    if (!z01) {
        draw_segment_ridge(v1, v2, v0,
                           emax1, emax2, emax0,
                           kmax1, kmax2, kmax0,
                           thresh, false);
    } else if (!z12) {
        draw_segment_ridge(v2, v0, v1,
                           emax2, emax0, emax1,
                           kmax2, kmax0, kmax1,
                           thresh, false);
    } else if (!z20) {
        draw_segment_ridge(v0, v1, v2,
                           emax0, emax1, emax2,
                           kmax0, kmax1, kmax2,
                           thresh, false);
    } else {
        // All three edges have crossings -- connect all to center
        draw_segment_ridge(v1, v2, v0,
                           emax1, emax2, emax0,
                           kmax1, kmax2, kmax0,
                           thresh, true);
        draw_segment_ridge(v2, v0, v1,
                           emax2, emax0, emax1,
                           kmax2, kmax0, kmax1,
                           thresh, true);
        draw_segment_ridge(v0, v1, v2,
                           emax0, emax1, emax2,
                           kmax0, kmax1, kmax2,
                           thresh, true);
    }
}

// Draw the ridges (valleys) of the mesh
void TriMeshView::draw_mesh_ridges(bool do_ridge, const vector<float> &ndotv,
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

        draw_face_ridges(*(t-2), *(t-1), *t,
                do_ridge, ndotv, do_bfcull, do_test, thresh);
        t++;
    }
}

void TriMeshView::draw_ridges(const vector<float> &ndotv, float thresh)
{
    glLineWidth(2);
    glBegin(GL_LINES);
    draw_mesh_ridges(true, ndotv, true, true,thresh);
    glEnd();
}

void TriMeshView::draw_valleys(const vector<float> &ndotv, float thresh)
{
    glLineWidth(2);
    glBegin(GL_LINES);
    draw_mesh_ridges(false, ndotv, true, true,thresh);
    glEnd();
}
