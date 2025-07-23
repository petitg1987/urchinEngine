// Synopsis: Simple point class
//
// Authors: Martin Kutz <kutz@math.fu-berlin.de>,
//          Kaspar Fischer <kf@iaeth.ch>

#ifndef SEB_POINT_H
#define SEB_POINT_H

#include <vector>

#include "Seb_configure.h"

namespace SEB_NAMESPACE {

  template<typename Float>
  class Point
  // A simple class representing a d-dimensional point.
  {
  public: // types:
    typedef typename std::vector<Float>::const_iterator Const_iterator;
    typedef typename std::vector<Float>::iterator Iterator;

  public: // construction and destruction:

    Point()
    : c({0, 0, 0})
    {
    }

    Point(Float x, Float y, Float z)
    : c({x, y, z})
    {
    }

  public: // access:

    const Float& operator[](unsigned int i) const
    // Returns a const-reference to the i-th coordinate.
    {
      SEB_ASSERT(0 <= i && i < c.size());
      return c[i];
    }

    Float& operator[](unsigned int i)
    // Returns a reference to the i-th coordinate.
    {
      SEB_ASSERT(0 <= i && i < c.size());
      return c[i];
    }

    Const_iterator begin() const
    // Returns a const-iterator to the first of the d Cartesian coordinates.
    {
      return c.begin();
    }

    Const_iterator end() const
    // Returns the past-the-end iterator corresponding to begin().
    {
      return c.end();
    }

  private: // member fields:
    std::array<Float, 3> c;       // Cartesian center coordinates
  };

} // namespace SEB_NAMESPACE

#endif // SEB_POINT_H
