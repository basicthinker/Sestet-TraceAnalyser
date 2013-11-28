// io_num.cc
// Sestet-TraceAnalyser
//
// Jinglei Ren <jinglei.ren@stanzax.org>
// Nov. 25, 2013

#include "data_item.h"
#include "simulator.h"

class IONum : public SimuState {
  public:
    IONum() : num_read_hits_(0), num_read_misses_(0),
        num_write_hits_(0), num_write_misses_(0),
        num_flushed_(0), num_evicted_(0) {}

    int num_read_hits() const { return num_read_hits_; }
    int num_read_misses() const { return num_read_misses_; }
    int num_write_hits() const { return num_write_hits_; }
    int num_write_misses() const { return num_write_misses_; }
    int num_flushed() const { return num_flushed_; }
    int num_evicted() const { return num_evicted_; }

    void OnRead(const DataItem &item, bool hit) {
      if (hit) ++num_read_hits_;
      else ++num_read_misses_;
    }

    void OnWrite(const DataItem &item, bool hit) {
      if (hit) ++num_write_hits_;
      else ++num_write_misses_;
    }

    void OnFlush(const DataItem &item) {
      ++num_flushed_;
    }

    void OnEvict(const DataItem &item, bool hit) {
      if (hit) ++num_evicted_;
    }

    bool Check() {
      if (num_write_misses() != num_flushed() + num_evicted()) {
        std::cerr << "Mismatch: write misses = " << num_write_misses()
            << ", flushed = " << num_flushed() << ", evicted = "
            << num_evicted() << std::endl;
        return false;
      } else return true;
    }

  private:
    int num_read_hits_;
    int num_read_misses_;
    int num_write_hits_;
    int num_write_misses_;
    int num_flushed_;
    int num_evicted_;
};

// Main:

using namespace std;

int main(int argc, const char *argv[]) {
  if (argc != 2) {
    cout << "Usage: " << argv[0] << " [FILE]" << endl;
    return -EINVAL;
  }

  const char *file = argv[1];
  Ext4Simulator ext4simu(file);
  AdaSimulator ada_simu(file);
  IONum ext4num, ada_num;
  
  ext4simu.engine().Register(ext4num);
  ada_simu.engine().Register(ada_num);

  ext4simu.Run();
  ada_simu.Run();

  cout << file << "\t";
  cout << ext4num.num_read_hits() << "\t"
      << ext4num.num_read_misses() << "\t"
      << ext4num.num_write_hits() << "\t"
      << ext4num.num_write_misses() << "\t";
  cout << ada_num.num_read_hits() << "\t"
      << ada_num.num_read_misses() << "\t"
      << ada_num.num_write_hits() << "\t"
      << ada_num.num_write_misses() << endl;

  // Final check of integrity
  ext4simu.engine().Clear();
  ada_simu.engine().Clear();
  if (!ext4num.Check() || !ada_num.Check()) {
    cerr << "Warning: " << file << " final integrity check failed!" << endl;
  }
}

