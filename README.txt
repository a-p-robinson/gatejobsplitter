#########################################        
# README.txt                            #
#                                       #
# APR: 16/05/17                         #
# andrew.robinson@npl.co.uk             #
#########################################

Usage:

1. cd src
   make all

2. Sample argument and condor / pbs header files can be found in ./sample_files/

3. To prepare a job for condor submission do,

   splitCondor GATE_VERSION macro_filename arguments_filename condor_arguments_filename InitialDir output_filename total_sim_time number_of_splits machine_req_filename(optional)

   eg:
   splitCondor 7.2 TOMO.mac args.txt condor.txt /home/me/SPECT/ scan1 600 30 (reqs.txt)

4. To prepare a job for PBS(torque) submission do,

   splitPBS macro_filename arguments_filename pbs_arguments_filename InitialDir output_dir output_filename total_sim_time number_of_splits number_of_angles(optional) machine_requirements_filename(optional)

   eg:
   splitPBS TOMO.mac args.txt pbs.txt /home/me/SPECT/ TOMO_3MBq scan1 600 30 (30) (reqs.txt)

5. Example shell scripts to generate the submit files can be found in ./sample_files/ (These should be run from the base directory of the simulation).

 
########################
# Notes on version 2.0 #
########################

splitCondor has been modified to allow for the change in argument syntax with GATE 6.2+

splitPBS has NOT been modified to support the syntax change.

The example files may be out of date - it is probably better to use one from the SPECT simulation.
