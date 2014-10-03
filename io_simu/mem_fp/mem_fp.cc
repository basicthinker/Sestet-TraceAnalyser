// mem_fp.cc
// Sestet-TraceAnalyser
//
// Jinglei Ren <jinglei@ren.systems>
// Sept. 30, 2014

#include <iostream>
#include <cstdlib>
#include "max_fp.h"
#include "ada_fp.h"
#include "ext4_fp.h"
#include "../simulator.h"

using namespace std;

#define KB (1024)
#define MB (1024 * KB)
#define GB (1024 * MB)

#define PAGE_SIZE (4 * KB)
#define MEM_SIZE (GB)

#define MINUTE (60)

int main(int argc, const char* argv[]) {
  if (argc != 5) {
    cout << "Usage: " << argv[0] << " [INPUT FILE] "
        "[MIN STALE] [THRESHOLD] [HISTORY LEN]" << endl;
    return -EINVAL;
  }

  const char *in_file = argv[1];
  const int min_stale = atoi(argv[2]);
  const double threshold = atof(argv[3]);
  const int len = atoi(argv[4]);

  Ext4Simulator ext4_simu(in_file);
  AdaSimulator ada_simu(in_file);

  MaxFootprint max_fp;
  AdaptiveFootprint ada_fp(len, threshold, min_stale, ada_simu.engine());
  Ext4Footprint ext4_fp;

  ext4_simu.engine().Register(max_fp);
  ext4_simu.engine().Register(ext4_fp);
  ada_simu.engine().Register(ada_fp);

  ext4_simu.Run();
  ada_simu.Run();

  double max_rate = (double)max_fp.GetSize() * PAGE_SIZE / MB /
      (max_fp.duration() / MINUTE);
  double ada_avg = ada_fp.GetAverage() * PAGE_SIZE / KB;
  double ext4_avg = ext4_fp.GetAverage() * PAGE_SIZE / KB;
  cout << in_file << '\t' << max_fp.duration() << '\t' << max_rate << '\t'
      << ext4_fp.num_intervals() << '\t' << ext4_avg << '\t'
      << ada_fp.num_trans() << '\t' << ada_avg << endl;

  return 0;
}

