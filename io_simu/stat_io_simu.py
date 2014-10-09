#!/usr/bin/python

import sys
import os
import math

def parse_trace_file(path):
    lines = [line.strip().split('\t') for line in open(path)]
    end_time = float(lines[-1][0])
    for line in reversed(lines):
        time = float(line[0])
        if end_time - time > 10:
            stale = float(line[1])
            ext4 = 1 - float(line[2]) / 100
            ideal = 1 - float(line[3]) / 100
            #print "%s\t%f\t%f\t%f\t%f" % (path, time, stale, ideal, ext4)
            return (time, stale, ideal, ext4)

def parse_ada_file(path):
    lines = [line.strip().split('\t') for line in open(path)]
    end_time = float(lines[-1][0])
    for line in reversed(lines):
        time = float(line[0])
        if end_time - time > 10:
            stale = float(line[1])
            adafs = 1 - float(line[2]) / 100
            return (time, stale, adafs)


# Main

if len(sys.argv) != 2:
    print "Usage: %s [TRACE DIR]" % sys.argv[0]
    sys.exit(-1)
dir_name = sys.argv[1]

data_set = {}

files = [f for f in os.listdir(dir_name) if os.path.splitext(f)[1] == ".trace"]
for filename in files:
    app = filename.split('-')[0]
    if filename.split('-')[1] != "io":
        continue
    trace_path = os.path.join(dir_name, filename) + ".txt"
    ada_path = os.path.join(dir_name, filename).replace(".trace", ".ada.txt")
    ext4_ret = parse_trace_file(trace_path)
    ada_ret = parse_ada_file(ada_path)
    if ext4_ret[0] != ada_ret[0] or ext4_ret[1] != ada_ret[1]:
        print "Error: mismatch of parsed items for %s" % filename
    stale = ext4_ret[1]
    ideal = ext4_ret[2]
    ext4 = ext4_ret[3]
    adafs = ada_ret[2]
    
    if not data_set.has_key(app):
        data_set[app] = {
            "count": 0,
            "ext4_ds": 0.0,
            "ada_ds": 0.0,
            "ext4_r": 1.0,
            "ada_r": 1.0
        }

    data_set[app]["count"] += 1
    data_set[app]["ext4_ds"] += stale * ext4
    data_set[app]["ada_ds"] += stale * adafs
    data_set[app]["ext4_r"] *= ext4 / ideal
    data_set[app]["ada_r"] *= adafs / ideal

for app in data_set.keys():
    n = data_set[app]["count"]
    ext4_r_mean = math.pow(data_set[app]["ext4_r"], 1.0/n)
    ada_r_mean = math.pow(data_set[app]["ada_r"], 1.0/n)
    ext4_ds_avg = data_set[app]["ext4_ds"] / n
    ada_ds_avg = data_set[app]["ada_ds"] / n
    print "%s\t%f\t%f\t%f\t%f" % (app, ext4_r_mean, ada_r_mean, ext4_ds_avg, ada_ds_avg)


