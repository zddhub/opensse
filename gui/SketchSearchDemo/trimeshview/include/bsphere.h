#ifndef BSPHERE_H
#define BSPHERE_H
//    Copright (C) 1999
//
//    This program is free software; you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation; either version 2 of the License, or
//    (at your option) any later version.
//
//    This program is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//    GNU General Public License for more details.
//
//    You should have received a copy of the GNU General Public License
//    along with this program; if not, write to the Free Software
//    Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA,
//    or download the License terms from prep.ai.mit.edu/pub/gnu/COPYING-2.0.
//
//    Contact:
//    --------
//    Bernd Gaertner
//    Institut f. Informatik
//    ETH Zuerich
//    ETH-Zentrum
//    CH-8092 Zuerich, Switzerland
//    http://www.inf.ethz.ch/personal/gaertner
//


#include "Vec.h"
#include <list>


// Class for a basis of points supporting a bounding sphere
template<int D, class T>
class Basis {
private:
	int m, s;		// size and number of support points
	T q0[D];

	T z[D+1];
	T f[D+1];
	T v[D+1][D];
	T a[D+1][D];

	T c[D+1][D];
	T sqr_r[D+1];

	T *current_c;		// points to some c[j]
	T current_sqr_r;

public:
	const T *center() const { return current_c; }
	T squared_radius() const { return current_sqr_r; }
	int size() const { return m; }
	T excess(const Vec<D,T> &p) const;
	void reset();		// generates empty sphere with m=s=0
	bool push(const Vec<D,T> &p);
	void pop();
};


// Class for hoding and computing the bounding sphere
template <int D, class T>
class Miniball {
public:
	typedef typename std::list< Vec<D,T> >::iterator It;

private:
	std::list< Vec<D,T> > L;// STL list keeping the points
	Basis<D,T> B;		// basis keeping the current ball
	It support_end;		// past-the-end iterator of support set

	void move_to_front(It j);
	T max_excess(It t, It i, It &pivot) const;
	void mtf_mb(It k);
	void pivot_mb(It k);

public:
	void check_in(const Vec<D,T> &p) { L.push_back(p); }
	template <class I>
	void check_in(I first, I last) { L.insert(L.end(), first, last); }
	void build(bool pivoting = true);
	Vec<D,T> center() const { return Vec<D,T>(B.center()); }
	T squared_radius() const { return B.squared_radius(); }
};


template <int D, class T>
T Basis<D,T>::excess(const Vec<D,T> &p) const
{
	T e = -current_sqr_r;
	for (int k = 0; k < D; k++)
		e += sqr(p[k] - current_c[k]);
	return e;
}


template <int D, class T>
void Basis<D,T>::reset()
{
	m = s = 0;
	// we misuse c[0] for the center of the empty sphere
	for (int j = 0; j < D; j++)
		c[0][j] = 0;
	current_c = c[0];
	current_sqr_r = -1;
}


template <int D, class T>
bool Basis<D,T>::push(const Vec<D,T> &p)
{
	int i, j;
	const T eps = T(1.0e-13);
	if (m == 0) {
		for (i = 0; i < D; i++)
			q0[i] = p[i];
		for (i = 0; i < D; i++)
			c[0][i] = q0[i];
		sqr_r[0] = 0;
	} else {
		// set v_m to Q_m
		for (i = 0; i < D; i++)
			v[m][i] = p[i] - q0[i];
   
		// compute the a_{m,i}, i < m
		for (i = 1; i < m; i++) {
			a[m][i] = 0;
			for (j = 0; j < D; j++)
				a[m][i] += v[i][j] * v[m][j];
			a[m][i] *= (T(2) / z[i]);
		}
   
		// update v_m to Q_m-\bar{Q}_m
		for (i = 1; i < m; i++) {
			for (j = 0; j < D; j++)
				v[m][j] -= a[m][i] * v[i][j];
		}
   
		// compute z_m
		z[m] = 0;
		for (j = 0; j < D; j++)
			z[m] += sqr(v[m][j]);
		z[m] *= T(2);
   
		// reject push if z_m too small
		if (z[m] < eps*current_sqr_r)
			return false;
   
		// update c, sqr_r
		T e = -sqr_r[m-1];
		for (i = 0; i < D; i++)
			e += sqr(p[i] - c[m-1][i]);
		f[m] = e / z[m];
   
		for (i = 0; i < D; i++)
			c[m][i] = c[m-1][i] + f[m]*v[m][i];
		sqr_r[m] = sqr_r[m-1] + e*f[m]*T(0.5);
       }
       current_c = c[m];
       current_sqr_r = sqr_r[m];
       s = ++m;
       return true;
}
   

template <int D, class T>
void Basis<D,T>::pop()
{
	m--;
}
   

template <int D, class T>
void Miniball<D,T>::move_to_front(It j)
{
	if (support_end == j)
		support_end++;
	L.splice(L.begin(), L, j);
}


template <int D, class T>
T Miniball<D,T>::max_excess(It t, It i, It &pivot) const
{
	const T *c = B.center(), sqr_r = B.squared_radius();
	T e, max_e = 0;
	for (It k = t; k != i; k++) {
		const Vec<D,T> &p = *k;
		e = -sqr_r;
		for (int j = 0; j < D; j++)
			e += sqr(p[j] - c[j]);
		if (e > max_e) {
			max_e = e;
			pivot = k;
		}
	}
	return max_e;
}


template <int D, class T>
void Miniball<D,T>::mtf_mb(It i)
{
	support_end = L.begin();
	if (B.size() == D+1)
		return;
	for (It k = L.begin(); k != i; ) {
		It j = k++;
		if (B.excess(*j) > 0) {
			if (B.push(*j)) {
				mtf_mb(j);
				B.pop();
				move_to_front(j);
			}
		}
	}
}


template <int D, class T>
void Miniball<D,T>::pivot_mb(It i)
{
	It t = ++L.begin();
	mtf_mb(t);
	T max_e, old_sqr_r = T(0);
	do {
		It pivot;
		max_e = max_excess(t, i, pivot);
		if (max_e > 0) {
			t = support_end;
			if (t == pivot)
				t++;
			old_sqr_r = B.squared_radius();
			B.push(*pivot);
			mtf_mb(support_end);
			B.pop();
			move_to_front(pivot);
		}
	} while (max_e > 0 && B.squared_radius() > old_sqr_r);
}

template <int D, class T>
void Miniball<D,T>::build(bool pivoting /* = true */)
{
	B.reset();
	support_end = L.begin();
	if (pivoting)
		pivot_mb(L.end());
	else
		mtf_mb(L.end());
}

#endif
