#!/bin/bash

# Generate PBS submission files
splitPBS TOMO_Scan-LEHR.mac ./submit/args_christie.txt $SWD/JobSplitter/sample_files/pbsheader_christie.txt $PWD 99mTc_Scan 99mTc_Scan 600 30 30
