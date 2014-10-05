CC=g++
CFLAGS=-std=c++0x

all: passive_curves fsyncs ada_curves

passive_curves: passive_curves.cc simulator.h o_curve.h
	$(CC) $(CFLAGS) $@.cc -o $@.out

fsyncs: fsyncs.cc simulator.h simu_state.h data_item.h
	$(CC) $(CFLAGS) $@.cc -o $@.out

ada_curves: ada_curves.cc simulator.h ada_curves.h
	$(CC) $(CFLAGS) $@.cc -o $@.out

clean:
	rm -rf *.out
