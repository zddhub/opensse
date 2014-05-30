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
#ifndef TRIMESHVIEW_H
#define TRIMESHVIEW_H

#include <QGLWidget>
#include <QMouseEvent>
#include <QWheelEvent>
#include <QKeyEvent>
#include <QTimer>

#include "trianglemesh.h"
#include "XForm.h"
#include "GLCamera.h"

using namespace std;

class TriMeshView : public QGLWidget
{
    Q_OBJECT
public:
    explicit TriMeshView(QWidget *parent = 0);
    ~TriMeshView();
    bool readMesh(const char *filename, const char* xffilename = "");
    void clearMesh();
    void closeAllDrawings();
signals:

public slots:
    void autospin();
protected:
    void cls();

    void resizeGL(int width, int height);
    void paintGL();

    void mousePressEvent(QMouseEvent *e);
    void mouseReleaseEvent(QMouseEvent *e);
    void mouseMoveEvent(QMouseEvent *e);
    void wheelEvent(QWheelEvent *e);

    void keyPressEvent(QKeyEvent *e);
    void keyReleaseEvent(QKeyEvent *);

private:
    //draw_base

    float feature_size;
    void compute_perview(vector<float> &ndotv, vector<float> &kr);
    void compute_perview(vector<float> &ndotv, vector<float> &kr,
                         vector<float> &sctest_num, vector<float> &sctest_den,
                         vector<float> &shtest_num, vector<float> &q1,
                         vector<vec2> &t1, vector<float> &Dt1q1,
                         bool extra_sin2theta = false);

    void draw_mesh();
    void draw_base_mesh();
    void draw_strips();

    void draw_edges();

    void draw_normals();

    void draw_preview();

    vector<Color> normal_colors;
    void compute_normal_colors();
    void draw_normal_colors();

    void draw_curv1();
    void draw_curv2();

    vector<Color> curv_colors;
    void compute_curv_colors();
    void draw_color_maps();
    void draw_curv_colors();

public:
    //draw_base

    bool isDrawEdges;

    bool isDrawNormals;
    bool isDrawPreview;
    bool isDrawCurv1;
    bool isDrawCurv2;

    bool isDrawCurvColors;
    bool isDrawNormalColors;

private:
    //draw_lines

    void draw_boundaries(bool do_hidden);

    inline vec gradkr(int i);
    float find_zero_hermite(int v0, int v1, float val0, float val1,
                            const vec &grad0, const vec &grad1);
    inline float find_zero_linear(float val0, float val1);
    void draw_face_isoline2(int v0, int v1, int v2,
                            const vector<float> &val,
                            const vector<float> &test_num,
                            const vector<float> &test_den,
                            bool do_hermite, bool do_test, float fade);
    void draw_face_isoline(int v0, int v1, int v2,
                           const vector<float> &val,
                           const vector<float> &test_num,
                           const vector<float> &test_den,
                           const vector<float> &ndotv,
                           bool do_bfcull, bool do_hermite,
                           bool do_test, float fade);
    void draw_isolines(const vector<float> &val,
                       const vector<float> &test_num,
                       const vector<float> &test_den,
                       const vector<float> &ndotv,
                       bool do_bfcull, bool do_hermite,
                       bool do_test, float fade);

    void draw_silhouette(const vector<float> &ndotv);
    void draw_occluding_contours(const vector<float> &ndotv, const vector<float> &kr);

    void draw_suggestive_contours(const vector<float> &ndotv,
                                  const vector<float> &kr,
                                  const vector<float> &sctest_num,
                                  const vector<float> &sctest_den);

    void draw_isophotes(const vector<float> &ndotv);
    void draw_topolines(const vector<float> &ndotv);

    //ridges and valleys
    void draw_segment_ridge(int v0, int v1, int v2,
                            float emax0, float emax1, float emax2,
                            float kmax0, float kmax1, float kmax2,
                            float thresh, bool to_center);
    void draw_face_ridges(int v0, int v1, int v2,
                          bool do_ridge,
                          const vector<float> &ndotv,
                          bool do_bfcull, bool do_test, float thresh);
    void draw_mesh_ridges(bool do_ridge, const vector<float> &ndotv,
                          bool do_bfcull, bool do_test, float thresh);
    void draw_ridges(const vector<float> &ndotv, float thresh);
    void draw_valleys(const vector<float> &ndotv, float thresh);

    //apparent ridges
    void compute_viewdep_curv(const TriangleMesh *mesh, int i, float ndotv,
                              float u2, float uv, float v2,
                              float &q1, vec2 &t1);
    void compute_Dt1q1(const TriangleMesh *mesh, int i, float ndotv,
                       const vector<float> &q1, const vector<vec2> &t1,
                       float &Dt1q1);
    void draw_segment_app_ridge(int v0, int v1, int v2,
                                float emax0, float emax1, float emax2,
                                float kmax0, float kmax1, float kmax2,
                                const vec &tmax0, const vec &tmax1, const vec &tmax2,
                                float thresh, bool to_center, bool do_test);
    void draw_face_app_ridges(int v0, int v1, int v2,
                              const vector<float> &ndotv, const vector<float> &q1,
                              const vector<vec2> &t1, const vector<float> &Dt1q1,
                              bool do_bfcull, bool do_test, float thresh);
    void draw_mesh_app_ridges(const vector<float> &ndotv, const vector<float> &q1,
                              const vector<vec2> &t1, const vector<float> &Dt1q1,
                              bool do_bfcull, bool do_test, float thresh);

    void draw_apparent_ridges(const vector<float> &ndotv, const vector<float> &q1,
                              const vector<vec2> &t1, const vector<float> &Dt1q1,
                              float thresh);

private:
    float sug_thresh;
    float rv_thresh;
    float ar_thresh;
public:
    //draw_lines
    bool isDrawBoundaries;

    bool isDrawSilhouette;//外部轮廓
    bool isDrawOccludingContours;
    bool isDrawSuggestiveContours;

    bool isDrawIsophotes;
    bool isDrawTopolines;

    bool isDrawRidges;
    bool isDrawValleys;

    bool isDrawApparentRidges;

private:
    TriangleMesh *triMesh;

    xform xf;
    GLCamera camera;
    point viewpos;//当前视点位置

    vec currcolor;// Current line color

    bool isCtrlPressed;

    QTimer *timer;
};

#endif // TRIMESHVIEW_H
