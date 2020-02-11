OPT = -g

modu2geom: resource.h algvec.h mesh.h
	g++ $(OPT) modu2geom.cpp mesh.cpp mesh.h algvec.h algvec.cpp resource.h -o modu2geom -lmesquite

paint: main.o algvec.o modu.o msq.o untangle.o 
	g++ $(OPT) main.o algvec.o modu.o msq.o untangle.o -o paint -lmesquite

main.o: modu.h msq.h
	g++ $(OPT) -c main.cpp
algvec.o: resource.h
	g++ $(OPT) -c algvec.cpp
modu.o: resource.h algvec.h
	g++ $(OPT) -c modu.cpp
msq.o: resource.h
	g++ $(OPT) -c msq.cpp
untangle.o: modu.h resource.h
	g++ $(OPT) -c untangle.cpp

.PHONY: clean
clean:
	-rm *.o
