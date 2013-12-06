#!/usr/bin/python

import sys
import os
import math
import numpy

def parse_ada_file(path):
    int_list = []
    lines = [line.strip().split('\t') for line in open(path)]
    begin_time = float(lines[0][0])
    for line in lines:
        time = float(line[0])
        ratio = float(line[3])
        if ratio == 0 and time - begin_time > 0.01:
            int_list.append(time - begin_time)
            begin_time = time
    time = float(lines[-1][0])
    ratio = float(lines[-1][3])
    if time - begin_time > 0.01:
        int_list.append(time - begin_time)

    arr = numpy.array(int_list)
    return (int_list, numpy.mean(arr), numpy.std(arr))

def parse_fsync_file(path):
    int_list = []
    lines = [line.strip().split('\t') for line in open(path)]
    begin_time = 0
    for line in lines:
        time = float(line[0])
        int_list.append(time - begin_time)
        begin_time = time
    arr = numpy.array(int_list)
    return (int_list, numpy.mean(arr), numpy.std(arr))
    
# Main

if len(sys.argv) < 2:
    print "Usage: %s DIR [APP]..." % sys.argv[0]
    sys.exit(-1)
dir_name = sys.argv[1]

data_set = {}

files = [f for f in os.listdir(dir_name) if os.path.splitext(f)[1] == ".trace"]
for filename in files:
    app = filename.split('-')[0]
    ada_path = os.path.join(dir_name, filename).replace(".trace", ".ada.txt")
    fsync_path = os.path.join(dir_name, filename).replace(".trace", ".fsyncs") 
    ada_ret = parse_ada_file(ada_path)
    fsync_ret = parse_fsync_file(fsync_path)
    if not data_set.has_key(app):
        data_set[app] = { "ada": [], "fsync": [] }
    data_set[app]["ada"] += ada_ret[0]
    data_set[app]["fsync"] += fsync_ret[0]
    if app in sys.argv:
        print "%s\t%f\t%f\t%f\t%f" % (ada_path, fsync_ret[1], fsync_ret[2],
                ada_ret[1], ada_ret[2])

all_lists = { "ada": [], "fsync": [] }
for app in data_set.keys():
    all_lists["ada"] += data_set[app]["ada"]
    all_lists["fsync"] += data_set[app]["fsync"]
    ada_arr = numpy.array(data_set[app]["ada"])
    fsync_arr = numpy.array(data_set[app]["fsync"])
    print "%s\t%f\t%f\t%f\t%f" % (app,
            numpy.mean(fsync_arr), numpy.std(fsync_arr),
            numpy.mean(ada_arr), numpy.std(ada_arr))

ada_arr = numpy.array(all_lists["ada"])
fsync_arr = numpy.array(all_lists["fsync"])
print "%s\t%f\t%f\t%f\t%f" % ("All",
        numpy.mean(fsync_arr), numpy.std(fsync_arr),
        numpy.mean(ada_arr), numpy.std(ada_arr))

