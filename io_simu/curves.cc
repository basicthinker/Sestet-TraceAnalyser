// curves.cc
// Sestet-TraceAnalyser
//
// Jinglei Ren <jinglei@ren.systems>
// Nov. 28, 2013

#include <cerrno>
#include <list>
#include <iostream>
#include <fstream>
#include "simulator.h"
#include "o_curve.h"

#define PAGE_SIZE (4096) // kernel config
#define MB (1024 * 1024) // bytes

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
  OCurve ext4curve, ideal_curve;

  ext4simu.engine().Register(ext4curve);
  ada_simu.engine().Register(ideal_curve);

  ext4simu.Run();
  ada_simu.Run();

  list<OPoint>::const_iterator ei = ext4curve.points().begin();
  list<OPoint>::const_iterator ii = ideal_curve.points().begin();
  for (; ei != ext4curve.points().end() && ii != ideal_curve.points().end();
      ++ei, ++ii) {
    if (ei->time() != ii->time() || ei->stale_blocks() != ii->stale_blocks()) {
      cerr << "Error: mismatch of time and staleness." << endl;
      return -EPROTO;
    }
    double mbs = (double)ei->stale_blocks() * PAGE_SIZE / MB;
    out_stream << ei->time() << "\t" << mbs << "\t"
        << ei->percent() << "\t" << ii->percent() << endl;
  }

  if (ei != ext4curve.points().end() || ii != ideal_curve.points().end()) {
    cerr << "Error: mismatch of point numbers: ext4 curve = "
        << ext4curve.points().size() << ", ideal curve = "
        << ideal_curve.points().size() << endl;
  }

  out_stream.close();
  return 0;
}

