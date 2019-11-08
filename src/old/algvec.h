#ifndef ALGVEC_H
#define ALGVEC_H
#include "resource.h"

struct Coord {
    double x;
    double y;
    double z;
};
class Algvec {
public:
    double dx, dy, dz;

    Algvec();
    Algvec(Coord frompoint, Coord topoint);
    ~Algvec();
    void Unit();
    Coord getFromPoint(Coord topoint);
    Coord getToPoint(Coord frompoint);
    double euLength();
    Algvec operator+(Algvec v1) const;
    Algvec operator-(Algvec v2) const;

};

double dotProduct(Algvec v1, Algvec v2);
Algvec crossProduct(Algvec v1, Algvec v2);
double mixProduct(Algvec v1, Algvec v2, Algvec v3);
#endif
