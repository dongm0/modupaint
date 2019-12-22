OPT = -O2

paint: main.o algvec.o modu.o msq.o 
	g++ $(OPT) main.o algvec.o modu.o msq.o -o paint -lmesquite

main.o: modu.h msq.h
	g++ $(OPT) -c main.cpp
algvec.o: resource.h
	g++ $(OPT) -c algvec.cpp
modu.o: resource.h algvec.h
	g++ $(OPT) -c modu.cpp
msq.o: resource.h
	g++ $(OPT) -c msq.cpp

.PHONY: clean
clean:
	-rm *.o
