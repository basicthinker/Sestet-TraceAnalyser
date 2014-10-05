// fsyncs.cc
// Sestet-TraceAnalyser
//
// Jinglei Ren <jinglei@ren.systems>
// Nov. 28, 2013

#include <cerrno>
#include <iostream>
#include <fstream>
#include "data_item.h"
#include "simu_state.h"
#include "simulator.h"

#define PAGE_SIZE 4096 // kernel config
#define MB (1024 * 1024) // bytes

class Fsyncs : public SimuState {
  public:
    Fsyncs(const char *file) : stale_(0) { output_.open(file); }
    ~Fsyncs() { output_.close(); }

    void OnWrite(const DataItem &item) {
      stale_ += 1;
    }

    void OnFsync(double time, unsigned long file) {
      output_ << time << "\t"
          << (double)stale_ * PAGE_SIZE / MB << "\t"
          << 50 << std::endl;
    }

  private:
    unsigned long stale_;
    std::ofstream output_;
};

using namespace std;

int main(int argc, const char *argv[]) {
  if (argc != 3) {
    cout << "Usage: " << argv[0] << " [INPUT FILE] [OUTPUT FILE]" << endl;
    return -EINVAL;
  }

  Simulator simu(argv[1]);
  VFSEngine engine(5);
  Fsyncs fsyncs(argv[2]);

  simu.Register(engine);
  engine.Register(fsyncs);

  simu.Run();
  return 0;
}

