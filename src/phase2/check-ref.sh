#!/bin/bash

T="test/reference/taeyun/"
I="test/reference/ikjae/"

make clean
make test

# You can compare outputs by ikjae and tayun
cd output
diff out-End.txt ${T}out-End.txt > ${T}out-End-diff.txt
diff out-Fibonacci.txt ${T}out-Fibonacci.txt > ${T}out-Fibonacci-diff.txt
diff out-Logical.txt ${T}out-Logical.txt > ${T}out-Logical-diff.txt
diff out-Memory.txt ${T}out-Memory.txt > ${T}out-Memory-diff.txt
diff out-Sum.txt ${T}out-Sum.txt > ${T}out-Sum-diff.txt
diff out-SU.txt ${T}out-SU.txt > ${T}out-SU.txt
diff out-Swap.txt ${T}out-Swap.txt > ${T}out-Swap-diff.txt
