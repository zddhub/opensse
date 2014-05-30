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
#ifndef COLOR_H
#define COLOR_H
/*
Szymon Rusinkiewicz
Princeton University

Color.h
Random class for encapsulating colors...

Due to all the possible colorspace variants, here's the documentation of
what is actually implemented:
 - CIE 1931 2-degree observer
 - D65 illuminant (including for CIELAB - it's not D50)
 - Rec. 709 primaries
 - Range of [0..1] for all spaces except CIELAB and hue in HSV
 - "RGB" means linear-scaled RGB with the above illuminant and primaries
 - HSV is single-hexcone, sRGB
 - Y'CbCr is JFIF-standard (Rec. 601 scaling, full excursion) starting from sRGB
*/

#include "Vec.h"
#include <cmath>
#include <algorithm>
#ifndef M_PI
# define M_PI 3.14159265358979323846
#endif


class Color : public Vec<3,float> {
public:
	Color()
		{}
	Color(const Vec<3,float> &v_) : Vec<3,float>(v_)
		{}
	Color(const Vec<3,double> &v_) : Vec<3,float>((float)v_[0], (float)v_[1], (float)v_[2])
		{}
	Color(float r, float g, float b) : Vec<3,float>(r,g,b)
		{}
	Color(double r, double g, double b) : Vec<3,float>((float)r, (float)g, (float)b)
		{}
	explicit Color(const float *rgb) : Vec<3,float>(rgb[0], rgb[1], rgb[2])
		{}
	explicit Color(const double *rgb) : Vec<3,float>((float)rgb[0], (float)rgb[1], (float)rgb[2])
		{}

	// Implicit conversion from float would be bad, so we have an
	// explicit constructor and an assignment statement.
	explicit Color(float c) : Vec<3,float>(c,c,c)
		{}
	explicit Color(double c) : Vec<3,float>((float)c, (float)c, (float)c)
		{}
	Color &operator = (float c)
		{ return *this = Color(c); }
	Color &operator = (double c)
		{ return *this = Color(c); }

	// Assigning from ints divides by 255
	Color(int r, int g, int b)
	{
		const float mult = 1.0f / 255.0f;
		*this = Color(mult*r, mult*g, mult*b);
	}
	explicit Color(const int *rgb)
		{ *this = Color(rgb[0], rgb[1], rgb[2]); }
	explicit Color(const unsigned char *rgb)
		{ *this = Color(rgb[0], rgb[1], rgb[2]); }
	explicit Color(int c)
		{ *this = Color(c,c,c); }
	Color &operator = (int c)
		{ return *this = Color(c); }

	static Color black()
		{ return Color(0.0f, 0.0f, 0.0f); }
	static Color white()
		{ return Color(1.0f, 1.0f, 1.0f); }
	static Color red()
		{ return Color(1.0f, 0.0f, 0.0f); }
	static Color green()
		{ return Color(0.0f, 1.0f, 0.0f); }
	static Color blue()
		{ return Color(0.0f, 0.0f, 1.0f); }
	static Color yellow()
		{ return Color(1.0f, 1.0f, 0.0f); }
	static Color cyan()
		{ return Color(0.0f, 1.0f, 1.0f); }
	static Color magenta()
		{ return Color(1.0f, 0.0f, 1.0f); }

	// 3x3 color transform - matrix given in *row-major* order
	const Color col_transform(float m11, float m12, float m13,
				  float m21, float m22, float m23,
				  float m31, float m32, float m33) const
	{
		return Color(m11*v[0]+m12*v[1]+m13*v[2],
			     m21*v[0]+m22*v[1]+m23*v[2],
			     m31*v[0]+m32*v[1]+m33*v[2]);
	}

private:
	const Color hsv2srgb() const
	{
		// From FvD
		float H = v[0], S = v[1], V = v[2];
		if (S <= 0.0f)
			return Color(V,V,V);
		H = std::fmod(H, float(2.0 * M_PI));
		if (H < 0.0f)
			H += float(2.0 * M_PI);
		H *= float(3.0 / M_PI);
		int i = int(std::floor(H));
		float f = H - i;
		float p = V * (1.0f - S);
		float q = V * (1.0f - (S*f));
		float t = V * (1.0f - (S*(1.0f-f)));
		switch(i) {
			case 0: return Color(V, t, p);
			case 1: return Color(q, V, p);
			case 2: return Color(p, V, t);
			case 3: return Color(p, q, V);
			case 4: return Color(t, p, V);
			default: return Color(V, p, q);
		}
	}
	const Color srgb2hsv() const
	{
		float V = std::max(std::max(v[0], v[1]), v[2]);
		float diff = V - std::min(std::min(v[0], v[1]), v[2]);
		float S = diff / V;
		float H = 0.0f;
		if (S == 0.0f)
			return Color(H, S, V);
		if (V == v[0])
			H = (v[1] - v[2]) / diff;
		else if (V == v[1])
			H = (v[2] - v[0]) / diff + 2.0f;
		else
			H = (v[0] - v[1]) / diff + 4.0f;
		H *= float(M_PI / 3.0);
		if (H < 0.0f)
			H += float(2.0 * M_PI);
		return Color(H, S, V);
	}

	static inline float cielab_nonlinearity(float x)
	{
		if (x > 216.0f / 24389.0f)
			return cbrt(x);
		else
			return 4.0f / 29.0f + (841.0f / 108.0f) * x;
	}
	static inline float inv_cielab_nonlinearity(float x)
	{
		if (x > (6.0f / 29.0f))
			return cube(x);
		else
			return (x - 4.0f / 29.0f) * (108.0f / 841.0f);
	}
	const Color xyz2cielab() const
	{
		float fx = cielab_nonlinearity(v[0] * (1.0f / 0.95047f));
		float fy = cielab_nonlinearity(v[1]);
		float fz = cielab_nonlinearity(v[2] * (1.0f / 1.08883f));
		return Color(116.0f * fy - 16.0f,
			     500.0f * (fx - fy),
			     200.0f * (fy - fz));
	}
	const Color cielab2xyz() const
	{
		float fy = (v[0] + 16.0f) * (1.0f / 116.0f);
		float fx = fy + v[1] * 0.002f;
		float fz = fy - v[2] * 0.005f;
		return Color(0.95047f * inv_cielab_nonlinearity(fx),
			     inv_cielab_nonlinearity(fy),
			     1.08883f * inv_cielab_nonlinearity(fz));
	}

	const Color xyz2rgb() const
	{
		return col_transform(3.24071f, -1.53726f, -0.498571f,
 				     -0.969258f, 1.87599f, 0.0415557f,
				     0.0556352f, -0.203996f, 1.05707f);
	}
	const Color rgb2xyz() const
	{
		return col_transform(0.412424f, 0.357579f, 0.180464f,
				     0.212656f, 0.715158f, 0.0721856f,
				     0.0193324f, 0.119193f, 0.950444f);
	}

	static inline float srgb_nonlinearity(float x)
	{
		if (x > 0.0031308f)
			return 1.055f * pow(x, 1.0f/2.4f) - 0.055f;
		else
			return x * 12.92f;
	}
	static inline float inv_srgb_nonlinearity(float x)
	{
		if (x > (0.0031308f * 12.92f))
			return pow((x + 0.055f) * (1.0f / 1.055f), 2.4f);
		else
			return x * (1.0f / 12.92f);
	}
	const Color rgb2srgb() const
	{
		return Color(srgb_nonlinearity(v[0]),
			     srgb_nonlinearity(v[1]),
			     srgb_nonlinearity(v[2]));
	}
	const Color srgb2rgb() const
	{
		return Color(inv_srgb_nonlinearity(v[0]),
			     inv_srgb_nonlinearity(v[1]),
			     inv_srgb_nonlinearity(v[2]));
	}

	const Color srgb2ycbcr() const
	{
		return Color(0.0f, 0.5f, 0.5f) + col_transform(
			0.299f, 0.587f, 0.114f,
			-0.168736f, -0.331264f, 0.5f,
			0.5f, -0.418688f, -0.081312f);
	}
	const Color ycbcr2srgb() const
	{
		return Color(v[0], v[1] - 0.5f, v[2] - 0.5f).col_transform(
			     1.0f, 0.0f, 1.402f,
			     1.0f, -0.344136f, -0.714136f,
			     1.0f, 1.772f, 0.0f);
	}

public:
	enum Colorspace { CIELAB, XYZ, RGB, SRGB, YCBCR, HSV };
	const Color convert(Colorspace src, Colorspace dst) const
	{
		if (src == dst)
			return Color(*this);
		if (src == HSV)
			return Color::hsv(v[0],v[1],v[2]).convert(SRGB, dst);
		else if (dst == HSV)
			return convert(src, SRGB).srgb2hsv();
		// Else we have a natural order in which to convert things
		int srcnum = int(src), dstnum = int(dst);
		if (srcnum < dstnum) switch (src) {
			case CIELAB:
				return (dst == XYZ) ? cielab2xyz() :
					cielab2xyz().convert(XYZ, dst);
			case XYZ:
				return (dst == RGB) ? xyz2rgb() :
					xyz2rgb().convert(RGB, dst);
			case RGB:
				return (dst == SRGB) ? rgb2srgb() :
					rgb2srgb().convert(SRGB, dst);
			default:
				return srgb2ycbcr();
		} else switch (src) {
			case YCBCR:
				return (dst == SRGB) ? ycbcr2srgb() :
					ycbcr2srgb().convert(SRGB, dst);
			case SRGB:
				return (dst == RGB) ? srgb2rgb() :
					srgb2rgb().convert(RGB, dst);
			case RGB:
				return (dst == XYZ) ? rgb2xyz() :
					rgb2xyz().convert(XYZ, dst);
			default:
				return xyz2cielab();
		}
	}

	// Linear to nonlinear - raises values to the power of 1/g
	const Color gamma(float g) const
	{
		float g1 = 1.0f / g;
		return Color(pow(v[0],g1), pow(v[1],g1), pow(v[2],g1));
	}

	// Just apply the nonlinearity, not full colorspace conversion
	const Color gamma(Colorspace dst) const
	{
		switch (dst) {
			case CIELAB:
				return Color(cielab_nonlinearity(v[0]),
					     cielab_nonlinearity(v[1]),
					     cielab_nonlinearity(v[2]));
			case SRGB:
			case YCBCR:
				return Color(srgb_nonlinearity(v[0]),
					     srgb_nonlinearity(v[1]),
					     srgb_nonlinearity(v[2]));
			default:
				return Color(*this);
		}
	}

	// Nonlinear to linear - raises values to the power of g
	const Color ungamma(float g) const
	{
		return Color(pow(v[0],g), pow(v[1],g), pow(v[2],g));
	}

	const Color ungamma(Colorspace dst) const
	{
		switch (dst) {
			case CIELAB:
				return Color(inv_cielab_nonlinearity(v[0]),
					     inv_cielab_nonlinearity(v[1]),
					     inv_cielab_nonlinearity(v[2]));
			case SRGB:
			case YCBCR:
				return Color(inv_srgb_nonlinearity(v[0]),
					     inv_srgb_nonlinearity(v[1]),
					     inv_srgb_nonlinearity(v[2]));
			default:
				return Color(*this);
		}
	}

	// For backwards compatibility with earlier versions of Color.h,
	// this stays as a static method.  New code should use convert().
	static Color hsv(float h, float s, float v)
	{
		return Color(h,s,v).hsv2srgb();
	}
};

#endif
