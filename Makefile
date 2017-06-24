
all: Main.o Interpreter.o
	g++ -Wall Main.o Interpreter.o -o Main.out

Main.o: Main.cpp
	g++ -Wall -c Main.cpp

Interpreter:o Interpreter.cpp
	g++ -Wall -c Interpreter.cpp

run: 
	./Main.out inst.txt data.txt
# -f removes any errors if the file is not present
clean:
	rm -rf *.o
	rm -f Main.out
	rm -f *~ *.h.gch *#
