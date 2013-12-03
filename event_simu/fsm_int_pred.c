#include <stdio.h>
#include <stdlib.h>
#include "ada_policy_util.h"

#define likely(x)       __builtin_expect((x),1)
#define unlikely(x)     __builtin_expect((x),0)

#define LEN_BITS 2
#define THR_INT 1.0

#define min_time(a, b) ((a) < (b) ? (a) : (b))

#define NUM_TESTS 2
#define TEST_MIN 0
#define TEST_AVG 1

int num_confl[NUM_TESTS];
int num_pred[NUM_TESTS];
double last_pred[NUM_TESTS];
double total_len[NUM_TESTS];

static void init_pred(double pred) {
  int i;
  for (i = 0; i < NUM_TESTS; ++i) {
    num_confl[i] = 0;
    num_pred[i] = 0;
    last_pred[i] = pred;
    total_len[i] = 0.0;
  }
}

static void check_conflict(double in) {
  int i;
  for (i = 0; i < NUM_TESTS; ++i) {
    if (in < last_pred[i]) {
      ++num_confl[i];
    }
    last_pred[i] -= in;
  }
}

static void try_pred(struct adafs_interval_history *int_hist, double in) {
  if (last_pred[TEST_MIN] <= 0) {
    double min = 1024, *pos;
    for_each_history(pos, int_hist) {
      if (*pos < min) min = *pos;
    }
    last_pred[TEST_MIN] = min;
    ++num_pred[TEST_MIN];
    total_len[TEST_MIN] += min;
  }

  if (last_pred[TEST_AVG] <= 0) {
    last_pred[TEST_AVG] = fh_state(int_hist) / fh_len(int_hist);
    ++num_pred[TEST_AVG];
    total_len[TEST_AVG] += last_pred[TEST_AVG];
  }
}

static void print_stat(char *filename) {
  int i;
  for (i = 0; i < NUM_TESTS; ++i) {
    printf("%s:\t%d\t%d\t%f\t%f\t%f\n", filename,
        num_confl[i], num_pred[i], (double)num_confl[i] / num_pred[i],
        total_len[i], total_len[i]/num_pred[i]);
  }
}

int main(int argc, char *argv[]) {
  struct adafs_interval_history int_hist = ADAFS_HISTORY_INIT(LEN_BITS, 0.0, 0.0);
  double next_int;
  if (argc != 2) {
    fprintf(stderr, "Usage: %s EventLog\n",
        argv[0]);
    return -1;
  }
  
  freopen(argv[1], "r", stdin);
  
  init_pred(THR_INT);
  while (scanf("%lf", &next_int) == 1) {
    fh_update_interval(&int_hist, next_int);
    check_conflict(next_int);
    try_pred(&int_hist, next_int);
  }
  print_stat(argv[1]);
  return 0;
}

