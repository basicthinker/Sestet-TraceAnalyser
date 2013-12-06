#!/usr/bin/python

import sys
import string

# Main
if len(sys.argv) != 4:
    print "Usage: %s [SIMU OUTPUT] [FSM METHOD] [MULTI]" % sys.argv[0]
    sys.exit(-1)

simu_file = open(sys.argv[1], 'r')
arg_method = sys.argv[2]
arg_multi = int(sys.argv[3])

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

for app in apps:
    min_stat = data_set[app]["LAST"][0]
    avg_stat = data_set[app]["LAST"][1]
    fsm_stat = data_set[app][arg_method][arg_multi]
    output = (
        app,
        min_stat["conflicts"] / min_stat["preds"],
        min_stat["length"] / min_stat["preds"],
        avg_stat["conflicts"] / avg_stat["preds"],
        avg_stat["length"] / avg_stat["preds"],
        fsm_stat["conflicts"] / fsm_stat["preds"],
        fsm_stat["length"] / fsm_stat["preds"],
    )
    print "%s\t%f\t%f\t%f\t%f\t%f\t%f" % output

