#!/bin/bash

##############
# Run the macro on a single thread
# Should be run from ../macros/ directory 
##############

Gate \
-a source Source-99mTc_Cylindrical_Phantom_x3Inserts.mac \
-a gamma_activity1 1050619.43 \
-a beta_activity1 38.87 \
-a gamma_activity2 985611.04 \
-a beta_activity2 36.47 \
-a gamma_activity3 1027551.94 \
-a beta_activity3 38.02 \
-a gate_install_dir /opt/gate_v6.1/ \
-a working_dir /home/apr/SPECT/GATE6.1_ver2/trunk   \
-a seed auto \
-a time_slice 20 \
-a time_start 0 \
-a time_stop 600 \
-a t0 0 \
-a t1 600 \
-a output_file 99mTc_Scan-1 \
-a output_dir 99mTc_Scan \
-a omega -0.3000000119 \
TOMO_Scan-LEHR.mac | tee ../logs/99mTc_Scan-1.log
