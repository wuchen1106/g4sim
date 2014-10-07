#!/bin/sh

bg4sim_sub -t sim -b 0 -e 0 -Y Al100_1.04 Al100_1.04

for i in {06..10}
do
  mom=$(printf "1.%.2d" "$i")
  bg4sim_sub -t sim -b 0 -e 0 -Y Al100_$mom Al100_$mom 
done

bg4sim_sub -t sim -b 0 -e 0 -Y Al100_1.12 Al100_1.12
