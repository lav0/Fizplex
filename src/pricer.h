#include "simplex.h"

class Pricer {
public:
  Simplex::Candidate price(const DVector &x, const LP &lp, const DVector &d,
                           const std::vector<size_t> &non_basic_indices) const;
};
