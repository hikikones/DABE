#!/bin/sh

############################################
## NOTE: Must run inside experiment folder
############################################

# Delete old results
ssh danerik@ssh1.ux.uis.no ssh root@pi3cluster.ux.uis.no rm /home/danny/teacup/experiment/experiments_started.txt
ssh danerik@ssh1.ux.uis.no ssh root@pi3cluster.ux.uis.no rm /home/danny/teacup/experiment/experiments_completed.txt
ssh danerik@ssh1.ux.uis.no ssh root@pi3cluster.ux.uis.no rm -rf /home/danny/teacup/experiment/result

# Copy config.py to UiS Unix host and then to pi3cluster
scp ./config.py danerik@ssh1.ux.uis.no:/home/stud/danerik/teacup/experiment/
ssh danerik@ssh1.ux.uis.no scp /home/stud/danerik/teacup/experiment/config.py root@pi3cluster.ux.uis.no:/home/danny/teacup/experiment/

# Start TEACUP experiment
ssh danerik@ssh1.ux.uis.no ssh root@pi3cluster.ux.uis.no fab -f /home/danny/teacup/experiment/fabfile.py run_experiment_single:test_id=result

# Move new results to experiment folder
ssh danerik@ssh1.ux.uis.no ssh root@pi3cluster.ux.uis.no mv ./experiments_started.txt /home/danny/teacup/experiment/
ssh danerik@ssh1.ux.uis.no ssh root@pi3cluster.ux.uis.no mv ./experiments_completed.txt /home/danny/teacup/experiment/
ssh danerik@ssh1.ux.uis.no ssh root@pi3cluster.ux.uis.no mv ./result /home/danny/teacup/experiment/
