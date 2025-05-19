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
  static simpleClause clause;
  if(lit){
    clause.push_back(lit);
    if(lit > numVars) numVars = lit;
  }
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
  // Init timeout detection before starting the solvers and sharers
	std::unique_lock<std::mutex> lock(mutexGlobalEnd);
	// to make sure that the broadcast is done when main has done its wait

	working = new PortfolioSimple();

	// Launch working
	std::vector<int> cube;

	std::thread mainWorker(&PortfolioSimple::solve_internal, (PortfolioSimple*)working, std::ref(cube), std::ref(initClause), numVars);

	int wakeupRet = 0;

	if (__globalParameters__.timeout > 0) {
		auto startTime = SystemResourceMonitor::getRelativeTimeSeconds();

		// Wait until end or __globalParameters__.timeout
		while ((unsigned int)SystemResourceMonitor::getRelativeTimeSeconds() < __globalParameters__.timeout &&
			   globalEnding == false) // to manage the spurious wake ups
		{
			auto remainingTime = std::chrono::duration<double>(
				__globalParameters__.timeout - (SystemResourceMonitor::getRelativeTimeSeconds() - startTime));
			auto wakeupStatus = condGlobalEnd.wait_for(lock, remainingTime);

			LOGDEBUG2("main wakeupRet = %s , globalEnding = %d ",
					  (wakeupStatus == std::cv_status::timeout ? "timeout" : "notimeout"),
					  globalEnding.load());
		}

		condGlobalEnd.notify_all();
		lock.unlock();

		if ((unsigned int)SystemResourceMonitor::getRelativeTimeSeconds() >= __globalParameters__.timeout &&
			finalResult ==
				SatResult::UNKNOWN) // if __globalParameters__.timeout set globalEnding otherwise a solver woke me up
		{
			globalEnding = true;
			finalResult = SatResult::TIMEOUT;
		}
	} else {
		// no __globalParameters__.timeout waiting
		while (globalEnding == false) // to manage the spurious wake ups
		{
			condGlobalEnd.wait(lock);
		}

		condGlobalEnd.notify_all();
		lock.unlock();
	}

	mainWorker.join();

	delete working;

	if (mpi_rank == mpi_winner) {
		if (finalResult == SatResult::SAT) {
			logSolution("SATISFIABLE");

			if (__globalParameters__.noModel == false) {
				logModel(finalModel);
			}
		} else if (finalResult == SatResult::UNSAT) {
			logSolution("UNSATISFIABLE");
		} else // if __globalParameters__.timeout or unknown
		{
			logSolution("UNKNOWN");
			finalResult = SatResult::UNKNOWN;
		}

		LOGSTAT("Resolution time: %f s", SystemResourceMonitor::getRelativeTimeSeconds());
	} else finalResult = SatResult::UNKNOWN; /* mpi will be forced to suspend job only by the winner */
  if (finalResult == SatResult::SAT) return Result::SAT;
  if (finalResult == SatResult::UNSAT) return Result::UNSAT;
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
  return "v1.24.10-dev";
}

/*------------------------------------------------------------------------*/

}  // namespace bzla::sat

/*------------------------------------------------------------------------*/
#endif
/*------------------------------------------------------------------------*/
