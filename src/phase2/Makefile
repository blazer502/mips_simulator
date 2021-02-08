TARGET = simulator
SOURCES = main.c instruction.c instruction.h unit.c unit.h
TEST = 
MODE = 1  # Choose mode

$(TARGET): $(SOURCES)
	gcc -o $(TARGET) -O2 -Wall $(SOURCES)
	# gcc -DDO_DEBUG=1 -g3 -o $(TARGET) $(SOURCES) 

# Custom Test
r-type: $(TARGET)
	./$(TARGET) $(TEST)r-type.txt 50 $(MODE) > out-r-type.txt

i-type: $(TARGET)
	./$(TARGET) $(TEST)i-type.txt 50 $(MODE) > out-i-type.txt
	
branch: $(TARGET)
	./$(TARGET) $(TEST)branch.txt 50 $(MODE) > out-branch.txt

# Formal Test
DH-all: $(TARGET)
	./$(TARGET) $(TEST)DH1.txt 1000 $(MODE) > out-DH1.txt
	./$(TARGET) $(TEST)DH2.txt 1000 $(MODE) > out-DH2.txt
	./$(TARGET) $(TEST)DH3.txt 1000 $(MODE) > out-DH3.txt
	./$(TARGET) $(TEST)DH4.txt 1000 $(MODE) > out-DH4.txt
	./$(TARGET) $(TEST)DH5.txt 1000 $(MODE) > out-DH5.txt
	./$(TARGET) $(TEST)DH6.txt 1000 $(MODE) > out-DH6.txt

SU: $(TARGET)
	./$(TARGET) $(TEST)SU.txt 100 $(MODE) > out-SU.txt

Sum: $(TARGET)
	./$(TARGET) $(TEST)Sum.txt 1000 $(MODE) > out-Sum.txt

logical: $(TARGET)
	./$(TARGET) $(TEST)Logical.txt 100 $(MODE) > out-Logical.txt

End: $(TARGET)
	./$(TARGET) $(TEST)End.txt 100 $(MODE) > out-End.txt

Fibonacci: $(TARGET)
	./$(TARGET) $(TEST)Fibonacci.txt 500 $(MODE) > out-Fibonacci.txt

Memory: $(TARGET)
	./$(TARGET) $(TEST)Memory.txt 100 $(MODE) > out-Memory.txt

Swap: $(TARGET)
	./$(TARGET) $(TEST)Swap.txt 200 $(MODE) > out-Swap.txt

# This is commented out but you can add more tests if you want to
test: SU Sum logical End Fibonacci Memory Swap DH-all

mode0: test

mode1: test

clean:
	rm simulator

