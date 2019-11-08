paint: main.o algvec.o modu.o msq.o 
	g++ -g main.o algvec.o modu.o msq.o -o paint -lmesquite

main.o: modu.h msq.h
	g++ -g -c main.cpp
algvec.o: resource.h
	g++ -g -c algvec.cpp
modu.o: resource.h algvec.h
	g++ -g -c modu.cpp
msq.o: resource.h
	g++ -g -c msq.cpp

.PHONY: clean
clean:
	-rm *.o
