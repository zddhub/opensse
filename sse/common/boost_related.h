#ifndef BOOST_RELATED_H
#define BOOST_RELATED_H

#include <boost/shared_ptr.hpp>
#include <boost/make_shared.hpp>
#include <boost/function.hpp>
#include <boost/bind.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>

#include "distance.h"

namespace sse {
using boost::property_tree::ptree;

typedef boost::property_tree::ptree PropertyTree_t;

// Returns the value that is stored in the property_tree under path.
// If path does not exist, the default value is returned.
template<class T>
inline T parse(const PropertyTree_t &p, const std::string &path, const T &defaultValue)
{
    T value = p.get(path, defaultValue);
    return value;
}

/**
 * @brief 'Base' template factory class for generating a distance function
 * by name, we typically use the partially specialized version for vector<T>, though.
 */
template <class T, class R = float>
struct Distance_functions
{
    typedef boost::function<R (const T&, const T&)> distfn_t;

    distfn_t make(const std::string& /*name*/)
    {
        return distfn_t();
    }
};

/**
 * @brief Factory class that generates a distance function from its name.
 *
 * Note that this is a partial specialization of distance_functions
 * for data of type vector<T>. This makes constructing our typical
 * distance function that actually work on a vector<float> just
 * a little bit more convenient than if we would use the more general version.
 */
template <class X, class R>
struct Distance_functions<std::vector<X>, R>
{
    typedef std::vector<X> T;
    typedef boost::function<R (const T&, const T&)> distfn_t;

    distfn_t make(const std::string& name)
    {
        if (name == "l1norm") return L1norm<T>();
        if (name == "l2norm") return L2norm<T>();
        if (name == "l2norm_squared") return L2norm_squared<T>();
        if (name == "jsd") return Jsd<T>();
        if (name == "chi2") return Chi2<T>();
        if (name == "one_minus_dot") return One_minus_dot<T>();
        if (name == "df") return Dist_df<T>();
        if (name == "frobenius") return Dist_frobenius<T>();

        return distfn_t();
    }
};

} // namespace

#endif

