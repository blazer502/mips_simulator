MIPS = mips_simu
$(MIPS) : hw4.c mode0.h mode0.c
	$(CC) -o $@ $^

debug : hw4.c mode0.h mode0.c
	$(CC) -o mips_debug -O0 -g $^
clean: 
	rm -rf $(MIPS) mips_debug

TEST := Sum DH2 DH3 End Fibonacci Logical Memory SU Swap

all : $(TEST) $(MIPS)

$(TEST): $(MIPS)
	./$(MIPS) binary/$@.txt 1000 1 > check/$@test.txt

