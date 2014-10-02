// mem_fp.cc
// Sestet-TraceAnalyser
//
// Jinglei Ren <jinglei@ren.systems>
// Sept. 30, 2014

#include <iostream>
#include <cstdlib>
#include "mem_max.h"
#include "mem_ada.h"
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
  MemoryMax mem_max;
  MemoryAdaptive mem_ada(len, threshold, min_stale, ada_simu.engine());

  ext4_simu.engine().Register(mem_max);
  ada_simu.engine().Register(mem_ada);

  ext4_simu.Run();
  ada_simu.Run();

  double max_rate = (double)mem_max.GetSize() * PAGE_SIZE / MB /
      (mem_max.duration() / MINUTE);
  cout << in_file << '\t' << mem_max.duration() << '\t' << max_rate << '\t'
      << mem_ada.num_trans() << endl;
  return 0;
}

