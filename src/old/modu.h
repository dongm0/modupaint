#ifndef MODU_H
#define MODU_H

#include "resource.h"

class Modu {
public:
    uint8_t vn, en, fn;


    typedef set<uint8_t> vertexset;
    typedef set<set<uint8_t> > edgeset;
    typedef set<set<uint8_t> > faceset;
    
    vertexset _vertexset;
    edgeset _edgeset;
    faceset _faceset;
    map<vertexset::iterator, set<uint8_t> > _nbhpoints;
    map<edgeset::iterator, vector<uint8_t> > _dihedral;
    map<faceset::iterator, vector<uint8_t> > _diagonal;
    vector<uint8_t> fixedpoints;
    Modu();
    ~Modu();
};

void ReadModu(vector<uint8_t> &raw, vector<uint8_t> &rawlong, string filename, string mode);

#endif
