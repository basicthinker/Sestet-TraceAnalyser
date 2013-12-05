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

static void init_pred() {
  int i;
  for (i = 0; i < NUM_TESTS; ++i) {
    num_confl[i] = 0;
    num_pred[i] = 0;
    last_pred[i] = 0;
    total_len[i] = 0;
  }
}

static void check_conflict(double past_int) {
  int i;
  for (i = 0; i < NUM_TESTS; ++i) {
    last_pred[i] -= past_int;
    if (last_pred[i] > 0) {
      fprintf(stderr, "\t[%d] flush remains %f\n", i, last_pred[i]);
      ++num_confl[i];
    } else {
      fprintf(stderr, "\t[%d] flush ends.\n", i);
    }
  }
}

static void try_pred(struct adafs_interval_history *int_hist) {
  if (last_pred[TEST_MIN] <= 0) {
    double min = 1024, *pos;
    for_each_history_limit(pos, int_hist, fh_len(int_hist)) {
      if (*pos < min) min = *pos;
    }
    last_pred[TEST_MIN] = min;
    fprintf(stderr, "\t[%d] flush begins %f\n", TEST_MIN, min);
    ++num_pred[TEST_MIN];
    total_len[TEST_MIN] += min;
  }

  if (last_pred[TEST_AVG] <= 0) {
    last_pred[TEST_AVG] = fh_state(int_hist) / fh_len(int_hist);
    fprintf(stderr, "\t[%d] flush begins %f\n", TEST_AVG, last_pred[TEST_AVG]);
    ++num_pred[TEST_AVG];
    total_len[TEST_AVG] += last_pred[TEST_AVG];
  }
}

static void print_stat(char *filename) {
  int i;
  for (i = 0; i < NUM_TESTS; ++i) {
    printf("%s\t%s\t%d\t%d\t%d\t%f\t%f\t%f\n", filename, "LAST", i,
        num_confl[i], num_pred[i], (double)num_confl[i] / num_pred[i],
        total_len[i], total_len[i]/num_pred[i]);
  }
}

int main(int argc, char *argv[]) {
  int i;
  struct adafs_interval_history int_hist = ADAFS_HISTORY_INIT(LEN_BITS, 0.0, 0.0);
  double past_int;
  if (argc != 2) {
    fprintf(stderr, "Usage: %s EventLog\n",
        argv[0]);
    return -1;
  }
  
  freopen(argv[1], "r", stdin);
  
  init_pred();
  fh_update_interval(&int_hist, THR_INT);
  try_pred(&int_hist);
  while (scanf("%lf", &past_int) == 1) {
    fprintf(stderr, "%f\n", past_int);
    check_conflict(past_int); // review
    fh_update_interval(&int_hist, past_int);
    try_pred(&int_hist);
 }
  print_stat(argv[1]);
  return 0;
}

