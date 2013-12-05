#!/usr/bin/python

import sys
import string

def calc_avg(data_set, apps, method, multi):
    ratio_sum = 0.0
    len_sum = 0.0
    for app in apps:
        stat = data_set[app][method][multi]
        ratio_sum += stat["conflicts"] / stat["preds"]
        len_sum += stat["length"] / stat["preds"]
    return [ratio_sum / len(apps), len_sum / len(apps)]

# Main
if len(sys.argv) != 2:
    print "Usage: %s [SIMU OUTPUT]" % sys.argv[0]
    sys.exit(-1)

simu_file = open(sys.argv[1], 'r')

apps = set()
max_multi = 0

data_set = {}
for line in simu_file:
    words = line.split('\t')
    app = words[0].split('/')[-1].split('-')[0]
    method = words[1]
    multi = int(float(words[2]))
    conflicts = float(words[3])
    preds = float(words[4])
    length = float(words[6])
    
    apps.add(app)
    if multi > max_multi:
        max_multi = multi

    if not data_set.has_key(app):
        data_set[app] = { }
    if not data_set[app].has_key(method):
        data_set[app][method] = { }
    if not data_set[app][method].has_key(multi):
        data_set[app][method][multi] = {
            "conflicts": conflicts,
            "preds": preds,
            "length": length,
        }
    else:
        data_set[app][method][multi]["conflicts"] += conflicts
        data_set[app][method][multi]["preds"] += preds
        data_set[app][method][multi]["length"] += length

# Calculate average for each method
for method in ("FSM-AVG", "FSM-MIN", "FSM-MIX"):
    print method
    for i in range(1, max_multi + 1):
        ret = calc_avg(data_set, apps, method, i)
        print "\t%d:\t%f\t%f" % (i, ret[0], ret[1])

ret = calc_avg(data_set, apps, "LAST", 0)
print "LAST-MIN\t%f\t%f" % (ret[0], ret[1])
ret = calc_avg(data_set, apps, "LAST", 1)
print "LAST-AVG\t%f\t%f" % (ret[0], ret[1])

