/***
 * Bitwuzla: Satisfiability Modulo Theories (SMT) solver.
 *
 * Copyright (C) 2022 by the authors listed in the AUTHORS file at
 * https://github.com/bitwuzla/bitwuzla/blob/main/AUTHORS
 *
 * This file is part of Bitwuzla under the MIT license. See COPYING for more
 * information at https://github.com/bitwuzla/bitwuzla/blob/main/COPYING
 */

#ifndef BZLA_SAT_PAINLESS_H_INCLUDED
#define BZLA_SAT_PAINLESS_H_INCLUDED

/*------------------------------------------------------------------------*/
#ifdef BZLA_USE_PAINLESS
/*------------------------------------------------------------------------*/

#include <painless.hpp>

#include <memory>

#include "sat/sat_solver.h"

namespace bzla::sat {

class Painless : public SatSolver
{
 public:
  Painless();
  ~Painless();

  void add(int32_t lit) override;
  void assume(int32_t lit) override;
  int32_t value(int32_t lit) override;
  bool failed(int32_t lit) override;
  int32_t fixed(int32_t lit) override;
  Result solve() override;
  void configure_terminator(Terminator *terminator) override;
  const char *get_name() const override { return "Painless"; }
  const char *get_version() const override;
};

}  // namespace bzla::sat

/*------------------------------------------------------------------------*/
#endif
/*------------------------------------------------------------------------*/

#endif
