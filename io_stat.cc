// io_stat.cc
// Sestet-TraceAnalyser
//
// Jinglei Ren <jinglei.ren@stanzax.org>
// Nov. 25, 2013

#include "data_item.h"
#include "simulator.h"

class IOStat : public SimuState {
  public:
    IOStat() : num_read_hits_(0), num_read_misses_(0), num_writes_(0) {}
    int num_read_hits() const { return num_read_hits_; }
    int num_read_misses() const { return num_read_misses_; }
    int num_writes() const { return num_writes_; }

    void OnRead(const DataItem &item, bool hit) {
      if (hit) ++num_read_hits_;
      else ++num_read_misses_;
    }

    void OnWrite(const DataItem &item, bool hit) {
      ++num_writes_;
    }

  private:
    int num_read_hits_;
    int num_read_misses_;
    int num_writes_;
}; 

// Main:

using namespace std;

int main(int argc, const char *argv[]) {
  if (argc < 2) {
    cout << "Usage: " << argv[0] << " [FILE]..." << endl;
    return -EINVAL;
  }

  for (int i = 1; i < argc; ++i) {
    Ext4Simulator ext4simu(argv[i]);
    AdaSimulator ada_simu(argv[i]);
    IOStat ext4stat, ada_stat;
    
    ext4simu.engine().Register(ext4stat);
    ada_simu.engine().Register(ada_stat);

    ext4simu.Run();
    ada_simu.Run();

    cout << argv[i] << "\t";
    cout << ext4stat.num_read_hits() << "\t"  << ext4stat.num_read_misses()
        << "\t" << ext4stat.num_writes() << "\t";
    cout << ada_stat.num_read_hits() << "\t"  << ada_stat.num_read_misses()
        << "\t" << ada_stat.num_writes() << endl;
  }
}

