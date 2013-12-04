CC=gcc
DEPS=ada_policy_util.h

all: simu_int_pred.out last_int_pred.out

%.out: %.c $(DEPS)
	$(CC) $< -o $@

clean:
	rm -rf *.out
