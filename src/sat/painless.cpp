/***
 * Bitwuzla: Satisfiability Modulo Theories (SMT) solver.
 *
 * Copyright (C) 2022 by the authors listed in the AUTHORS file at
 * https://github.com/bitwuzla/bitwuzla/blob/main/AUTHORS
 *
 * This file is part of Bitwuzla under the MIT license. See COPYING for more
 * information at https://github.com/bitwuzla/bitwuzla/blob/main/COPYING
 */

/*------------------------------------------------------------------------*/
#ifdef BZLA_USE_PAINLESS
/*------------------------------------------------------------------------*/

#include "sat/painless.h"

#include <cassert>

#include "util/exceptions.h"

/*------------------------------------------------------------------------*/

namespace bzla::sat {

Painless::Painless()
{
  
}

Painless::~Painless() {}

void
Painless::add(int32_t lit)
{
  static SimpleClause clause;
  if(lit)
    clause.push_back(lit);
  else{
    //end of clause
    initClause.push_back(clause); // register
    clause.clear();  // empty clause
  }
}

void
Painless::assume(int32_t lit)
{
  (void) lit;
  throw Error("Incremental solving not supported in Painless");
}

int32_t
Painless::value(int32_t lit)
{
  int32_t val = finalModel[abs(lit) -1];
  if(lit < 0) val = -val;
  if (val > 0) return 1;
  if (val < 0) return -1;
  return 0;
}

bool
Painless::failed(int32_t lit)
{
  (void) lit;
  throw Error("Incremental solving not supported in Painless");
  return false;
}

int32_t
Painless::fixed(int32_t lit)
{
  (void) lit;
  throw Error("fixed() not supported in Painless");
  return false;
}

Result
Painless::solve()
{
  SatResult res = painless_solve(d_solver);
  if (res == SatResult::SAT) return Result::SAT;
  if (res == SatResult::UNSAT) return Result::UNSAT;
  return Result::UNKNOWN;
}

void
Painless::configure_terminator(Terminator* terminator)
{
  if (terminator)
  {
    throw Unsupported("terminator not supported in Painless");
  }
}

const char *
Painless::get_version() const
{
  return painless_version();
}

/*------------------------------------------------------------------------*/

}  // namespace bzla::sat

/*------------------------------------------------------------------------*/
#endif
/*------------------------------------------------------------------------*/
