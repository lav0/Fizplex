#ifndef BASE_H
#define BASE_H

#include "colmatrix.h"


template <int m>
class Base {
  public:
    Base();
    ColMatrix<m> base;
    void invert();
    void updateVec(SVector& vec);
  private:
    struct ETM {
      SVector eta;
      int col;
    };
    std::vector<ETM> etms;
    std::array<double,m> work;
    void updateUnfinishedEtas(int i);
    void updateVecWithETM(ETM& etm, SVector& vec);
};


template <int m>
Base<m>::Base() {
  work.fill(0.0);
};


template <int m>
void Base<m>::invert() {
//  etms.clear();
//  etms.reserve(m);
  for(int i = 0; i < m; i++)
    etms.push_back({base.column(i), i});
  double mult;
  for(int i = 0; i < m; i++) {
    for(auto& n : etms[i].eta) {
      if(n.index == i) {
        mult =  - 1.0 / n.value;
        n.value = -1;
//        std::cout << "Mult = " << mult << std::endl;
        break;
      }
    }
    for(auto& n : etms[i].eta) {
      n.value *= mult;
    }

    etms[i].col = i;

    // Update v_i
    updateUnfinishedEtas(i);

  }
  std::cout << "Eta:" << std::endl;
  for(auto j = 0; j < etms.size(); j++)
    std::cout << "i = " << j << " eta = " << etms[j].eta << std::endl;
};

template <int m>
void Base<m>::updateVecWithETM(ETM& etm, SVector& vec) {
  // Copy eta to work array TODO only do if found
  for(auto& entry : etm.eta)
    work[entry.index] = entry.value;

  double mult = 0.0;
  bool found = false;
  // Find nonzero multiplier in vec_i
  for(auto& v : vec) {
    if(v.index == etm.col) {
      found = true;
      mult = v.value;
      v.value = 0; // otherwise v_col = v_col + v_col * mult; should be v_col = v_col * mult
    }
  }
  if(found) { // if !found: v^i_finishedETM is zero, no update required
    for(auto& e : vec) {
      if(work[e.index] != 0.0) {
        e.value += work[e.index] * mult;
        work[e.index] = 0.0;
      }
    }
    for(auto& e : etm.eta) {
      if(work[e.index] != 0.0) { // not handled already => we have fill in
        vec.add_value(e.index, work[e.index] * mult);
        work[e.index] = 0.0;
//        std::cout << "Fill in!" << std::endl;
      }
    }

    // v_i = v_i + eta_i * v_i except for pivot element, there it's v_i = eta_i * v_i, i = posi
    work[etm.col] -= mult;
  }
}

template <int m>
void Base<m>::updateVec(SVector& vec) {
  for(const auto& etm : etms)
    updateVecWithETM(etm, vec);
}

template <int m>
void Base<m>::updateUnfinishedEtas(int finishedETM) {
  // Update v_i in that are saved in etas
  for(int i = finishedETM + 1; i < etms.size(); i++)
    updateVecWithETM(etms[finishedETM], etms[i].eta);

};

//std::ostream& operator<<(std::ostream& os, Vec const& vec) {
//  for(const auto& n : vec)
//    std::cout << n;
//  std::cout << std::endl;
//}

#endif
