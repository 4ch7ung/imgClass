#!/bin/bash
if [ $3 -eq "1" ]
then
  meth="SIFT"
else
  meth="SURF"
fi
echo "SVMing $2/10 experiment $meth started"
"$1" "$3" "$4" "$5" "$6" "$7" "$8"
echo "Done $meth $2/10"
