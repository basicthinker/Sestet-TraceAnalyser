CC=gcc
DEPS=ada_policy_util.h

all: fsm_avg_int_pred.out fsm_min_int_pred.out fsm_mix_int_pred.out last_int_pred.out

%.out: %.c $(DEPS)
	$(CC) $< -o $@

clean:
	rm -rf *.out
