#ifndef MODU_H
#define MODU_H

#include "resource.h"
#include "algvec.h"
#include "msq.h"

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

    moducache cache;

    Modu();
    ~Modu();
    void ReadTopo(string filename);
    void AddNewcell();
    void VtkOut();
    void VtkIn();

    void SaveCache();
    void LoadCache();
    bool TryUntangle();

private:
    map<uint8_t, uint8_t> vtkmapping;
    void AddType0();
    void AddType1();
    void AddType2();
    void AddType3();

    void UntangleType1(set<set<uint8_t> >);
    void UntangleType2(set<set<uint8_t> >);
    void UntangleType3(set<set<uint8_t> >);
    void Custom_push(vector<uint8_t>, vector<uint8_t>, set<uint8_t>);
    Coord movepos(Algvec, Coord, Coord);
    /* bool AddVirtualType1(set<set<uint8_t> > nfs); */
    /* bool AddVirtualType2(); */
    /* bool AddVirtualType3(); */

    /* void FindPosv1(vector<uint8_t>&, vector<uint8_t>&, vector<uint8_t>&, set<set<uint8_t> >); */
};

#endif
