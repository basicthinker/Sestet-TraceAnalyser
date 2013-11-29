// curves.cc
// Sestet-TraceAnalyser
//
// Jinglei Ren <jinglei.ren@stanzax.org>
// Nov. 28, 2013

#include <cerrno>
#include <list>
#include <iostream>
#include <fstream>
#include "simulator.h"
#include "o_curve.h"

using namespace std;

int main(int argc, const char *argv[]) {
  if (argc != 3) {
    cout << "Usage: " << argv[0] << " [INPUT FILE] [OUTPUT FILE]" << endl;
    return -EINVAL;
  }

  const char *in_file = argv[1];
  const char *out_file = argv[2];
  ofstream out_stream(out_file);

  Ext4Simulator ext4simu(in_file);
  AdaSimulator ada_simu(in_file);
  OCurve ext4curve, idle_curve;

  ext4simu.engine().Register(ext4curve);
  ada_simu.engine().Register(idle_curve);

  ext4simu.Run();
  ada_simu.Run();

  list<OPoint>::const_iterator ei = ext4curve.points().begin();
  list<OPoint>::const_iterator ii = idle_curve.points().begin();
  for (; ei != ext4curve.points().end() && ii != idle_curve.points().end();
      ++ei, ++ii) {
    if (ei->p_time != ii->p_time || ei->p_stale != ii->p_stale) {
      cerr << "Error: mismatch of time and staleness." << endl;
      return -EPROTO;
    }
    out_stream << ei->p_time << "\t" << ei->p_stale << "\t"
        << ei->p_oratio << "\t" << ii->p_oratio << endl;
  }

  if (ei != ext4curve.points().end() || ii != idle_curve.points().end()) {
    cerr << "Error: mismatch of point numbers: ext4 curve = "
        << ext4curve.points().size() << ", idle curve = "
        << idle_curve.points().size() << endl;
  }

  out_stream.close();
  return 0;
}

