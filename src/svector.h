#ifndef SVECTOR_H
#define SVECTOR_H

#include <cmath>
#include <iomanip>
#include <iostream>
#include <vector>

#include "numeric.h"

// Sparse Vector
class SVector {
public:
  SVector();
  // cppcheck-suppress noExplicitConstructor
  SVector(std::initializer_list<Nonzero>);

  void add_value(size_t ind, double val);
  size_t length() const;

  auto begin() { return values.begin(); }
  auto end() { return values.end(); }
  auto cbegin() const { return values.cbegin(); }
  auto cend() const { return values.cend(); }

  friend std::ostream &operator<<(std::ostream &os, SVector const &n);

  bool operator==(const SVector &rhs) const;
  bool operator!=(const SVector &rhs) const;
  double get_value(size_t ind) const;
  double &operator[](size_t);

private:
  std::vector<Nonzero> values;
};

std::ostream &operator<<(std::ostream &os, Nonzero const &n);
SVector operator*(const double c, const SVector &vec);

#endif
