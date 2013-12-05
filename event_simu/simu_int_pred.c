#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include "ada_policy_util.h"

#define likely(x)       __builtin_expect((x),1)
#define unlikely(x)     __builtin_expect((x),0)

#define LEN_BITS 2
#define TIME_WAIT (~0UL)

#define INIT_SHORT 0.2
#define INIT_LONG 3.0
double THRE_MULTI = 3;

enum state {
  ST_SHORT_INT = 0,
  ST_LONG_INT = 1,
  ST_SHORTER,
  ST_LONGER
};

char *state_name[] = {
    [ST_SHORT_INT] = "ST_SHORT_INT",
    [ST_LONG_INT] = "ST_LONG_INT",
    [ST_SHORTER] = "ST_SHORTER",
    [ST_LONGER] = "ST_LONGER" };

enum event {
  EV_USER,
  EV_TIMER // either threshold or predicted value passed
};

char *event_name[] = {
    [EV_USER] = "EV_USER",
    [EV_TIMER] = "EV_TIMER" };

#define min_time(a, b) ((a) < (b) ? (a) : (b))

static inline void print_event(enum event ev, double past, enum state s) {
  fprintf(stderr, "%s on %s=%f\n", state_name[s], event_name[ev], past);
}

static inline void print_state(enum state ns, double timer) {
  fprintf(stderr, "=> %s timer=%f\n", state_name[ns], timer);
}

#define hist_avg(ts) ({ \
    struct adafs_interval_history *fh = &(ts)->int_hist; \
    fh_state(fh) / fh_len(fh); })

#define threshold(ts_vec) (hist_avg((ts_vec) + ST_SHORT_INT) * THRE_MULTI)

static inline void update_hist(struct adafs_touch_state *ts, double past_int) { 
    fh_update_interval(&ts->int_hist, past_int);
    ts->timer = hist_avg(ts);
}

int num_conflicts = 0;
int num_pred=0;
double last_pred = 0.0;
double total_len = 0.0;

static inline double try_pred(struct adafs_touch_state ts_vec[]) {
  double pred = ts_vec[ST_LONG_INT].timer;
  if (last_pred <= 0) {
    ++num_pred;
    total_len += pred;
    last_pred = pred;
    fprintf(stderr, "\tFlush begins %f\n", pred);
  } else {
    fprintf(stderr, "\tFlush remains %f\n", last_pred);
  }
  return pred;
}

static inline void check_conflict() {
  if (last_pred > 0) {
    ++num_conflicts;
    fprintf(stderr, "\tFlush remains %f\n", last_pred);
  }
}

// When event is from timer, @past_int is the last timer value;
// when event is from user, @past_int is the last remaining interval excluding timer values.
// Returns timer value
double transfer(struct adafs_touch_state ts_vec[],
    enum state *s, enum event ev, double past_int) {
  switch (*s) {
  case ST_SHORT_INT:
    if (ev == EV_USER) {
      check_conflict();
      update_hist(&ts_vec[ST_SHORT_INT], past_int);
      return threshold(ts_vec);
    } else if (ev == EV_TIMER) {
      *s = ST_LONG_INT;
      return try_pred(ts_vec);
    }
    break;
  case ST_LONG_INT:
    if (ev == EV_USER) {
      check_conflict();
      if (past_int <= threshold(ts_vec)) {
        *s = ST_SHORT_INT;
        update_hist(&ts_vec[ST_SHORT_INT], past_int);
        return threshold(ts_vec);
      } else {
        *s = ST_SHORTER;
        update_hist(&ts_vec[ST_LONG_INT], past_int);
        return threshold(ts_vec);
      }
    } else if (ev == EV_TIMER) {
      *s = ST_LONGER;
      return TIME_WAIT;
    }
    break;
  case ST_SHORTER:
    if (ev == EV_USER) {
      check_conflict();
      *s = ST_SHORT_INT;
      update_hist(&ts_vec[ST_SHORT_INT], past_int);
      return threshold(ts_vec);
    } else if (ev == EV_TIMER) {
      *s = ST_LONG_INT;
      return try_pred(ts_vec);
    }
    break;
  case ST_LONGER:
    if (ev == EV_USER) {
      check_conflict();
      *s = ST_SHORT_INT;
      update_hist(&ts_vec[ST_LONG_INT], past_int);
      return threshold(ts_vec);
    } else fprintf(stderr, "Invalid event %d on state %d\n", ev, *s);
    break;
  }
  return -EINVAL;
}

int main(int argc, char *argv[]) {
  struct adafs_touch_state ts_vec[2] = { ADAFS_TOUCH_STATE_INIT(LEN_BITS), ADAFS_TOUCH_STATE_INIT(LEN_BITS) };
  enum state s;
  double past_int;
  double timer;

  if (argc != 3) {
    fprintf(stderr, "Usage: %s [EVENT TRACE] [MULTIPLE]\n",
        argv[0]);
    return -1;
  }
  
  freopen(argv[1], "r", stdin);
  THRE_MULTI = atof(argv[2]);

  update_hist(&ts_vec[ST_SHORT_INT], INIT_SHORT);
  update_hist(&ts_vec[ST_LONG_INT], INIT_LONG);

  s = ST_SHORT_INT;
  timer = threshold(ts_vec);
  while (scanf("%lf", &past_int) == 1) {
    double rest_int = past_int;
    fprintf(stderr, "%f\n", past_int);
    while (rest_int > timer) {
      print_event(EV_TIMER, timer, s);
      rest_int -= timer;
      last_pred -= timer;
      timer = transfer(ts_vec, &s, EV_TIMER, timer);
      if (timer < 0) return -EINVAL;
      print_state(s, timer);
    }
    print_event(EV_USER, past_int, s);
    last_pred -= rest_int;
    timer = transfer(ts_vec, &s, EV_USER, rest_int);
    if (timer < 0) return -EINVAL;
    print_state(s, timer);
  }
  printf("%s:\t%d\t%d\t%f\t%f\t%f\n", argv[1],
      num_conflicts, num_pred, (double)num_conflicts / num_pred,
      total_len, total_len/num_pred);
  return 0;
}

