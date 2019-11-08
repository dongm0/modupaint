#include "hexmesh.h"

Hexmesh::Hexmesh() {}
Hexmesh::~Hexmesh() {}
/*
 *      5-------6
 *     /|      /|
 *    / |     / |
 *   4-------7  |
 *   |  1----|--2
 *   | /     | /
 *   |/      |/
 *   0-------3
 */
void Hexmesh::addFirstCell(vector<uint8_t> rawdata) {
    if (structure.size() != 0)
        return;
    //structure.insert(structure.end(), rawdata.begin(), rawdata.end());
    uint8_t pn = 0;
    for (auto x : rawdata) {
        if (points.find(x) == points.end())
            points.insert(make_pair(x, pn++));
        structure.push_back(points[x]);
    }
    //Coordinates.resize(8);
    //Coordinates[0] = {0, 0, 0};
    //Coordinates[1] = {0, 1, 0};
    //Coordinates[2] = {1, 1, 0};
    //Coordinates[3] = {1, 0, 0};
    //Coordinates[4] = {0, 0, 1};
    //Coordinates[5] = {0, 1, 1};
    //Coordinates[6] = {1, 1, 1};
    //Coordinates[7] = {1, 0, 1};
    return;
}

void Hexmesh::addCell(vector<uint8_t> rawdata, vector<uint8_t> rawlong) {
    int npn = 0;
    //trimDir(rawdata);
    uint8_t pn = points.size();
    vector<uint8_t> newpoints;
    map<uint8_t, uint8_t> mapping;
    for (auto x : rawdata) {
        if (points.find(x) == points.end()) {
            points.insert(make_pair(x, pn));
            newpoints.push_back(pn++);
            npn++;
        }
        //structure.push_back(points[x]);
    }
    for (auto &x : rawdata)
        x = points[x];
    trimDir(rawdata);
    structure.insert(structure.end(), rawdata.begin(), rawdata.end());
    auto r1 = rawlong;
    for (auto &x : r1)
        x = points[x];
    //auto r = rawdata;
    //for (auto &x : r)
        //x = points[x];
    //if (npn == 4)
        //addType1(newpoints, rawdata);
    //if (npn == 2)
        //addType2(newpoints, rawdata);
    //if (npn == 1)
        //addType3(newpoints, r1);
    //movePos();
}

void Hexmesh::vtkWrite(ofstream & fout) {
    fout << "# vtk DataFile Version 3.0\n";
    fout << "Modu Geometry\n";
    fout << "ASCII\n";
    fout << "DATASET UNSTRUCTURED_GRID\n";
    fout << "POINTS " << Coordinates.size() << " double\n";
    for (auto x : Coordinates)
        fout << x.x << " " << x.y << " " << x.z << endl;
    int cellnum = structure.size()/8;
    fout << "CELLS " << cellnum << " " << cellnum*9 << endl;
    for (int i=0; i<cellnum; ++i) {
        fout << "8 ";
        for (int j=0; j<8; ++j)
            fout << (int)structure[i*8+j] << " ";
        fout << endl;
    }
    fout << "CELL_TYPES " << cellnum << endl;
    for (int i=0; i<cellnum; ++i)
        fout << "12" << endl;
}

void Hexmesh::addType1(vector<uint8_t> newpoints, vector<uint8_t> raw) {
    /*
    set<uint8_t> newset(newpoints.begin(), newpoints.end());
    int i;
    for (i=0; i<6; ++i) {
        int q = 0;
        for (int j=0; j<4; ++j) {
            if (newset.find(rawlong[i*4+j]) == newset.end()) break;
            if (j==3 and newset.find(rawlong[i*4+j])!=newset.end()) {
                q = 1;
                break;
            }
        }
        if (q == 1) break;
    }
    vector<uint8_t> tmpcell;
    int p1, p2;
    if (i>0 and i<5)
        p1 = ((i+1)%4+1)*4, p2 = i*4;
    else if (i==0 or i==5)
        p1 = (5-i)*4, p2 = i*4;
    tmpcell.insert(tmpcell.end(), rawlong.begin()+p1, rawlong.begin()+p1+4);
    tmpcell.insert(tmpcell.end(), rawlong.begin()+p2, rawlong.begin()+p2+4);
    auto temp = tmpcell[5];
    tmpcell[5] = tmpcell[4];
    tmpcell[4] = temp;
    temp = tmpcell[7];
    tmpcell[7] = tmpcell[6];
    tmpcell[6] = temp;
    trimDir(tmpcell);
    */
    for (int i=0; i<4; ++i) {
        Coordinates.push_back({0, 0, 0});
    }
    for (int k=0; k<4; k++) {
        Coord c1 = Coordinates[raw[k]];
        Coord c2 = Coordinates[raw[(k+1)%4]];
        Coord c3 = Coordinates[raw[(k+3)%4]];
        auto v1 = crossProduct(Algvec(c1, c3), Algvec(c1, c2));
        v1.Unit();
        Coordinates[raw[k+4]] = v1.getToPoint(c1);
    }
}

void Hexmesh::addType2(vector<uint8_t> newpoints, vector<uint8_t> raw) {
    Coordinates.push_back({0, 0, 0});
    Coordinates.push_back({0, 0, 0});
    for (int i=0; i<2; ++i) {
        /*
        int fn, fp;
        auto p = newpoints[i], pn = newpoints[1-i];
        for (auto j=0; j<6; ++j) {
            int q = 0;
        */
        int fp = 0, fn = 0;
        auto p = newpoints[i], pn = newpoints[1-i];
        for (auto k=0; k<4; ++k) {
            if (raw[4+k] == p) {
                fp = k;
                fn = 1;
                if (raw[4+(k+1)%4] == pn)
                    fn = 3;
                break;
            }
        }
        auto p1 = raw[4+(fp+fn)%4];
        auto p2 = raw[fp];
        auto p3 = raw[(fp+fn)%4];
        auto v = Algvec(Coordinates[p3], Coordinates[p1]) + 
                 Algvec(Coordinates[p3], Coordinates[p2]);
        Coordinates[p] = v.getToPoint(Coordinates[p3]);
    }
}

void Hexmesh::addType3(vector<uint8_t> newpoints, vector<uint8_t> rawlong) {
    Coordinates.push_back({0, 0, 0});
    int fn[3], fp[3], f=0;
    auto p = newpoints[0];
    for (int i=0; i<6; ++i) {
        for (int j=0; j<4; ++j) {
            if (rawlong[i*4+j] == p) {
                fn[f] = i;
                fp[f] = j;
                f++;
                break;
            }
        }
    }
    Coord c[3];
    for (int i=0; i<3; ++i) {
        auto p1 = Coordinates[rawlong[fn[i]*4+(fp[i]+2)%4]];
        auto p2 = Coordinates[rawlong[fn[i]*4+(fp[i]+1)%4]];
        auto p3 = Coordinates[rawlong[fn[i]*4+(fp[i]+3)%4]];
        auto v = Algvec(p1, p2) + Algvec(p1, p3);
        c[i] = v.getToPoint(p1);
        Coordinates[p].x += c[i].x/3;
        Coordinates[p].y += c[i].y/3;
        Coordinates[p].z += c[i].z/3;
    }
}

void Hexmesh::movePos() {
    int i=0;
    while (i++ < 100) { //最外层大循环
        for (auto x : points) {
            auto p = x.second;
            vector<Coord> candidate(9, Coordinates[p]);
            for (int i1=0; i1<2; ++i1) {
                for (int i2=0; i2<2; ++i2) {
                    for (int i3=0; i3<2; ++i3) {
                        candidate[i1*4+i2*2+i3+1].x += (i1-0.5)*0.02;
                        candidate[i1*4+i2*2+i3+1].y += (i2-0.5)*0.02;
                        candidate[i1*4+i2*2+i3+1].z += (i3-0.5)*0.02;
                    }
                }
            }
            double minf = func(structure, Coordinates);
            int minp = 0;
            for (int i=1; i<9; ++i) {
                Coordinates[p] = candidate[i];
                double nm = func(structure, Coordinates);
                if (nm < minf) {
                    minf = nm;
                    minp = i;
                }
            }
            Coordinates[p] = candidate[minp];
        }
    }
}

void Hexmesh::trimDir(vector<uint8_t> &tempcell) {
    uint8_t cellnum = structure.size()/8;
    vector<uint8_t> uf(tempcell.begin(), tempcell.begin()+4);
    //set<uint8_t> suf(tempcell.begin(), tempcell.begin()+4);
    int cn = -1;
    for (int i=0; i<cellnum; ++i) {
        set<uint8_t> tmp(structure.begin()+i*8, structure.begin()+i*8+8);
        bool b = true;
        for (auto x : uf) {
            if (tmp.find(x) == tmp.end()) {
                b = false;
                break;
            }
        }
        if (b) {
            cn = i;
            break;
        }
    }
    vector<vector<uint8_t> > fs(6);
    int arr[24] {0, 1, 2, 3, 1, 0, 4, 5, 2, 1, 5, 6, 
                 3, 2, 6, 7, 0, 3, 7, 4, 4, 7, 6, 5};
    for (auto i=0; i<6; ++i) {
        for (int j=0; j<4; ++j)
            fs[i].emplace_back(structure[cn*8+arr[i*4+j]]);
    }
    for (auto f : fs) {
        if (set<uint8_t>(uf.begin(), uf.end()) == set<uint8_t>(f.begin(), f.end())) {
            for (int i=0; i<4; ++i) {
                if (f[i] == uf[0]) {
                    if (f[(i+3)%4] == uf[1])
                        return;
                    else {
                        auto temp = tempcell[1];
                        tempcell[1] = tempcell[3];
                        tempcell[3] = temp;
                        temp = tempcell[5];
                        tempcell[5] = tempcell[7];
                        tempcell[7] = temp;
                        return;                        
                    }
                }
            }
        }
    }
}

double func(vector<uint8_t> stru, vector<Coord> co) {
    double res = 0;
    int cellnum = stru.size()/8;
    for (int i=0; i<cellnum; ++i) {
        vector<uint8_t> c(stru.begin()+i*8, stru.begin()+i*8+8);
        auto mid = midPrecudure(c, co);
        res += fabs(fsize(mid)-1);
        res += fabs(fshape(mid)-1);
        res += fabs(fskew(mid)-1);
    }
    res = res/cellnum;
    res = res/3;
    return res;
}

vector<double> midPrecudure(vector<uint8_t> ls, vector<Coord> co) {
    int mapping[24] {1, 4, 3, 2, 5, 0, 3, 6, 1, 0, 7, 2,
                     7, 0, 5, 4, 1, 6, 5, 2, 7, 6, 3, 4};
    vector<double> res;
    for (int i=0; i<8; ++i) {
        Algvec v1(co[ls[i]], co[ls[mapping[i*3]]]);
        Algvec v2(co[ls[i]], co[ls[mapping[i*3+1]]]);
        Algvec v3(co[ls[i]], co[ls[mapping[i*3+2]]]);
        res.push_back(v1.euLength());
        res.push_back(v2.euLength());
        res.push_back(v3.euLength());
        res.push_back(mixProduct(v1, v2, v3));
    }
    return res;
}

double fsize(vector<double> mid) {
    double t = 0;
    for (int i=0; i<8; ++i) {
        if (mid[i*4+3] > 0)
            t += mid[i*4+3];
    }
    t = t/8;
    if (t > 1/t) t = 1/t;
    return t;
}
double fshape(vector<double> mid) {
    double s = 0;
    for (int i=0; i<8; ++i) {
        double s1 = mid[i*4] + mid[i*4+1] + mid[i*4+2];
        s1 = s1/pow(mid[i*4+3], 2/3);
        s += s1;
    }
    return 24/s;
}
double fskew(vector<double> mid) {
    double s = 0;
    for (int i=0; i<8; ++i) {
        double s1 = sqrt(mid[i*4]*mid[i*4+1]*mid[i*4+2]);
        s1 = s1/mid[i*4+3];
        s1 = pow(s1, 2/3);
        s += s1;
    }
    return 8/s;
}
