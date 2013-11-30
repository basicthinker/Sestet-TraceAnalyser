// ada_curve.cc
// Sestet-TraceAnalyser
//
// Jinglei Ren <jinglei.ren@stanzax.org>
// Nov. 29, 2013

#include <cerrno>
#include <cstdlib>
#include <list>
#include <iostream>
#include <fstream>
#include "simulator.h"
#include "ada_curve.h"

using namespace std;

int main(int argc, const char *argv[]) {
  if (argc != 5) {
    cout << "Usage: " << argv[0] << " [INPUT FILE] [OUTPUT FILE] "
        "[HISTORY LEN] [THRESHOLD]" << endl;
    return -EINVAL;
  }

  const char *in_file = argv[1];
  const char *out_file = argv[2];
  const int len = atoi(argv[3]);
  const double threshold = atof(argv[4]);
  ofstream out_stream(out_file);

  AdaSimulator ada_simu(in_file);
  AdaCurve ada_curve(len, threshold, ada_simu.engine());

  ada_simu.engine().Register(ada_curve);

  ada_simu.Run();

  list<OPoint>::const_iterator ai = ada_curve.points().begin();
  list<OPoint>::const_iterator ti = ada_curve.tran_points().begin();
  for (; ai != ada_curve.points().end() && ti != ada_curve.tran_points().end();
      ++ai, ++ti) {
    if (ai->p_time != ti->p_time || ai->p_stale != ti->p_stale) {
      cerr << "Error: Ada curves mismatch time and/or staleness." << endl;
      return -EPROTO;
    }
    out_stream << ai->p_time << "\t" << (double)ai->p_stale / 1024 << "\t"
        << ai->p_oratio * 100 << "\t" << ti->p_oratio * 100 << endl;
  }

  if (ai != ada_curve.points().end() || ti != ada_curve.tran_points().end()) {
    cerr << "Error: mismatch of point numbers: overwrite curve = "
        << ada_curve.points().size() << ", transaction curve = "
        << ada_curve.tran_points().size() << endl;
  }

  out_stream.close();
  return 0;
}

