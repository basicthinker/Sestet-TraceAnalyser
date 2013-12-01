CC=g++

all: curves fsyncs ada_curves

curves: curves.cc simulator.h o_curve.h
	$(CC) $^ -o $@.out

fsyncs: fsyncs.cc simulator.h simu_state.h data_item.h
	$(CC) $^ -o $@.out

ada_curves: ada_curves.cc simulator.h ada_curve.h
	$(CC) $^ -o $@.out

clean:
	rm -rf *.out
