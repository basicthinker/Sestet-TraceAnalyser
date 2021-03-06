// mem_fp.cc
// Sestet-TraceAnalyser
//
// Jinglei Ren <jinglei@ren.systems>
// Sept. 30, 2014

#include <iostream>
#include <cstdlib>
#include <cstring>
#include "vfs_max.h"
#include "ada_max.h"
#include "../simu_engine.h"
#include "../simulator.h"

using namespace std;

#define PAGE_KB (4)

int main(int argc, const char* argv[]) {
  if (argc != 6) {
    cout << "Usage: " << argv[0] << " [INPUT FILE] "
        "[MIN STALE] [MAX STALE] [THRESHOLD] [HISTORY LEN]" << endl;
    return -EINVAL;
  }

  const char *in_file = argv[1];
  const int min_stale = atoi(argv[2]);
  const int max_stale = atoi(argv[3]);
  const double threshold = atof(argv[4]);
  const int len = atoi(argv[5]);

  Simulator simu(in_file);
  LazyEngine lazy;
  simu.Register(lazy);

  VFSMaxFP vfs_max;
  AdaMaxFP ada_max(len, threshold, min_stale, max_stale);
  lazy.Register(vfs_max).Register(ada_max);

  double duration = simu.Run();

  const char *file_name = in_file + strlen(in_file);
  while (*file_name != '/' && file_name >= in_file) {
    --file_name;
  }
  ++file_name;

  double vfs_rate = 2 * vfs_max.GetAverage() * PAGE_KB / duration;
  double ada_rate = 2 * ada_max.GetAverage() * PAGE_KB / duration;
  cout << file_name << '\t' << duration << '\t' << vfs_rate << '\t'
      << ada_max.num_trans() << '\t' << ada_rate << endl;

  return 0;
}

