#!/bin/bash

OUTDIR=/tmp
EXEC_DIR=/home/cmsdaq/DAQ/VMEDAQ

echo "Counting for `expr $1 \* 1000`ms with HV settings: PMT8 $2 PMT9 $3"
echo "Writing output into $OUTDIR/scaler_Plateau_PMT8_$2_PMT9_$3_$1s.txt"
${EXEC_DIR}/count -t `expr $1 \* 1000` | grep "V560:: channel" | awk '{printf "%d\t%d\n",$3,$5}' |& tee $OUTDIR/scaler_Plateau_PMT8_$2_PMT9_$3_$1s.txt