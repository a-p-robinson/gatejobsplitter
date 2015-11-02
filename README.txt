#########################################        
# README.txt                            #
#                                       #
# APR: 12/07/12                         #
# andrew.paul.robinson@manchester.ac.uk #
#########################################

Usage:

1. cd src
   make all

2. Sample argument and condor / pbs header files can be found in ./sample_files/

3. To prepare a job for condor submission do,

   splitCondor macro_filename arguments_filename condor_arguments_filename InitialDir output_filename total_sim_time number_of_splits macine_req_filename(optional)

   eg:
   splitCondor TOMO.mac args.txt condor.txt /home/me/SPECT/ scan1 600 30 (reqs.txt)

4. To prepare a job for PBS(torque) submission do,

   splitPBS macro_filename arguments_filename pbs_arguments_filename InitialDir output_dir output_filename total_sim_time number_of_splits number_of_angles(optional) machine_requirements_filename(optional)

   eg:
   splitPBS TOMO.mac args.txt pbs.txt /home/me/SPECT/ TOMO_3MBq scan1 600 30 (30) (reqs.txt)

5. Example shell scripts to generate the submit files can be found in ./sample_files/ (These shoudl be run from the base directory of the simulation).
