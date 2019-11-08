#include "algvec.h"

Algvec::Algvec(){}
Algvec::~Algvec(){}

Algvec::Algvec(Coord fp, Coord tp) {
    dx = tp.x - fp.x;
    dy = tp.y - fp.y;
    dz = tp.z - fp.z;
}

double Algvec::euLength() {
    return sqrt(dx*dx+dy*dy+dz*dz);
}

Coord Algvec::getToPoint(Coord fp) {
    Coord res = {fp.x+dx, fp.y+dy, fp.z+dz};
    return res;
}

Coord Algvec::getFromPoint(Coord tp) {
    return {tp.x-dx, tp.y-dy, tp.z-dz};
}

void Algvec::Unit() {
    double l = euLength();
    dx = dx/l;
    dy = dy/l;
    dz = dz/l;
}


Algvec Algvec::operator+(Algvec v1) const {
    Algvec res;
    res.dx = dx + v1.dx;
    res.dy = dy + v1.dy;
    res.dz = dz + v1.dz;
    return res;
}
Algvec Algvec::operator-(Algvec v1) const {
    Algvec res;
    res.dx = dx - v1.dx;
    res.dy = dy - v1.dy;
    res.dz = dz - v1.dz;
    return res;
}
Algvec Algvec::operator*(double a) const {
    Algvec res;
    res.dx = dx*a;
    res.dy = dy*a;
    res.dz = dz*a;
    return res;
}
Algvec operator*(double a, const Algvec &v) {
    Algvec res;
    res.dx = v.dx*a;
    res.dy = v.dy*a;
    res.dz = v.dz*a;
    return res;
}

double dotProduct(Algvec v1, Algvec v2) {
    return (v1.dx*v2.dx+v1.dy*v2.dy+v1.dz*v2.dz);
}
Algvec crossProduct(Algvec v1, Algvec v2) {
    Algvec res;
    res.dx = v1.dy*v2.dz - v1.dz*v2.dy;
    res.dy = v1.dz*v2.dx - v1.dx*v2.dz;
    res.dz = v1.dx*v2.dy - v1.dy*v2.dx;
    return res;
}
double mixProduct(Algvec v1, Algvec v2, Algvec v3) {
    return v1.dx*v2.dy*v3.dz + v1.dy*v2.dz*v3.dx +
           v1.dz*v2.dx+v3.dy - v1.dx*v2.dz*v3.dy - 
           v1.dy*v2.dx*v3.dz - v1.dz*v2.dy*v3.dx;
}
