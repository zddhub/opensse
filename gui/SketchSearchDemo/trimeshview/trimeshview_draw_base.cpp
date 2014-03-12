#include "trimeshview.h"

#ifndef M_2_PI
#	define M_2_PI 6.283185307179586232    /* 2*pi  */
#endif

//------------------------private function------------------------

//计算ndotv和kr
void TriMeshView::compute_perview(std::vector<float> &ndotv, vector<float> &kr)
{
    triMesh->need_normals();

    int nv = triMesh->vertices.size();

    ndotv.resize(nv);
    kr.resize(nv);

#pragma omp parallel for
    for(int i = 0; i < nv; i++)
    {
        vec viewdir = viewpos - triMesh->vertices[i];
        ndotv[i] = triMesh->normals[i] DOT viewdir;

        float u = viewdir DOT triMesh->pdir1[i], u2 = u*u;
        float v = viewdir DOT triMesh->pdir2[i], v2 = v*v;

        // Note:  this is actually Kr * sin^2 theta
        kr[i] = triMesh->curv1[i] * u2 + triMesh->curv2[i] * v2;
    }
}

// Compute per-vertex n dot l, n dot v, radial curvature, and
// derivative of curvature for the current view
void TriMeshView::compute_perview(vector<float> &ndotv, vector<float> &kr,
                     vector<float> &sctest_num, vector<float> &sctest_den,
                     vector<float> &shtest_num, vector<float> &q1,
                     vector<vec2> &t1, vector<float> &Dt1q1,
                     bool extra_sin2theta)
{
    if(isDrawApparentRidges)
        triMesh->need_adjacentfaces();

    int nv = triMesh->vertices.size();

    float scthresh = sug_thresh / sqr(feature_size);
//    float shthresh = sh_thresh / sqr(feature_size);
//    bool need_DwKr = (draw_sc || draw_sh || draw_DwKr);
    bool need_DwKr = isDrawSuggestiveContours;

    ndotv.resize(nv);
    kr.resize(nv);
    if (isDrawApparentRidges) {
        q1.resize(nv);
        t1.resize(nv);
        Dt1q1.resize(nv);
    }
    if (need_DwKr) {
        sctest_num.resize(nv);
        sctest_den.resize(nv);
//            if (draw_sh)
//                    shtest_num.resize(nv);
    }

    // Compute quantities at each vertex
#pragma omp parallel for
    for (int i = 0; i < nv; i++) {
        // Compute n DOT v
        vec viewdir = viewpos - triMesh->vertices[i];
        float rlv = 1.0f / len(viewdir);
        viewdir *= rlv;
        ndotv[i] = viewdir DOT triMesh->normals[i];

        float u = viewdir DOT triMesh->pdir1[i], u2 = u*u;
        float v = viewdir DOT triMesh->pdir2[i], v2 = v*v;

        // Note:  this is actually Kr * sin^2 theta
        kr[i] = triMesh->curv1[i] * u2 + triMesh->curv2[i] * v2;

        if (isDrawApparentRidges) {
            float csc2theta = 1.0f / (u2 + v2);
            compute_viewdep_curv(triMesh, i, ndotv[i],
                    u2*csc2theta, u*v*csc2theta, v2*csc2theta,
                    q1[i], t1[i]);
        }
        if (!need_DwKr)
                continue;

        // Use DwKr * sin(theta) / cos(theta) for cutoff test
        sctest_num[i] = u2 * (     u*triMesh->dcurv[i][0] +
                              3.0f*v*triMesh->dcurv[i][1]) +
                        v2 * (3.0f*u*triMesh->dcurv[i][2] +
                                   v*triMesh->dcurv[i][3]);
        float csc2theta = 1.0f / (u2 + v2);
        sctest_num[i] *= csc2theta;
        float tr = (triMesh->curv2[i] - triMesh->curv1[i]) *
                   u * v * csc2theta;
        sctest_num[i] -= 2.0f * ndotv[i] * sqr(tr);
        if (extra_sin2theta)
                sctest_num[i] *= u2 + v2;

        sctest_den[i] = ndotv[i];

//        if (draw_sh) {
//                shtest_num[i] = -sctest_num[i];
//                shtest_num[i] -= shthresh * sctest_den[i];
//        }
        sctest_num[i] -= scthresh * sctest_den[i];
    }
    if (isDrawApparentRidges) {
#pragma omp parallel for
        for (int i = 0; i < nv; i++)
            compute_Dt1q1(triMesh, i, ndotv[i], q1, t1, Dt1q1[i]);
    }
}

void TriMeshView::draw_mesh()
{
    // These are static so the memory isn't reallocated on every frame
    static vector<float> ndotv, kr;
    static vector<float> sctest_num, sctest_den, shtest_num;
    static vector<float> q1, Dt1q1;
    static vector<vec2> t1;
    compute_perview(ndotv, kr, sctest_num, sctest_den, shtest_num,
            q1, t1, Dt1q1, true);
//    compute_perview(ndotv, kr);

    // Enable antialiased lines
    glEnable(GL_POINT_SMOOTH);
    glEnable(GL_LINE_SMOOTH);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // 外部轮廓,仅使用ndotv，不做其它测试
    if (isDrawSilhouette)
        draw_silhouette(ndotv);

    // The mesh itself, possibly colored and/or lit
    glDisable(GL_BLEND);
    draw_base_mesh();
    glEnable(GL_BLEND);

    if(isDrawApparentRidges)
        draw_apparent_ridges(ndotv, q1, t1, Dt1q1, ar_thresh / sqr(feature_size));

    if(isDrawOccludingContours)
        draw_occluding_contours(ndotv, kr);

    if(isDrawSuggestiveContours)
        draw_suggestive_contours(ndotv, kr, sctest_num, sctest_den);

    if(isDrawRidges)
        draw_ridges(ndotv, rv_thresh / feature_size);
    if(isDrawValleys)
        draw_valleys(ndotv, rv_thresh / feature_size);

    if(isDrawBoundaries)
        draw_boundaries(false);

    glDisable(GL_LINE_SMOOTH);
    glDisable(GL_POINT_SMOOTH);
    glDisable(GL_BLEND);
    glDepthMask(GL_TRUE);
}

void TriMeshView::draw_base_mesh()
{
    // Enable the vertex array
    glEnableClientState(GL_VERTEX_ARRAY);
    glVertexPointer(3, GL_FLOAT, 0, &triMesh->vertices[0][0]);

    //normal array
    if(!triMesh->normals.empty())
    {
        glEnableClientState(GL_NORMAL_ARRAY);
        glNormalPointer(GL_FLOAT, 0, &triMesh->normals[0][0]);
    }
    else
    {
        glDisableClientState(GL_NORMAL_ARRAY);
    }

//    glColor3f(1, 1, 1);
//    draw_strips();//遮住背面法线

    // Draw the mesh, possibly with color and/or lighting
    glDepthFunc(GL_LESS);
    glEnable(GL_DEPTH_TEST);
    glPolygonOffset(5.0f, 30.0f);
    glEnable(GL_POLYGON_OFFSET_FILL);
    glEnable(GL_CULL_FACE);


    if(isDrawCurvColors)
        draw_curv_colors();

    if(isDrawNormalColors)
        draw_normal_colors();

 //   draw_color_maps();
    glColor3f(1, 1, 1);
    draw_strips();//遮住背面法线
 //
    // Reset everything
    glDisableClientState(GL_COLOR_ARRAY);
    glDisable(GL_CULL_FACE);
    glDepthMask(GL_FALSE); // Do not remove me, else get dotted lines--zdd: That's true.

    if(isDrawEdges)
        draw_edges();

    if(isDrawNormals)
        draw_normals();

    if(isDrawPreview)
        draw_preview();

    if(isDrawCurv1)
        draw_curv1();
    if(isDrawCurv2)
        draw_curv2();

    glDisableClientState(GL_VERTEX_ARRAY);
}

// Draw triangle strips.  They are stored as length followed by values.
void TriMeshView::draw_strips()
{
    const int *t = &triMesh->tstrips[0];
    const int *end = t + triMesh->tstrips.size();

    while (likely(t < end)) {
        int striplen = *t++;
        glDrawElements(GL_TRIANGLE_STRIP, striplen, GL_UNSIGNED_INT, t);
        t += striplen;
    }
}

//绘制线框
void TriMeshView::draw_edges()
{
    glPolygonOffset(10.0f, 10.0f);
    glEnable(GL_POLYGON_OFFSET_FILL);

    glEnable(GL_CULL_FACE);
    glLineWidth(1);
    glPolygonMode(GL_FRONT, GL_LINE);
    glColor3f(0.5, 1.0, 1.0);
    draw_strips();
    glPolygonMode(GL_FRONT, GL_FILL);
    glDisable(GL_CULL_FACE);

    glDisable(GL_POLYGON_OFFSET_FILL);
}

//绘制法线
void TriMeshView::draw_normals()
{
    glColor3f(0.7, 0.7, 0);
    glBegin(GL_LINES);
    for(int i = 0; i < triMesh->vertices.size(); i++)
    {
        glVertex3fv(triMesh->vertices[i]);
        glVertex3fv(triMesh->vertices[i]+ feature_size * triMesh->normals[i]);
    }
    glEnd();

    glPointSize(3);
    glDrawArrays(GL_POINTS, 0, triMesh->vertices.size());
}

//绘制投影视线方向
//当视线与发现垂直时，ndotv=0， 绘制长度为0
void TriMeshView::draw_preview()
{
    glColor3f(0.0, 0.0, 0.8);
    glBegin(GL_LINES);

    for(int i = 0; i < triMesh->vertices.size(); i++)
    {
        vec w = viewpos - triMesh->vertices[i];
        w -= triMesh->normals[i] * (w DOT triMesh->normals[i]);
        normalize(w);
        glVertex3fv(triMesh->vertices[i]);
        glVertex3fv(triMesh->vertices[i] + 0.5f * feature_size * w);
    }

    glEnd();
}

void TriMeshView::compute_normal_colors()
{
    int nv = triMesh->vertices.size();
    normal_colors.resize(nv);

    triMesh->need_normals();
    for (int i = 0; i < nv; i++) {
        normal_colors[i] = Color(0.5f, 0.5f, 0.5f) +
                    0.5f * triMesh->normals[i];
    }

    return;
}

void TriMeshView::draw_normal_colors()
{
    if (normal_colors.empty())
        compute_normal_colors();
    glEnableClientState(GL_COLOR_ARRAY);
    glColorPointer(3, GL_FLOAT, 0,
            &normal_colors[0][0]);

    draw_strips();

    glDisableClientState(GL_COLOR_ARRAY);
}

//绘制模值最大的主曲率方向
void TriMeshView::draw_curv1()
{
    float line_len = 0.5f * feature_size;
    glColor3f(0.2, 0.7, 0.2);
    glBegin(GL_LINES);
    for (int i = 0; i < triMesh->vertices.size(); i++) {
        glVertex3fv(triMesh->vertices[i] -
                    line_len * triMesh->pdir1[i]);
        glVertex3fv(triMesh->vertices[i] +
                    line_len * triMesh->pdir1[i]);
    }
    glEnd();
}

//绘制模值最小的主曲率方向
void TriMeshView::draw_curv2()
{
    float line_len = 0.5f * feature_size;
    glColor3f(0.7, 0.2, 0.2);
    glBegin(GL_LINES);
    for (int i = 0; i < triMesh->vertices.size(); i++) {
        glVertex3fv(triMesh->vertices[i] -
                    line_len * triMesh->pdir2[i]);
        glVertex3fv(triMesh->vertices[i] +
                    line_len * triMesh->pdir2[i]);
    }
    glEnd();
}

void TriMeshView::compute_curv_colors()
{
    int nv = triMesh->vertices.size();
    curv_colors.resize(nv);

//    for (int i = 0; i < nv; i++)
//    {
//        float k = triMesh->curv1[i] * triMesh->curv2[i];
//        if(k > 0)
//        {
//            if(triMesh->curv1[i] > 0)
//                curv_colors[i] = Color(k/255, 0.0f, 0.0f);
//            else
//                curv_colors[i] = Color(0.0f, 0.0f, k/255);
//        }
//        else if(k < 0)
//        {
//            if(triMesh->curv1[i] > 0)
//                curv_colors[i] = Color(0.0f, -k/255, 0.0f);
//            else
//                curv_colors[i] = Color(0.0f, -k/255, 0.0f);
//        }
//        else
//        {
//            curv_colors[i] = Color(1.0f, 1.0f, 1.0f);
//        }
//    }

    float cscale = sqr(2.5f * feature_size);
    for (int i = 0; i < nv; i++) {
        float H = 0.5f * (triMesh->curv1[i] + triMesh->curv2[i]);
        float K = triMesh->curv1[i] * triMesh->curv2[i];
        float h = 4.0f / 3.0f * fabs(atan2(H*H-K,H*H*sgn(H)));
        float s = M_2_PI * atan((2.0f*H*H-K)*cscale);
        curv_colors[i] = Color::hsv(h,s,0.9);
    }
}

void TriMeshView::draw_color_maps()
{
    glBegin(GL_QUADS);
    glColor3f(0.0, 1.0, 0.0);
    glVertex2f(1, -1);
    glColor3f(0.5, 0.0, 0.0);
    glVertex2f(1,  1);
    glColor3f(0.5, 1.0, 0.0);
    glVertex2f(-1, 1);
    glColor3f(1.0, 1.0, 1.0);
    glVertex2f(-1, -1);
    glEnd();
}

//绘制带曲率颜色的模型
void TriMeshView::draw_curv_colors()
{
    if (curv_colors.empty())
            compute_curv_colors();
    glEnableClientState(GL_COLOR_ARRAY);
    glColorPointer(3, GL_FLOAT, 0,
            &curv_colors[0][0]);

    draw_strips();

    glDisableClientState(GL_COLOR_ARRAY);
}
