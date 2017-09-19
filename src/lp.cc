#include "lp.h"
#include <cassert>

LP::LP() : A(), b(), c(), cols(), rows() {}

LP::Column::Column(ColType _type, double _lower, double _upper,
                   bool _is_logical)
    : type(_type), is_logical(_is_logical), lower(_lower), upper(_upper) {}

void LP::add_column(ColType _type, double _lower, double _upper,
                    bool _is_logical) {
  cols.push_back(Column(_type, _lower, _upper, _is_logical));
  A.add_column();
}

LP::Row::Row(RowType _type, double _lower, double _upper)
    : type(_type), lower(_lower), upper(_upper) {}

void LP::add_row(RowType _type, double _lower, double _upper) {
  rows.push_back(Row(_type, _lower, _upper));
  A.add_row();
}

size_t LP::column_count() const { return cols.size(); }

size_t LP::row_count() const { return rows.size(); }

void LP::add_value(size_t row, size_t column, double value) {
  assert(row < row_count());
  assert(column < column_count());

  A.add_value(row, column, value);
}

double LP::get_value(size_t row, size_t column) const {
  assert(row < row_count());
  assert(column < column_count());

  return A.get_value(row, column);
}

void LP::set_b() {
  b = DVector(row_count());
  for (size_t i = 0; i < row_count(); i++) {
    if (rows[i].type == RowType::GE)
      b[i] = rows[i].lower;
    else if (rows[i].type == RowType::NonBinding)
      b[i] = 0.0f;
    else
      b[i] = rows[i].upper;
  }
}

void LP::add_obj_value(size_t ind, double d) {
  assert(ind < c.dimension());
  c[ind] = d;
}

double LP::get_obj_value(size_t ind) const {
  assert(ind < c.dimension());
  return c[ind];
}

void LP::add_logicals() {
  const size_t n = column_count();
  for (size_t i = 0; i < row_count(); i++) {
    switch (rows[i].type) {
    case RowType::Equality:
      assert(is_finite(rows[i].upper));
      assert(is_finite(rows[i].lower));
      assert(is_eq(rows[i].upper, rows[i].lower));
      add_column(ColType::Fixed, 0.0, 0.0, true);
      break;
    case RowType::Range:
      assert(is_finite(rows[i].upper));
      assert(is_finite(rows[i].lower));
      add_column(ColType::Bounded, 0.0, rows[i].upper - rows[i].lower, true);
      break;
    case RowType::LE:
      assert(is_finite(rows[i].upper));
      add_column(ColType::LowerBound, 0.0, inf, true);
      break;
    case RowType::NonBinding:
      add_column(ColType::Free, -inf, inf, true);
      break;
    case RowType::GE:
      assert(is_finite(rows[i].lower));
      add_column(ColType::UpperBound, -inf, 0, true);
      break;
    default:
      break;
    }
    add_value(i, i + n, 1.0);
  }
  c = DVector(column_count());
}

const LP::Column &LP::column_header(size_t column) const {
  assert(column < cols.size());
  return cols[column];
}

bool LP::is_feasible(const DVector &x) const {
  assert(x.dimension() == column_count());
  for (size_t i = 0; i < cols.size(); i++) {
    if (cols[i].is_logical)
      continue;
    if ((is_finite(cols[i].lower) && is_lower(x[i], cols[i].lower)) ||
        (is_finite(cols[i].upper) && is_greater(x[i], cols[i].upper))) {
      return false;
    }
  }
  for (size_t row = 0; row < row_count(); row++) {
    double val = 0.0f;
    for (size_t col = 0; col < column_count(); col++) {
      val += A.get_value(row, col) * x[col];
    }
    if ((is_finite(rows[row].lower) && is_lower(val, rows[row].lower)) ||
        (is_finite(rows[row].upper) && is_greater(val, rows[row].upper))) {
      return false;
    }
  }
  return true;
}
