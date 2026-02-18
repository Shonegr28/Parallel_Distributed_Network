#!/bin/bash
# Choose the partition that you will use.
# You can choose: oucspdn_cpu OR debug_5min OR debug OR others 
#
#SBATCH --partition=oucspdn_cpu
#
#SBATCH --ntasks=8
#SBATCH --ntasks-per-node=8
#
#SBATCH --time=00:15:00
#SBATCH --job-name=auto
#
#
# --------------------- Change the items below here! ---------------------
#
#SBATCH --mail-user=Shone.G.George.Kutty.Renjan-1@ou.edu
#SBATCH --mail-type=ALL
#
#SBATCH --chdir=/home/oucspdn020/project_1_autograde/GeorgeKuttyRenjan_Shone_Project_1/
#SBATCH --output=/home/oucspdn020/project_1_autograde/GeorgeKuttyRenjan_Shone_Project_1/autograding_%J_stdout.txt
#SBATCH --error=/home/oucspdn020/project_1_autograde/GeorgeKuttyRenjan_Shone_Project_1/autograding_%J_stderr.txt

#################################################

source /home/oucspdnta/activate_pdn_env.sh

# To run all the problems and get all results
python3 autograder_project_1.py