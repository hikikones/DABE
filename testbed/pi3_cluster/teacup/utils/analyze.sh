#!/bin/sh

# ############################################
# ## NOTE: Must run inside experiment folder
# ############################################

# Cleanup local
rm experiments_completed.txt experiments_started.txt teacup_dir_cache.txt teacup_flow_cache.txt
rm -rf result

# Cleanup UiS Unix host
ssh danerik@ssh1.ux.uis.no rm -rf /home/stud/danerik/teacup/experiment

# Get new TEACUP results
ssh danerik@ssh1.ux.uis.no scp -r root@pi3cluster.ux.uis.no:/home/danny/teacup/experiment /home/stud/danerik/teacup/
scp -r danerik@ssh1.ux.uis.no:/home/stud/danerik/teacup/experiment/* ./

# Analyze results
fab analyse_all:test_id=result,lnames='Receiver;Sender',plot_params="TC_TITLE\='TCP NewReno Sawtooth'"
