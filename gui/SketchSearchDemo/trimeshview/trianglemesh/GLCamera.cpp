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

GLCamera.cc
Manages OpenGL camera and trackball/arcball interaction
*/

#ifdef __APPLE__
#include <OpenGL/gl.h>
#else
#ifdef _WIN32
#include <windows.h>
#undef min
#undef max
#endif
#include <GL/gl.h>
#endif

#include "GLCamera.h"
using namespace std;


#ifndef M_PI
# define M_PI 3.14159265358979323846
#endif

#define DOF 10.0f
#define MAXDOF 10000.0f
#define SPIN_TIME 0.1f
#define SPIN_SPEED 0.05f
#define TRACKBALL_R 0.8f
#define DEPTH_FUDGE (1.0f + 0.2f * field_of_view)
#define MOVEZ_MULT 5.0f
#define WHEEL_MOVE 0.2f
#define MAX_LIGHT (M_PI-0.001)


// A few functions from GLU, for systems in which it's not included.
//
// (Specifically, Qt 4.8 no longer links to GLU, and just implementing
// the functions we need is simpler than figuring out how to link GLU
// in a way that's cross-platform.)
//
static inline GLint myGluProject(GLdouble objX, GLdouble objY, GLdouble objZ,
	const GLdouble *model, const GLdouble *proj, const GLint *view,
	GLdouble* winX, GLdouble* winY, GLdouble* winZ)
{
	GLdouble x = model[0]*objX + model[4]*objY + model[8]*objZ + model[12];
	GLdouble y = model[1]*objX + model[5]*objY + model[9]*objZ + model[13];
	GLdouble z = model[2]*objX + model[6]*objY + model[10]*objZ + model[14];
	GLdouble w = model[3]*objX + model[7]*objY + model[11]*objZ + model[15];

	GLdouble W = proj[3]*x + proj[7]*y + proj[11]*z + proj[15]*w;
	if (W == 0.0)
		return GL_FALSE;
	GLdouble rW = 1.0 / W;

	GLdouble X = proj[0]*x + proj[4]*y + proj[8]*z + proj[12]*w;
	GLdouble Y = proj[1]*x + proj[5]*y + proj[9]*z + proj[13]*w;
	GLdouble Z = proj[2]*x + proj[6]*y + proj[10]*z + proj[14]*w;

	*winX = (0.5 + 0.5 * X * rW) * view[2] + view[0];
	*winY = (0.5 + 0.5 * Y * rW) * view[3] + view[1];
	*winZ = (0.5 + 0.5 * Z * rW);
	return GL_TRUE;
}

static inline GLint myGluUnProject(GLdouble winX, GLdouble winY, GLdouble winZ,
	const GLdouble *model, const GLdouble *proj, const GLint *view,
	GLdouble* objX, GLdouble* objY, GLdouble* objZ)
{
	xform xf = inv(xform(proj) * xform(model));
	vec v = xf * vec((winX - view[0]) / view[2] * 2.0 - 1.0,
		(winY - view[1]) / view[3] * 2.0 - 1.0,
		winZ * 2.0 - 1.0);
	*objX = v[0];
	*objY = v[1];
	*objZ = v[2];
	return GL_TRUE;
}


// Read back the framebuffer at the given pixel, and determine
// the 3D point there.  If there's nothing there, reads back a
// number of pixels farther and farther away.
bool GLCamera::read_depth(int x, int y, point &p) const
{
	GLdouble M[16], P[16]; GLint V[4];
	glGetDoublev(GL_MODELVIEW_MATRIX, M);
	glGetDoublev(GL_PROJECTION_MATRIX, P);
	glGetIntegerv(GL_VIEWPORT, V);

	static const float dx[] =
	{ 0, 1,-1,-1, 1, 3,-3, 0, 0, 6,-6,-6, 6, 25,-25,  0,  0 };
	static const float dy[] =
	{ 0, 1, 1,-1,-1, 0, 0, 3,-3, 6, 6,-6,-6,  0,  0, 25,-25 };
	const float scale = 0.01f;
	const int displacements = sizeof(dx) / sizeof(float);

	int xmin = V[0], xmax = V[0]+V[2]-1, ymin = V[1], ymax = V[1]+V[3]-1;

	for (int i = 0 ; i < displacements; i++) {
		int xx = min(max(x + int(dx[i]*scale*V[2]), xmin), xmax);
		int yy = min(max(y + int(dy[i]*scale*V[3]), ymin), ymax);
		float d;
		glReadPixels(xx, yy, 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, &d);

		static float maxd = 0.0f;
		if (!maxd) {
			glScissor(xx, yy, 1, 1);
			glEnable(GL_SCISSOR_TEST);
			glClearDepth(1);
			glClear(GL_DEPTH_BUFFER_BIT);
			glReadPixels(xx, yy, 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, &maxd);
			if (maxd) {
				glClearDepth(d / maxd);
				glClear(GL_DEPTH_BUFFER_BIT);
			}
			glDisable(GL_SCISSOR_TEST);
			glClearDepth(1);
			if (!maxd)
				return false;
		}

		d /= maxd;
		if (d > 0.0001f && d < 0.9999f) {
			GLdouble X, Y, Z;
			myGluUnProject(xx, yy, d, M, P, V, &X, &Y, &Z);
			p = point((float)X, (float)Y, (float)Z);
			return true;
		}
	}
	return false;
}


// Mouse helper - decide whether to start auto-spin
void GLCamera::startspin()
{
	float dt = now() - last_time;
	if (dt < SPIN_TIME && fabs(spinspeed) > SPIN_SPEED)
		dospin = true;
}


// Mouse rotation helper - compute trackball position from mouse pos
vec GLCamera::mouse2tb(float x, float y)
{
	float r2 = x*x + y*y;
	float t = 0.5f * sqr(TRACKBALL_R);

	vec pos(x, y, 0);
	if (r2 < t)
		pos[2] = sqrt(2*t - r2);
	else
		pos[2] = t / sqrt(r2);
	return pos;
}

// Mouse helper - rotate
void GLCamera::rotate(int mousex, int mousey, xform &xf)
{
	float ox = (lastmousex - tb_screen_x) / tb_screen_size;
	float oy = (lastmousey - tb_screen_y) / tb_screen_size;
	float nx = (    mousex - tb_screen_x) / tb_screen_size;
	float ny = (    mousey - tb_screen_y) / tb_screen_size;

	vec pos1 = mouse2tb(ox, oy);
	vec pos2 = mouse2tb(nx, ny);
	if (constraint_ == XCONSTRAINED) pos1[1] = pos2[1] = 0.0f;
	if (constraint_ == YCONSTRAINED) pos1[0] = pos2[0] = 0.0f;
	if (constraint_ == ZCONSTRAINED) pos1[2] = pos2[2] = 0.0f;
	spinaxis = pos1 CROSS pos2;
	float spinamount = sqrt(sqr(nx-ox)+sqr(ny-oy));

	xf = xform::trans(spincenter) * xform::rot(spinamount, spinaxis) *
		xform::trans(-spincenter) * xf;

	float dt = now() - last_time;
	if (dt > SPIN_TIME)
		spinspeed = spinamount / SPIN_TIME;
	else
		spinspeed = (spinamount / SPIN_TIME) +
		(1.0f-dt/SPIN_TIME)*spinspeed;
}


// Mouse helper - translate
void GLCamera::movexy(int mousex, int mousey, xform &xf)
{
	float dx = pixscale * click_depth * (mousex - lastmousex);
	float dy = pixscale * click_depth * (mousey - lastmousey);
	xf = xform::trans(dx, dy, 0) * xf;
}


// Mouse helper - translate in Z
// In order to be extra-clever, though, this actually translates along the
// direction of the center of the trackball
void GLCamera::movez(int mousex, int mousey, xform &xf)
{
	float delta = MOVEZ_MULT / field_of_view * pixscale *
		((mousex-lastmousex) - (mousey-lastmousey));
	float dz = click_depth * (exp(-delta) - 1.0f);
	//xf = xform::trans(0, 0, -dz) * xf;
	xf = xform::trans(dz * spincenter / len(spincenter)) * xf;

	surface_depth += dz;
	if (surface_depth < 0)
		surface_depth = 0;
	click_depth += dz;
	if (click_depth < 0)
		click_depth = 0;
}


// Mouse helper - wheel motion
void GLCamera::wheel(Mouse::button updown, xform &xf)
{
	float dz = click_depth * WHEEL_MOVE;
	if (updown == Mouse::WHEELDOWN)
		dz = -dz;
	//xf = xform::trans(0, 0, -dz) * xf;
	xf = xform::trans(dz * spincenter / len(spincenter)) * xf;

	surface_depth += dz;
	if (surface_depth < 0)
		surface_depth = 0;
	click_depth += dz;
	if (click_depth < 0)
		click_depth = 0;
}


// Mouse helper - change lighting direction
void GLCamera::relight(int mousex, int mousey)
{
	GLint V[4];
	glGetIntegerv(GL_VIEWPORT, V);

	float x = 2.0f * float(mousex - V[0]) / float(V[2]) - 1.0f;
	float y = 2.0f * float(mousey - V[1]) / float(V[3]) - 1.0f;

	float theta = (float)MAX_LIGHT * min(sqrt(x*x+y*y), 1.0f);
	float phi = atan2(y, x);

	lightdir[0] = sin(theta)*cos(phi);
	lightdir[1] = sin(theta)*sin(phi);
	lightdir[2] = cos(theta);
}


// Handle a mouse click - sets up rotation center, pixscale, and click_depth
void GLCamera::mouse_click(int mousex, int mousey,
	const point &scene_center, float scene_size)
{
	GLdouble M[16], P[16]; GLint V[4];
#if 0
	glGetDoublev(GL_MODELVIEW_MATRIX, M);
#else
	M[0] = M[5] = M[10] = M[15] = 1.0;
	M[1] = M[2] = M[3] = M[4] =
		M[6] = M[7] = M[8] = M[9] =
		M[11] = M[12] = M[13] = M[14] = 0;
#endif
	glGetDoublev(GL_PROJECTION_MATRIX, P);
	glGetIntegerv(GL_VIEWPORT, V);

	// Assume glFrustum only...
	pixscale = 2.0f / float(max(P[0]*V[2], P[5]*V[3]));


	point surface_point;
	if (read_depth(mousex, mousey, surface_point))
		click_depth = -surface_point[2];
	else
		click_depth = surface_depth;


	GLdouble cx = 0.0, cy = 0.0, cz = 0.0;
	myGluProject(scene_center[0], scene_center[1], scene_center[2],
		M, P, V,
		&cx, &cy, &cz);

	double csize = max(V[2], V[3]);
	int xmin = V[0], xmax = V[0]+V[2], ymin = V[1], ymax = V[1]+V[3];
	if (scene_center[2] < 0 && len(scene_center) > scene_size) {
		csize = -scene_size / scene_center[2] / pixscale;
		xmin = min(max((GLint) (cx - csize), V[0]), V[0]+V[2]);
		xmax = min(max((GLint) (cx + csize), V[0]), V[0]+V[2]);
		ymin = min(max((GLint) (cy - csize), V[1]), V[1]+V[3]);
		ymax = min(max((GLint) (cy + csize), V[1]), V[1]+V[3]);
	}

	GLdouble s[3];
	myGluUnProject((xmin+xmax)/2, (ymin+ymax)/2, 1,
		M, P, V,
		&s[0], &s[1], &s[2]);
	spincenter = vec(s);
	normalize(spincenter);
	if (read_depth((xmin+xmax)/2, (ymin+ymax)/2, surface_point))
		spincenter *=  DEPTH_FUDGE * surface_point[2] / spincenter[2];
	else
		spincenter *= -DEPTH_FUDGE * click_depth / spincenter[2];

	float f = (float)csize / max(V[2], V[3]);
	f = min(max(2.0f * f - 1.0f, 0.0f), 1.0f);
	spincenter = f * spincenter + (1.0f - f) * scene_center;

	myGluProject(spincenter[0], spincenter[1], spincenter[2],
		M, P, V,
		&cx, &cy, &cz);
	tb_screen_x = (float)cx;
	tb_screen_y = (float)cy;
	tb_screen_size = (float)csize;
	tb_screen_size = min(tb_screen_size, 0.7f * min(V[2], V[3]));
	tb_screen_size = max(tb_screen_size, 0.3f * min(V[2], V[3]));
}


// Handle a mouse event 
void GLCamera::mouse(int mousex, int mousey, Mouse::button b,
	const point &scene_center, float scene_size,
	xform &xf)
{
	if (b == Mouse::NONE && lastb == Mouse::NONE)
		return;

	GLint V[4];
	glGetIntegerv(GL_VIEWPORT, V);
	mousey = V[1] + V[3] - 1 - mousey;

	dospin = false;
	if ((b != lastb) && (b != Mouse::NONE))
		mouse_click(mousex, mousey, scene_center, scene_size);

	// Handle rotation
	if ((b == Mouse::ROTATE) && (lastb == Mouse::NONE))
		spinspeed = 0;
	if ((b == Mouse::ROTATE) && (lastb == Mouse::ROTATE))
		rotate(mousex, mousey, xf);
	if ((b == Mouse::NONE) && (lastb == Mouse::ROTATE))
		startspin();

	// Handle translation
	if ((b == Mouse::MOVEXY) && (lastb == Mouse::MOVEXY))
		movexy(mousex, mousey, xf);
	if ((b == Mouse::MOVEZ) && (lastb == Mouse::MOVEZ))
		movez(mousex, mousey, xf);
	if (b == Mouse::WHEELUP || b == Mouse::WHEELDOWN)
		wheel(b, xf);

	// Handle lighting
	if (b == Mouse::LIGHT)
		relight(mousex, mousey);


	lastmousex = mousex;  lastmousey = mousey;  lastb = b;
	last_time = now();
}


// Idle loop - handles auto-rotate.  Returns true iff auto-rotating
bool GLCamera::autospin(xform &xf)
{
	if (!dospin)
		return false;

	float dt = now() - last_time;
	float spinamount = spinspeed * dt;

	xf = xform::trans(spincenter) * xform::rot(spinamount, spinaxis) *
		xform::trans(-spincenter) * xf;

	last_time = now();
	return true;
}


// Set up the OpenGL camera for rendering
void GLCamera::setupGL(const point &scene_center, float scene_size) const
{
	GLint V[4];
	glGetIntegerv(GL_VIEWPORT, V);
	int width = V[2], height = V[3];

	point surface_point;
	if (read_depth(width/2, height/2, surface_point))
		surface_depth = -surface_point[2];

	float fardist  = max(-(scene_center[2] - scene_size),
		scene_size / DOF);
	float neardist = max(-(scene_center[2] + scene_size),
		scene_size / MAXDOF);
	surface_depth = min(surface_depth, fardist);
	surface_depth = max(surface_depth, neardist);
	surface_depth = max(surface_depth, fardist / MAXDOF);
	neardist = max(neardist, surface_depth / DOF);

	float diag = sqrt(float(sqr(width) + sqr(height)));
	float top = (float) height/diag * 0.5f*field_of_view * neardist;
	float bottom = -top;
	float right = (float) width/diag * 0.5f*field_of_view * neardist;
	float left = -right;

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glFrustum(left, right, bottom, top, neardist, fardist);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	GLfloat light0_position[] = { lightdir[0], lightdir[1], lightdir[2], 0 };
	GLfloat light1_position[] = { -lightdir[0], -lightdir[1], -lightdir[2], 0 };
	GLfloat light2_position[] = { lightdir[2], 0, -lightdir[0], 0 };
	GLfloat light3_position[] = { -lightdir[2], 0, lightdir[0], 0 };
	GLfloat light4_position[] = { 0, lightdir[2], -lightdir[1], 0 };
	GLfloat light5_position[] = { 0, -lightdir[2], lightdir[1], 0 };
	glLightfv(GL_LIGHT0, GL_POSITION, light0_position);
	glLightfv(GL_LIGHT1, GL_POSITION, light1_position);
	glLightfv(GL_LIGHT2, GL_POSITION, light2_position);
	glLightfv(GL_LIGHT3, GL_POSITION, light3_position);
	glLightfv(GL_LIGHT4, GL_POSITION, light4_position);
	glLightfv(GL_LIGHT5, GL_POSITION, light5_position);
}

