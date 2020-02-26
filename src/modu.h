#ifndef MODU_H
#define MODU_H

#include "resource.h"
#include "algvec.h"
#include "msq.h"

class Polygon {
public:
    uint8_t polytype;
    vector<uint8_t> bottomface;
    map<uint8_t, vector<uint8_t> > otherpoints;

    Polygon();
    ~Polygon();
};

struct moducache {
    uint8_t cnum, vnum, chosenC;
    vector<uint8_t> topo;
    map<uint8_t, Coord> coords;
    set<uint8_t> chosenV;
    map<uint8_t, uint8_t> vtkmapping;
};

class Modu {
public:
    uint8_t cnum, vnum, chosenC;
    vector<uint8_t> topo;
    map<uint8_t, Coord> coords;
    set<uint8_t> chosenV;
    int newcelltype;
    set<uint8_t> newcellpoints;
    set<set<uint8_t> > initedfaces;

    moducache cache;

    Modu();
    ~Modu();
    void ReadTopo(string filename);
    void AddNewcell(uint8_t);
    void VtkOut();
    void VtkOut(set<uint8_t>);
    void VtkIn();

    void UntangleforNewCell(Polygon);

private:
    void UntanglePlaneBottom(Polygon);
    void UntangleLineBottom(Polygon);
    void UntanglePointBottom(Polygon);
    map<uint8_t, uint8_t> vtkmapping;

    Coord ProjecttoPlane(Coord, Coord, Algvec);
    Coord FinePoint(uint8_t, Polygon);
};
vector<uint8_t> fullcell(vector<uint8_t> oc);
#endif
