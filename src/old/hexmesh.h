#ifndef HEXMESH_H
#define HEXMESH_H

#include "resource.h"
#include "algvec.h"
class Hexmesh {
public:
    vector<uint8_t> structure;
    map<uint8_t, uint8_t> points;
    vector<Coord> Coordinates;
    //rawdata是从文件读入的数据,稍作处理
    

    Hexmesh();
    ~Hexmesh();
    void addFirstCell(vector<uint8_t> rawdata);
    void addCell(vector<uint8_t> rawdata, vector<uint8_t>);
    void vtkWrite(ofstream &fout);

private:
    void addType1(vector<uint8_t> rawdata, vector<uint8_t>);
    void addType2(vector<uint8_t> rawdata, vector<uint8_t>);
    void addType3(vector<uint8_t> rawdata, vector<uint8_t>);
    void movePos();
    void trimDir(vector<uint8_t> &tempcell);
};

double func(vector<uint8_t> structure, vector<Coord> Coordinates);
vector<double> midPrecudure(vector<uint8_t> cellstructure, vector<Coord> Coordinates);
double fsize(vector<double> mid);
double fshape(vector<double> mid);
double fskew(vector<double> mid);

#endif
