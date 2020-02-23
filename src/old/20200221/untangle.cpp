#include "untangle.h"

pair<uint8_t, Polygon> is_tangled(Modu &modu) {
    set<set<uint8_t> > faces;
    for (int32_t i=0; i<modu.chosenC; ++i) {
        faces.insert(set<uint8_t>{modu.topo[i*8+0], modu.topo[i*8+1], modu.topo[i*8+2], modu.topo[i*8+3]});
        faces.insert(set<uint8_t>{modu.topo[i*8+4], modu.topo[i*8+5], modu.topo[i*8+6], modu.topo[i*8+7]});
        faces.insert(set<uint8_t>{modu.topo[i*8+0], modu.topo[i*8+1], modu.topo[i*8+4], modu.topo[i*8+5]});
        faces.insert(set<uint8_t>{modu.topo[i*8+1], modu.topo[i*8+2], modu.topo[i*8+5], modu.topo[i*8+6]});
        faces.insert(set<uint8_t>{modu.topo[i*8+2], modu.topo[i*8+3], modu.topo[i*8+6], modu.topo[i*8+7]});
        faces.insert(set<uint8_t>{modu.topo[i*8+3], modu.topo[i*8+4], modu.topo[i*8+7], modu.topo[i*8+0]});
    }
    vector<set<uint8_t> > newc;
    int cnum = modu.chosenC;
    newc.push_back(set<uint8_t>{modu.topo[cnum*8+0], modu.topo[cnum*8+1], modu.topo[cnum*8+2], modu.topo[cnum*8+3]});
    newc.push_back(set<uint8_t>{modu.topo[cnum*8+4], modu.topo[cnum*8+5], modu.topo[cnum*8+6], modu.topo[cnum*8+7]});
    newc.push_back(set<uint8_t>{modu.topo[cnum*8+0], modu.topo[cnum*8+1], modu.topo[cnum*8+4], modu.topo[cnum*8+5]});
    newc.push_back(set<uint8_t>{modu.topo[cnum*8+1], modu.topo[cnum*8+2], modu.topo[cnum*8+5], modu.topo[cnum*8+6]});
    newc.push_back(set<uint8_t>{modu.topo[cnum*8+2], modu.topo[cnum*8+3], modu.topo[cnum*8+6], modu.topo[cnum*8+7]});
    newc.push_back(set<uint8_t>{modu.topo[cnum*8+3], modu.topo[cnum*8+4], modu.topo[cnum*8+7], modu.topo[cnum*8+0]});
    vector<uint8_t> oldfaces;
    for (int8_t i=0; i<6; ++i)
        if (faces.find(newc[i]) != faces.end())
            oldfaces.push_back(i);
    if (oldfaces.size() == 0)
        return make_pair(0, Polygon());
    if (oldfaces.size() == 1)
        return make_pair(1, Polygon());
    else
        return untangle(&modu, newc, oldfaces);
}

pair<uint8_t, Polygon> untangle(Modu *modu, vector<set<uint8_t> > newcell, vector<uint8_t> oldfaces) {
    if (oldfaces[0] != 0) {}
    uint8_t cent = findcentface(oldfaces);
    uint8_t pp[18] = {0, 1, 2, 4, 7, 6, 0, 4, 5, 1, 5, 6, 2, 6, 7, 3, 7, 4};
    double p0 = pp[cent*3], p1 = pp[cent*3+1], p2 = pp[cent*3+2];
    Algvec e1(modu->coords[modu->topo[(modu->chosenC)*8+p0]], modu->coords[modu->topo[(modu->chosenC)*8+p1]]);
    Algvec e2(modu->coords[modu->topo[(modu->chosenC)*8+p1]], modu->coords[modu->topo[(modu->chosenC)*8+p2]]);
    Algvec norm = crossProduct(e1, e2);
    norm.Unit();
    Coord fromp = modu->coords[modu->topo[(modu->chosenC)*8+p0]];
    for (uint8_t i=0; i<oldfaces.size(); ++i) {
        if (oldfaces[i] == cent) continue;
        for (auto x : newcell[oldfaces[i]]) {
            if (newcell[oldfaces[0]].find(x) == newcell[oldfaces[0]].end()) {
                Algvec v1(modu->coords[x], fromp);
                if (dotProduct(v1, norm) > 0) { // 需要位移
                    modu->coords[x] = movetoplane(modu->coords[x], norm, fromp);
                    //modu->VtkOut();
                }
            }
        }
    }
    return make_pair(oldfaces.size(), Polygon());
}

Coord movetoplane(Coord p0, Algvec norm, Coord basepoint) {
    double l, para = 1.1;
    double x0, y0, z0, x1, y1, z1, i, j, k;
    x0=basepoint.x, y0=basepoint.y, z0=basepoint.z;
    x1=p0.x, y1=p0.y, z1=p0.z, i=norm.dx, j=norm.dy, k=norm.dz;
    l = i*x0+j*y0+k*z0-i*x1-j*y1-k*z1;
    l = l/(i*i+j*j+k*k);
    Coord res{x1+l*i*para, y1+l*j*para, z1+l*k*para};
    return res;
}

uint8_t findcentface(vector<uint8_t> faces) {
    if (faces.size() < 3)
        return 0;
    else if (faces.size() < 5) {
        if (faces[1] == 1)
            return faces[2];
        else
            return 0;
    }
    else {
        if (faces[1] != 1)
            return 0;
        else if (faces[2] != 2)
            return 4;
        else if (faces[3] != 3)
            return 5;
        else if (faces[4] != 4)
            return 2;
        else 
            return 3;
    }
}
