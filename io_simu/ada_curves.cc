// ada_curves.cc
// Sestet-TraceAnalyser
//
// Jinglei Ren <jinglei@ren.systems>
// Nov. 29, 2013

#include <cerrno>
#include <cstdlib>
#include <list>
#include <iostream>
#include <fstream>
#include "simulator.h"
#include "ada_curve.h"

#define PAGE_SIZE 4096 // kernel config
#define MB (1024 * 1024) // bytes

using namespace std;

int main(int argc, const char *argv[]) {
  if (argc != 6) {
    cout << "Usage: " << argv[0] << " [INPUT FILE] [OUTPUT FILE] "
        "[MIN STALE] [THRESHOLD] [HISTORY LEN]" << endl;
    return -EINVAL;
  }

  const char *in_file = argv[1];
  const char *out_file = argv[2];
  const int min_stale = atoi(argv[3]);
  const double threshold = atof(argv[4]);
  const int len = atoi(argv[5]);
  ofstream out_stream(out_file);

  AdaSimulator ada_simu(in_file);
  AdaCurve ada_curve(len, threshold, min_stale, ada_simu.engine());

  ada_simu.engine().Register(ada_curve);

  ada_simu.Run();

  list<OPoint>::const_iterator ai = ada_curve.points().begin();
  list<OPoint>::const_iterator ti = ada_curve.tran_points().begin();
  for (; ai != ada_curve.points().end() && ti != ada_curve.tran_points().end();
      ++ai, ++ti) {
    if (ai->time() != ti->time() || ai->staleness_mb() != ti->staleness_mb()) {
      cerr << "Error: Ada curves mismatch time and/or staleness." << endl;
      return -EPROTO;
    }
    out_stream << ai->time() << "\t" << ai->staleness_mb() << "\t"
        << ai->percent() << "\t" << ti->percent() << endl;
  }

  if (ai != ada_curve.points().end() || ti != ada_curve.tran_points().end()) {
    cerr << "Error: mismatch of point numbers: overwrite curve = "
        << ada_curve.points().size() << ", transaction curve = "
        << ada_curve.tran_points().size() << endl;
  }

  out_stream.close();
  return 0;
}

