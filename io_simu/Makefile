CC=g++

all: curves fsyncs ada_curves

curves: curves.cc simulator.h o_curve.h
	$(CC) $@.cc -o $@.out

fsyncs: fsyncs.cc simulator.h simu_state.h data_item.h
	$(CC) $@.cc -o $@.out

ada_curves: ada_curves.cc simulator.h ada_curve.h
	$(CC) $@.cc -o $@.out

clean:
	rm -rf *.out
