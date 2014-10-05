// curves.cc
// Sestet-TraceAnalyser
//
// Jinglei Ren <jinglei@ren.systems>
// Nov. 28, 2013

#include <cerrno>
#include <cassert>
#include <list>
#include <iostream>
#include <fstream>
#include "simulator.h"
#include "passive_curve.h"

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

  Simulator simu(in_file);

  LazyEngine lazy;
  VFSEngine vfs(5);

  PassiveCurve ideal_curve, ext4_curve;

  lazy.Register(&ideal_curve);
  vfs.Register(&ext4_curve);

  simu.Register(&lazy).Register(&vfs);

  simu.Run();

  list<OPoint>::const_iterator ii = ideal_curve.GetPoints().begin();
  list<OPoint>::const_iterator ei = ext4_curve.GetPoints().begin();
  for (; ei != ext4_curve.GetPoints().end() &&
      ii != ideal_curve.GetPoints().end(); ++ei, ++ii) {
    assert(ei->time() == ii->time() &&
        ei->stale_blocks() == ii->stale_blocks());
    double mbs = (double)ei->stale_blocks() * PAGE_SIZE / MB;
    out_stream << ei->time() << "\t" << mbs << "\t"
        << ei->percent() << "\t" << ii->percent() << endl;
  }
  assert(ei == ext4_curve.GetPoints().end() &&
      ii == ideal_curve.GetPoints().end());

  out_stream.close();
  return 0;
}

