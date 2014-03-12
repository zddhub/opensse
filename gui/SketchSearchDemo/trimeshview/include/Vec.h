#ifndef VEC_H
#define VEC_H
/*
Szymon Rusinkiewicz
Princeton University

Vec.h
Class for a constant-length vector

Supports the following operations:
	vec v1;			// Initialized to (0,0,0)
	vec v2(1,2,3);		// Initialized to (1,2,3)
	vec v3(v2);		// Copy constructor
	float farray[3];
	vec v4 = vec(farray);	// Explicit: "v4 = farray" won't work
	Vec<3,double> vd;	// The "vec" used above is Vec<3,float>
	point p1, p2, p3;	// Same as vec

	v3 = v1 + v2;		// Also -, *, /  (all componentwise)
	v3 = 3.5f * v1;		// Also vec * scalar, vec / scalar
				// NOTE: scalar has to be the same type:
				// it won't work to do double * vec<float>
	v1 = min(v2,v3);	// Componentwise min/max
	v1 = sin(v2);		// Componentwise - all the usual functions...
	swap(v1,v2);		// In-place swap

	v3 = v1 DOT v2;		// Actually operator^
	v3 = v1 CROSS v2;	// Actually operator%

	float f = v1[0];	// Subscript
	float *fp = v1;		// Implicit conversion to float *

	f = len(v1);		// Length (also len2 == squared length)
	f = dist(p1, p2);	// Distance (also dist2 == squared distance)
	normalize(v1);		// Normalize (i.e., make it unit length)
				// normalize(vec(0,0,0)) => vec(1,0,0)
	v1 = trinorm(p1,p2,p3); // Normal of triangle (area-weighted)

	cout << v1 << endl;	// iostream output in the form (1,2,3)
	cin >> v2;		// iostream input using the same syntax

Also defines the utility functions sqr, cube, sgn, fract, clamp, mix,
step, smoothstep, faceforward, reflect, and refract
*/


// Windows defines min and max as macros, which prevents us from using the
// type-safe versions from std::, as well as interfering with method defns.
// Also define NOMINMAX, which prevents future bad definitions.
#ifdef min
# undef min
#endif
#ifdef max
# undef max
#endif
#ifndef NOMINMAX
# define NOMINMAX
#endif

#include <cmath>
#include <iostream>
#include <algorithm>
#include <cstddef>


// Let gcc optimize conditional branches a bit better...
#ifndef likely
#  if !defined(__GNUC__) || (__GNUC__ == 2 && __GNUC_MINOR__ < 96)
#    define likely(x) (x)
#    define unlikely(x) (x)
#  else
#    define likely(x)   (__builtin_expect((x), 1))
#    define unlikely(x) (__builtin_expect((x), 0))
#  endif
#endif


// Boost-like compile-time assertion checking
template <bool X> struct VEC_STATIC_ASSERTION_FAILURE;
template <> struct VEC_STATIC_ASSERTION_FAILURE<true>
	{ void operator () () {} };
#define VEC_STATIC_CHECK(expr) VEC_STATIC_ASSERTION_FAILURE<bool(expr)>()


template <int D, class T = float>
class Vec {
protected:
	T v[D];

public:
	// Constructor for no arguments.  Everything initialized to 0.
	Vec() { for (int i = 0; i < D; i++) v[i] = T(0); }

	// Uninitialized constructor - meant mostly for internal use
#define VEC_UNINITIALIZED ((void *) 0)
	Vec(void *) {}

	// Constructors for 2-4 arguments
	Vec(T x, T y)
		{ VEC_STATIC_CHECK(D == 2); v[0] = x; v[1] = y; }
	Vec(T x, T y, T z)
		{ VEC_STATIC_CHECK(D == 3); v[0] = x; v[1] = y; v[2] = z; }
	Vec(T x, T y, T z, T w)
		{ VEC_STATIC_CHECK(D == 4); v[0] = x; v[1] = y; v[2] = z; v[3] = w; }

	// Constructor from anything that can be accessed using []
	// Pretty aggressive, so marked as explicit.
	template <class S> explicit Vec(const S &x)
		{ for (int i = 0; i < D; i++) v[i] = T(x[i]); }

	// No destructor or assignment operator needed

	// Array reference and conversion to pointer - no bounds checking
	const T &operator [] (int i) const
		{ return v[i]; }
	T &operator [] (int i)
		{ return v[i]; }
	operator const T * () const
		{ return v; }
	operator const T * ()
		{ return v; }
	operator T * ()
		{ return v; }

	// Member operators
	Vec<D,T> &operator += (const Vec<D,T> &x)
	{
		for (int i = 0; i < D; i++)
#pragma omp atomic
			v[i] += x[i];
		return *this;
	}
	Vec<D,T> &operator -= (const Vec<D,T> &x)
	{
		for (int i = 0; i < D; i++)
#pragma omp atomic
			v[i] -= x[i];
		return *this;
	}
	Vec<D,T> &operator *= (const Vec<D,T> &x)
	{
		for (int i = 0; i < D; i++)
#pragma omp atomic
			v[i] *= x[i];
		return *this;
	}
	Vec<D,T> &operator *= (const T &x)
	{
		for (int i = 0; i < D; i++)
#pragma omp atomic
			v[i] *= x;
		return *this;
	}
	Vec<D,T> &operator /= (const Vec<D,T> &x)
	{
		for (int i = 0; i < D; i++)
#pragma omp atomic
			v[i] /= x[i];
		return *this;
	}
	Vec<D,T> &operator /= (const T &x)
	{
		for (int i = 0; i < D; i++)
#pragma omp atomic
			v[i] /= x;
		return *this;
	}

	// Set each component to min/max of this and the other vector
	Vec<D,T> &min(const Vec<D,T> &x)
	{
#pragma omp critical
		for (int i = 0; i < D; i++)
			if (x[i] < v[i]) v[i] = x[i];
		return *this;
	}
	Vec<D,T> &max(const Vec<D,T> &x)
	{
#pragma omp critical
		for (int i = 0; i < D; i++)
			if (x[i] > v[i]) v[i] = x[i];
		return *this;
	}

	// Swap with another vector.  (Also exists as a global function.)
	void swap(Vec<D,T> &x)
	{
		using namespace std;
#pragma omp critical
		for (int i = 0; i < D; i++) swap(v[i], x[i]);
	}

	// Outside of class: + - * / % ^ << >>

	// Some partial compatibility with std::vector
	typedef T value_type;
	typedef T *pointer;
	typedef const T *const_pointer;
	typedef T *iterator;
	typedef const T *const_iterator;
	typedef T &reference;
	typedef const T &const_reference;
	typedef size_t size_type;
	typedef ptrdiff_t difference_type;

	size_t size() const
		{ return D; }
	T *begin()
		{ return &(v[0]); }
	const T *begin() const
		{ return &(v[0]); }
	T *end()
		{ return begin() + D; }
	const T *end() const
		{ return begin() + D; }

	// clear() and empty() - set to zero or check for all zero
	void clear()
		{ for (int i = 0; i < D; i++) v[i] = T(0); }
	bool empty() const
	{
		for (int i = 0; i < D; i++)
			if (v[i]) return false;
		return true;
	}

	// Some partial compatibility with std::valarray, plus generalizations
	T sum() const
	{
		T total = v[0];
		for (int i = 1; i < D; i++)
			total += v[i];
		return total;
	}
	T sumabs() const
	{
		T total = fabs(v[0]);
		for (int i = 1; i < D; i++)
			total += fabs(v[i]);
		return total;
	}
	T avg() const
		{ return sum() / D; }
	T product() const
	{
		T total = v[0];
		for (int i = 1; i < D; i++)
			total *= v[i];
		return total;
	}
	T min() const
	{
		T m = v[0];
		for (int i = 1; i < D; i++)
			if (v[i] < m) m = v[i];
		return m;
	}
	T max() const
	{
		T m = v[0];
		for (int i = 1; i < D; i++)
			if (v[i] > m) m = v[i];
		return m;
	}
	Vec<D,T> apply(T func(T)) const
	{
		Vec<D,T> result(VEC_UNINITIALIZED);
		for (int i = 0; i < D; i++) result[i] = func(v[i]);
		return result;
	}
	Vec<D,T> apply(T func(const T&)) const
	{
		Vec<D,T> result(VEC_UNINITIALIZED);
		for (int i = 0; i < D; i++) result[i] = func(v[i]);
		return result;
	}
	Vec<D,T> cshift(int n) const
	{
		Vec<D,T> result(VEC_UNINITIALIZED);
		if (n < 0)
			n = (n % D) + D;
		for (int i = 0; i < D; i++)
			result[i] = v[(i+n)%D];
		return result;
	}
	Vec<D,T> shift(int n) const
	{
		if (abs(n) >= D)
			return Vec<D,T>();
		Vec<D,T> result; // Must be initialized to zero
		int start = n < T(0) ? -n : 0;
		int stop = n > T(0) ? D - n : D;
		for (int i = start; i < stop; i++) result[i] = v[i+n];
		return result;
	}
};


// Shorthands for particular flavors of Vecs
typedef Vec<3,float> vec;
typedef Vec<3,float> point;
typedef Vec<2,float> vec2;
typedef Vec<3,float> vec3;
typedef Vec<4,float> vec4;
typedef Vec<2,int> ivec2;
typedef Vec<3,int> ivec3;
typedef Vec<4,int> ivec4;

typedef Vec<3, unsigned short int> uvec3;


// Nonmember operators that take two Vecs
template <int D, class T>
static inline const Vec<D,T> operator + (const Vec<D,T> &v1, const Vec<D,T> &v2)
{
	Vec<D,T> result(VEC_UNINITIALIZED);
	for (int i = 0; i < D; i++)
		result[i] = v1[i] + v2[i];
	return result;
}

template <int D, class T>
static inline const Vec<D,T> operator - (const Vec<D,T> &v1, const Vec<D,T> &v2)
{
	Vec<D,T> result(VEC_UNINITIALIZED);
	for (int i = 0; i < D; i++)
		result[i] = v1[i] - v2[i];
	return result;
}

template <int D, class T>
static inline const Vec<D,T> operator * (const Vec<D,T> &v1, const Vec<D,T> &v2)
{
	Vec<D,T> result(VEC_UNINITIALIZED);
	for (int i = 0; i < D; i++)
		result[i] = v1[i] * v2[i];
	return result;
}

template <int D, class T>
static inline const Vec<D,T> operator / (const Vec<D,T> &v1, const Vec<D,T> &v2)
{
	Vec<D,T> result(VEC_UNINITIALIZED);
	for (int i = 0; i < D; i++)
		result[i] = v1[i] / v2[i];
	return result;
}


// Dot product
template <int D, class T>
static inline const T operator ^ (const Vec<D,T> &v1, const Vec<D,T> &v2)
{
	T sum = v1[0] * v2[0];
	for (int i = 1; i < D; i++)
		sum += v1[i] * v2[i];
	return sum;
}
#define DOT ^


// Cross product - only in 3 dimensions
template <class T>
static inline const Vec<3,T> operator % (const Vec<3,T> &v1, const Vec<3,T> &v2)
{
	return Vec<3,T>(v1[1]*v2[2] - v1[2]*v2[1],
			v1[2]*v2[0] - v1[0]*v2[2],
			v1[0]*v2[1] - v1[1]*v2[0]);
}
#define CROSS %


// Component-wise equality and inequality (#include the usual caveats
// about comparing floats for equality...)
template <int D, class T>
static inline bool operator == (const Vec<D,T> &v1, const Vec<D,T> &v2)
{
	for (int i = 0; i < D; i++)
		if (v1[i] != v2[i])
			return false;
	return true;
}

template <int D, class T>
static inline bool operator != (const Vec<D,T> &v1, const Vec<D,T> &v2)
{
	for (int i = 0; i < D; i++)
		if (v1[i] != v2[i])
			return true;
	return false;
}


// Unary operators
template <int D, class T>
static inline const Vec<D,T> &operator + (const Vec<D,T> &v)
{
	return v;
}

template <int D, class T>
static inline const Vec<D,T> operator - (const Vec<D,T> &v)
{
	Vec<D,T> result(VEC_UNINITIALIZED);
	for (int i = 0; i < D; i++)
		result[i] = -v[i];
	return result;
}

template <int D, class T>
static inline bool operator ! (const Vec<D,T> &v)
{
	return v.empty();
}


// Vec/scalar operators
template <int D, class T>
static inline const Vec<D,T> operator * (const T &x, const Vec<D,T> &v)
{
	Vec<D,T> result(VEC_UNINITIALIZED);
	for (int i = 0; i < D; i++)
		result[i] = x * v[i];
	return result;
}

template <int D, class T>
static inline const Vec<D,T> operator * (const Vec<D,T> &v, const T &x)
{
	Vec<D,T> result(VEC_UNINITIALIZED);
	for (int i = 0; i < D; i++)
		result[i] = v[i] * x;
	return result;
}

template <int D, class T>
static inline const Vec<D,T> operator / (const T &x, const Vec<D,T> &v)
{
	Vec<D,T> result(VEC_UNINITIALIZED);
	for (int i = 0; i < D; i++)
		result[i] = x / v[i];
	return result;
}

template <int D, class T>
static inline const Vec<D,T> operator / (const Vec<D,T> &v, const T &x)
{
	Vec<D,T> result(VEC_UNINITIALIZED);
	for (int i = 0; i < D; i++)
		result[i] = v[i] / x;
	return result;
}


// iostream operators
template <int D, class T>
static inline std::ostream &operator << (std::ostream &os, const Vec<D,T> &v)

{
	os << "(";
	for (int i = 0; i < D-1; i++)
		os << v[i] << ", ";
	return os << v[D-1] << ")";
}

template <int D, class T>
static inline std::istream &operator >> (std::istream &is, Vec<D,T> &v)
{
	char c1 = 0, c2 = 0;

	is >> c1;
	if (c1 == '(' || c1 == '[') {
		is >> v[0] >> std::ws >> c2;
		for (int i = 1; i < D; i++) {
			if (c2 == ',')
				is >> v[i] >> std::ws >> c2;
			else
				is.setstate(std::ios::failbit);
		}
	}

	if (c1 == '(' && c2 != ')')
		is.setstate(std::ios::failbit);
	else if (c1 == '[' && c2 != ']')
		is.setstate(std::ios::failbit);

	return is;
}


// Swap two Vecs.  Not atomic, unlike class method.
namespace std {
  template <int D, class T>
  static inline void swap(const Vec<D,T> &v1, const Vec<D,T> &v2)
  {
	for (int i = 0; i < D; i++)
		swap(v1[i], v2[i]);
  }
}


// Squared length
template <int D, class T>
static inline const T len2(const Vec<D,T> &v)
{
	T l2 = v[0] * v[0];
	for (int i = 1; i < D; i++)
		l2 += v[i] * v[i];
	return l2;
}


// Length
template <int D, class T>
static inline const T len(const Vec<D,T> &v)
{
	return std::sqrt(len2(v));
}

// Utility functions for square and cube, to go along with sqrt and cbrt
template <class T>
static inline T sqr(const T &x)
{
    return x*x;
}

template <class T>
static inline T cube(const T &x)
{
    return x*x*x;
}

// Squared distance
template <int D, class T>
static inline const T dist2(const Vec<D,T> &v1, const Vec<D,T> &v2)
{
	T d2 = sqr(v2[0]-v1[0]);
	for (int i = 1; i < D; i++)
		d2 += sqr(v2[i]-v1[i]);
	return d2;
}


// Distance
template <int D, class T>
static inline const T dist(const Vec<D,T> &v1, const Vec<D,T> &v2)
{
	return std::sqrt(dist2(v1,v2));
}


// In-place normalization to unit length
template <int D, class T>
static inline Vec<D,T> normalize(Vec<D,T> &v)
{
	T l = len(v);
	if (unlikely(l <= T(0))) {
		v[0] = T(1);
		for (int i = 1; i < D; i++)
			v[i] = T(0);
		return v;
	}

	l = T(1) / l;
	for (int i = 0; i < D; i++)
		v[i] *= l;

	return v;
}


// Area-weighted triangle face normal
template <class T>
static inline T trinorm(const T &v0, const T &v1, const T &v2)
{
	return (typename T::value_type) 0.5 * ((v1 - v0) CROSS (v2 - v0));
}


// Sign of a scalar.  Note that sgn(0) == 1.
template <class T>
static inline T sgn(const T &x)
{
	return (x < T(0)) ? T(-1) : T(1);
}


// Utility functions based on GLSL
template <class T>
static inline T fract(const T &x)
{
	return x - floor(x);
}

template <class T>
static inline T clamp(const T &x, const T &a, const T &b)
{
	return x > a ? x < b ? x : b : a;  // returns a on NaN
}

template <class T, class S>
static inline T mix(const T &x, const T &y, const S &a)
{
	return (S(1)-a) * x + a * y;
}

template <class T>
static inline T step(const T &x, const T &a)
{
	return x < a ? T(0) : T(1);
}

template <class T>
static inline T smoothstep(const T &a, const T &b, const T &x)
{
	if (b <= a) return step(x,a);
	T t = (x - a) / (b - a);
	return t <= T(0) ? T(0) : t >= T(1) ? T(1) : t * t * (T(3) - T(2) * t);
}

template <int D, class T>
static inline T faceforward(const Vec<D,T> &N, const Vec<D,T> &I,
			    const Vec<D,T> &Nref)
{
	return ((Nref DOT I) < T(0)) ? N : -N;
}

template <int D, class T>
static inline T reflect(const Vec<D,T> &I, const Vec<D,T> &N)
{
	return I - (T(2) * (N DOT I)) * N;
}

template <int D, class T>
static inline T refract(const Vec<D,T> &I, const Vec<D,T> &N,
			const T &eta)
{
	T NdotI = N DOT I;
	T k = T(1) - sqr(eta) * (T(1) - sqr(NdotI));
	return (k < T(0)) ? T(0) : eta * I - (eta * NdotI * std::sqrt(k)) * N;
}


// C99 compatibility functions for MSVS
#ifdef _WIN32
#ifdef cbrt
# undef cbrt
#endif
inline float cbrt(float x)
{
	return (x < 0.0f) ? -std::pow(-x, 1.0f / 3.0f) : std::pow(x, 1.0f / 3.0f);
}
inline double cbrt(double x)
{
	return (x < 0.0) ? -std::pow(-x, 1.0 / 3.0) : std::pow(x, 1.0 / 3.0);
}
inline long double cbrt(long double x)
{
	return (x < 0.0L) ? -std::pow(-x, 1.0L / 3.0L) : std::pow(x, 1.0L / 3.0L);
}
#ifdef round
# undef round
#endif
inline float round(float x)
{
	return (x < 0.0f) ? float(int(x - 0.5f)) : float(int(x + 0.5f));
}
inline double round(double x)
{
	return (x < 0.0f) ? double(int(x - 0.5)) : double(int(x + 0.5));
}
inline long double round(long double x)
{
	return (x < 0.0f) ? (long double)(int(x - 0.5L)) : (long double)(int(x + 0.5L));
}
#ifdef trunc
# undef trunc
#endif
inline float trunc(float x)
{
	return (x < 0.0f) ? float(int(x)) : float(int(x));
}
inline double trunc(double x)
{
	return (x < 0.0f) ? double(int(x)) : double(int(x));
}
inline long double trunc(long double x)
{
	return (x < 0.0f) ? (long double)(int(x)) : (long double)(int(x));
}
#endif



// Generic macros for declaring 1-, 2-, and 3- argument
// componentwise functions on vecs
#define VEC_DECLARE_ONEARG(name) \
 template <int D, class T> \
 static inline Vec<D,T> name(const Vec<D,T> &v) \
 { \
	using namespace std; \
	Vec<D,T> result(VEC_UNINITIALIZED); \
	for (int i = 0; i < D; i++) \
		result[i] = name(v[i]); \
	return result; \
 }

#define VEC_DECLARE_TWOARG(name) \
 template <int D, class T> \
 static inline Vec<D,T> name(const Vec<D,T> &v, const T &w) \
 { \
	using namespace std; \
	Vec<D,T> result(VEC_UNINITIALIZED); \
	for (int i = 0; i < D; i++) \
		result[i] = name(v[i], w); \
	return result; \
 } \
 template <int D, class T> \
 static inline Vec<D,T> name(const Vec<D,T> &v, const Vec<D,T> &w) \
 { \
	using namespace std; \
	Vec<D,T> result(VEC_UNINITIALIZED); \
	for (int i = 0; i < D; i++) \
		result[i] = name(v[i], w[i]); \
	return result; \
 }

#define VEC_DECLARE_THREEARG(name) \
 template <int D, class T> \
 static inline Vec<D,T> name(const Vec<D,T> &v, const T &w, const T &x) \
 { \
	using namespace std; \
	Vec<D,T> result(VEC_UNINITIALIZED); \
	for (int i = 0; i < D; i++) \
		result[i] = name(v[i], w, x); \
	return result; \
 } \
 template <int D, class T> \
 static inline Vec<D,T> name(const Vec<D,T> &v, const Vec<D,T> &w, const Vec<D,T> &x) \
 { \
	using namespace std; \
	Vec<D,T> result(VEC_UNINITIALIZED); \
	for (int i = 0; i < D; i++) \
		result[i] = name(v[i], w[i], x[i]); \
	return result; \
 }

VEC_DECLARE_ONEARG(fabs)
VEC_DECLARE_ONEARG(floor)
VEC_DECLARE_ONEARG(ceil)
VEC_DECLARE_ONEARG(round)
VEC_DECLARE_ONEARG(trunc)
VEC_DECLARE_ONEARG(sin)
VEC_DECLARE_ONEARG(asin)
VEC_DECLARE_ONEARG(sinh)
VEC_DECLARE_ONEARG(cos)
VEC_DECLARE_ONEARG(acos)
VEC_DECLARE_ONEARG(cosh)
VEC_DECLARE_ONEARG(tan)
VEC_DECLARE_ONEARG(atan)
VEC_DECLARE_ONEARG(tanh)
VEC_DECLARE_ONEARG(exp)
VEC_DECLARE_ONEARG(log)
VEC_DECLARE_ONEARG(sqrt)
VEC_DECLARE_ONEARG(sqr)
VEC_DECLARE_ONEARG(cbrt)
VEC_DECLARE_ONEARG(cube)
VEC_DECLARE_ONEARG(sgn)
VEC_DECLARE_TWOARG(atan2)
VEC_DECLARE_TWOARG(pow)
VEC_DECLARE_TWOARG(fmod)
VEC_DECLARE_TWOARG(step)
namespace std {
 VEC_DECLARE_TWOARG(min)
 VEC_DECLARE_TWOARG(max)
}
VEC_DECLARE_THREEARG(smoothstep)
VEC_DECLARE_THREEARG(clamp)

#undef VEC_DECLARE_ONEARG
#undef VEC_DECLARE_TWOARG
#undef VEC_DECLARE_THREEARG


// Inject into std namespace
namespace std {
	using ::fabs;
	using ::floor;
	using ::ceil;
	using ::round;
	using ::trunc;
	using ::sin;
	using ::asin;
	using ::sinh;
	using ::cos;
	using ::acos;
	using ::cosh;
	using ::tan;
	using ::atan;
	using ::tanh;
	using ::exp;
	using ::log;
	using ::sqrt;
	using ::cbrt;
	using ::atan2;
	using ::pow;
	using ::fmod;
}


// Both valarrays and GLSL use abs() on a vector to mean fabs().
// Let's do the same...
template <int D, class T>
static inline Vec<D,T> abs(const Vec<D,T> &v)
{
	return fabs(v);
}

#endif
