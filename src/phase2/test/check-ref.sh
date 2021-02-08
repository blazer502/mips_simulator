#!/bin/sh

T="reference/taeun/"
I="reference/ikjae/"

# taeun
diff Endtest.txt $(T)Endtest.txt > $(T)Endtest-diff.txt
diff Fibtest.txt $(T)Fibtest.txt > $(T)Fibtest-diff.txt
diff Logicaltest.txt $(T)Logicaltest.txt > $(T)Logicaltest-diff.txt
diff Memorytest.txt $(T)Memorytest.txt > $(T)Memorytest-diff.txt
diff Sumtest.txt $(T)Sumtest.txt > $(T)Sumtest-diff.txt
diff SUtest.txt $(T)SUtest.txt > $(T)SUtest-diff.txt
diff Swaptest.txt $(T)Swaptest.txt > $(T)Swaptest-diff.txt

# ikjae test
diff Endtest.txt $(I)Endtest.txt > $(I)Endtest-diff.txt
diff Fibtest.txt $(I)Fibtest.txt > $(I)Fibtest-diff.txt
diff Logicaltest.txt $(I)Logicaltest.txt > $(I)Logicaltest-diff.txt
diff DH2test.txt $(I)DH2test.txt > $(I)DH2test-diff.txt
diff DH3test.txt $(I)DH3test.txt > $(I)DH3test-diff.txt