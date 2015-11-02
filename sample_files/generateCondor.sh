#!/bin/bash

# Generate condor submission files
../../bin/splitCondor TOMO_Scan-MEGP.mac ./submit/args-220MBq.txt ./submit/condor.txt $PWD 177Lu_x3_BG_220MBq 177Lu_x3_BG_220MBq 1200 1530 30 ./submit/req1530.txt


