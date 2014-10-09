// ada_curves.cc
// Sestet-TraceAnalyser
//
// Jinglei Ren <jinglei@ren.systems>
// Nov. 29, 2013

#include <cerrno>
#include <cstdlib>
#include <cassert>
#include <list>
#include <iostream>
#include <fstream>
#include "simu_engine.h"
#include "simulator.h"
#include "ada_curves.h"

#define KB (1024) // bytes
#define MB (1024 * KB)
#define PAGE_SIZE (4 * KB) // kernel config

using namespace std;

int main(int argc, const char *argv[]) {
  if (argc != 7) {
    cout << "Usage: " << argv[0] << " [INPUT FILE] [OUTPUT FILE] "
        "[MIN STALE] [MAX STALE] [THRESHOLD] [HISTORY LEN]" << endl;
    return -EINVAL;
  }

  const char *in_file = argv[1];
  const char *out_file = argv[2];
  const int min_stale = atoi(argv[3]);
  const int max_stale = atoi(argv[4]);
  const double threshold = atof(argv[5]);
  const int len = atoi(argv[6]);
  ofstream out_stream(out_file);

  Simulator simu(in_file);
  LazyEngine engine;
  simu.Register(engine);

  AdaCurves curves(len, threshold, min_stale, max_stale);
  engine.Register(curves);

  double duration = simu.Run();

  list<OPoint>::const_iterator ai = curves.GetPoints().begin();
  list<OPoint>::const_iterator ti = curves.GetTranPoints().begin();
  for (; ai != curves.GetPoints().end() && ti != curves.GetTranPoints().end();
      ++ai, ++ti) {
    assert(ai->time() == ti->time() &&
        ai->stale_blocks() == ti->stale_blocks());
    double mbs = (double)ai->stale_blocks() * PAGE_SIZE / MB; 
    out_stream << ai->time() << "\t" << mbs << "\t"
        << ai->percent() << "\t" << ti->percent() << endl;
  }
  assert(ai == curves.GetPoints().end() &&
      ti == curves.GetTranPoints().end());

  // additional stats
  unsigned long kbs = curves.flushed_blocks() * PAGE_SIZE / KB;
  cout << in_file << '\t' << kbs / duration << endl;

  out_stream.close();
  return 0;
}

