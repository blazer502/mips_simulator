TARGET = simulator
SOURCES = main.c Instruction.c Instruction.h functionality.c functionality.h unit.c unit.h

$(TARGET): $(SOURCES)
	gcc -o $(TARGET) -O2 -Wall $(SOURCES)

test1: $(TARGET)
	./$(TARGET) p1e1.txt 5 > out-p1e1.txt && diff p1e1-output.txt out-p1e1.txt

test2: $(TARGET)
	./$(TARGET) p1e2.txt 10 > out-p1e2.txt
	
test3: $(TARGET)
	./$(TARGET) p1e3.txt 20 > out-p1e3.txt
	
test4: $(TARGET)
	./$(TARGET) p1e4.txt 70 > out-p1e4.txt

clean:
	rm -rf *.o simulator

# This is commented out but you can add more tests if you want to
#test: test1 test2 test3 test4

test: test1 test2 test3 test4

clean: clean
