#include "modu.h"

Polygon::Polygon() {}
Polygon::~Polygon() {}

Modu::Modu() {
    cnum = 0;
    vnum = 0;
    chosenC = 0;
    topo = vector<uint8_t>();
    vtkmapping = map<uint8_t, uint8_t>();
    chosenV = set<uint8_t>();
    coords = map<uint8_t, Coord>();
    return;
}
Modu::~Modu(){}

void Modu::ReadTopo(string filename) {
    ifstream fin;
    fin.open(filename);
    uint32_t tmp;
    fin >> tmp;
    cnum = (uint8_t)tmp;
    topo.resize(cnum*8);
    for (uint8_t i=0; i<cnum; ++i) {
        for (uint8_t j=0; j<8; ++j) {
            fin >> tmp;
            topo[i*8+j] = (uint8_t)tmp;
        }
    }
    fin.close();
    set<uint8_t> tmpset;
    for (auto x : topo)
        tmpset.insert(x);
    vnum = tmpset.size();
    chosenC = 0;
    return;
}

void Modu::VtkOut() {
    //假定这里chosenV和chosenC已经就绪
    //vtkmapping也已经就绪
    ofstream fout;
    string filename = "./vtk/" + to_string(chosenC);
    filename += "_tmp.vtk";
    fout.open(filename);
    fout << "# vtk DataFile Version 3.0\n";
    fout << "Modu Geometry\n";
    fout << "ASCII\n";
    fout << "DATASET UNSTRUCTURED_GRID\n";
    fout << "POINTS " << chosenV.size() << " double\n";
    for (auto x : chosenV)
        fout << coords[x].x << " " << coords[x].y << " " << coords[x].z << endl;
    fout << "CELLS " << (int)chosenC << " " << (int)chosenC*9 << endl;
    for (int i=0; i<chosenC; ++i) {
        fout << "8 ";
        for (int j=0; j<8; ++j)
            fout << (int)topo[i*8+j] << " ";
        fout << endl;
    }
    fout << "CELL_TYPES " << (int)chosenC << endl;
    for (int i=0; i<chosenC; ++i)
        fout << "12" << endl;
    fout << "POINT_DATA " << (int)chosenV.size() << endl;
    fout << "SCALARS fixed int\nLOOKUP_TABLE default\n";
    for (int i=0; i<chosenV.size(); ++i) {
        if (i <= 1) fout << "1\n";
        else fout << "0\n";
    }
    fout.close();
}

void Modu::VtkOut(set<uint8_t> fixed) {
    ofstream fout;
    string filename = "./vtk/" + to_string(chosenC);
    filename += "_tmp.vtk";
    fout.open(filename);
    fout << "# vtk DataFile Version 3.0\n";
    fout << "Modu Geometry\n";
    fout << "ASCII\n";
    fout << "DATASET UNSTRUCTURED_GRID\n";
    fout << "POINTS " << chosenV.size() << " double\n";
    for (auto x : chosenV)
        fout << coords[x].x << " " << coords[x].y << " " << coords[x].z << endl;
    fout << "CELLS " << (int)chosenC << " " << (int)chosenC*9 << endl;
    for (int i=0; i<chosenC; ++i) {
        fout << "8 ";
        for (int j=0; j<8; ++j)
            fout << (int)topo[i*8+j] << " ";
        fout << endl;
    }
    fout << "CELL_TYPES " << (int)chosenC << endl;
    for (int i=0; i<chosenC; ++i)
        fout << "12" << endl;
    fout << "POINT_DATA " << (int)chosenV.size() << endl;
    fout << "SCALARS fixed int\nLOOKUP_TABLE default\n";
    for (int i=0; i<chosenV.size(); ++i) {
        //if (i <= 2) fout << "1\n";
        if (fixed.find(i) != fixed.end()) fout << "1\n";
        else fout << "0\n";
    }
    fout.close();
}

void Modu::VtkIn() {
    string filename = "./vtk/" + to_string(chosenC);
    filename += "_tmp_opt.vtk";
    ifstream fin;
    fin.open(filename);
    char buffer[256];
    fin.getline(buffer, 256);
    fin.getline(buffer, 256);
    fin.getline(buffer, 256);
    fin.getline(buffer, 256);
    fin.getline(buffer, 256);
    for (auto x : chosenV) {
        fin >> coords[x].x;
        fin >> coords[x].y;
        fin >> coords[x].z;
    }
    fin.close();
    return;
}

void Modu::AddNewcell(uint8_t c) {
    vector<uint8_t> cell(topo.begin()+c*8, topo.begin()+c*8+8);
    map<uint8_t, uint8_t> vpos;
    for (uint8_t i=0; i<8; ++i)
        vpos.insert(make_pair(cell[i], i));
    vector<uint8_t> fc = fullcell(cell);
    vector<uint8_t> faces;
    for (uint8_t i=0; i<6; ++i) {
        set<uint8_t> f(fc.begin()+i*4, fc.begin()+i*4+4);
        if (initedfaces.find(f) != initedfaces.end()) 
            faces.push_back(i);
    }
    if (faces.size() == 0) {
        coords.insert(make_pair(0, Coord{0, 0, 0}));
        coords.insert(make_pair(1, Coord{1, 0, 0}));
        coords.insert(make_pair(2, Coord{1, 1, 0}));
        coords.insert(make_pair(3, Coord{0, 1, 0}));
        coords.insert(make_pair(4, Coord{0, 0, 1}));
        coords.insert(make_pair(5, Coord{1, 0, 1}));
        coords.insert(make_pair(6, Coord{1, 1, 1}));
        coords.insert(make_pair(7, Coord{0, 1, 1}));
    }
    else if (faces.size() == 1) {
        Polygon poly;
        poly.polytype = 1;
        uint8_t bfnum = faces[0];
        poly.bottomface = vector<uint8_t>{fc[bfnum*4], fc[bfnum*4+3], fc[bfnum*4+2], fc[bfnum*4+1]};
        vector<uint8_t> bf = poly.bottomface;
        vector<uint8_t> tf{fc[bfnum*4+24], fc[bfnum*4+3+24], fc[bfnum*4+2+24], fc[bfnum*4+1+24]};
        Coord cs[4] = {coords[bf[0]], coords[bf[1]], coords[bf[2]], coords[bf[3]]};
        for (uint8_t i=0; i<4; ++i) {
            Algvec v1(cs[(i+3)%4], cs[i]);
            Algvec v2(cs[i], cs[(i+1)%4]);
            Algvec vn = crossProduct(v1, v2);
            vn.Unit();
            Coord v3 = vn.getToPoint(cs[i]);
            coords[tf[i]] = v3;
        }
    }
    else if (faces.size() == 2) {
        Polygon poly;
        poly.polytype = 2;
        uint8_t bfnum = faces[0];
        poly.bottomface = vector<uint8_t>{fc[bfnum*4], fc[bfnum*4+3], fc[bfnum*4+2], fc[bfnum*4+1]};
        vector<uint8_t> tf{fc[bfnum*4+24], fc[bfnum*4+3+24], fc[bfnum*4+2+24], fc[bfnum*4+1+24]};
        map<uint8_t, uint8_t> pos;//底面每个点在底面中的位置
        for (uint8_t i=0; i<4; ++i)
            pos.insert(make_pair(poly.bottomface[i], i));
        for (uint8_t i=0; i<faces.size(); ++i) {
            if (faces[i] == bfnum) continue;
            vector<uint8_t> face{fc[faces[i]*4], fc[faces[i]*4+3], fc[faces[i]*4+2], fc[faces[i]*4+1]};
            for (uint8_t i=0; i<4; ++i) {
                if (pos.find(face[i])!=pos.end() and pos.find(face[(i+1)%4])!=pos.end()) {
                    uint8_t pos1 = pos[face[i]];
                    uint8_t pos2 = (pos[face[i]]+1)%4;
                    uint8_t pos3 = pos[face[(i+1)%4]];
                    uint8_t pos4 = (pos[face[(i+1)%4]]+3)%4;
                    if (poly.otherpoints.find(face[(i+3)%4]) == poly.otherpoints.end())
                        poly.otherpoints.insert(make_pair(face[(i+3)%4], vector<uint8_t>{poly.bottomface[pos1]}));
                    poly.otherpoints[face[(i+3)%4]].push_back(poly.bottomface[pos2]);
                    if (poly.otherpoints.find(face[(i+2)%4]) == poly.otherpoints.end())
                        poly.otherpoints.insert(make_pair(face[(i+2)%4], vector<uint8_t>{poly.bottomface[pos3]}));
                    poly.otherpoints[face[(i+2)%4]].push_back(poly.bottomface[pos4]);
                }
            }
        }
        auto bf = poly.bottomface;
        for (uint8_t i=0; i<4; ++i) {//新的点定位置
            if (poly.otherpoints.find(tf[i])!=poly.otherpoints.end() or poly.otherpoints.find(tf[(i+1)%4])!=poly.otherpoints.end()) continue;
            Coord c1s[3] = {coords[bf[(i+3)%4]], coords[bf[i]], coords[tf[(i+3)%4]]}, 
                  c2s[3] = {coords[bf[(i+2)%4]], coords[bf[(i+1)%4]], coords[tf[(i+2)%4]]};
            Algvec n = crossProduct(Algvec(c1s[0], c1s[1]), Algvec(c1s[1], c2s[1]));
            Algvec n1 = crossProduct(Algvec(c1s[2], c1s[0]), Algvec(c1s[0], c2s[0]));
            n.Unit();
            n1.Unit();
            Algvec v1 = n + n1;
            v1.Unit();
            v1 = v1*1.414;
            if (dotProduct(v1, n) < 0) v1 = v1*(-1);
            coords[tf[i]] = v1.getToPoint(c1s[0]);
            /* Algvec v2 = Algvec(c2s[0], c2s[1]) + Algvec(c2s[0], c2s[2]); */
            /* if (v2.euLength() < 1e-3) v2 = n; */
            /* if (dotProduct(v2, n) < 0) v2 = v2*(-1); */
            /* v2.Unit(); */
            coords[tf[(i+1)%4]] = v1.getToPoint(c2s[0]);
        }
        UntangleforNewCell(poly);
    }
    else if (faces.size() == 3) {
        set<uint8_t> ftmp;
        Polygon poly;
        for (auto x : faces) ftmp.insert(x);
        vector<uint8_t> pfaces = {0, 5, 1, 3, 2, 4};
        poly.polytype = 3;
        uint8_t bfnum = faces[0];
        poly.bottomface = vector<uint8_t>{fc[bfnum*4], fc[bfnum*4+3], fc[bfnum*4+2], fc[bfnum*4+1]};
        for (uint8_t i=0; i<3; ++i) {
            uint8_t f1 = pfaces[i*2], f2 = pfaces[i*2+1];
            if (ftmp.find(f1)==ftmp.end() or ftmp.find(f2)==ftmp.end()) continue;
            for (auto x : ftmp)
                if (x!=f1 and x!= f2) bfnum = x;
            poly.polytype = 4;
            poly.bottomface = vector<uint8_t>{fc[bfnum*4], fc[bfnum*4+3], fc[bfnum*4+2], fc[bfnum*4+1]};
        }
        map<uint8_t, uint8_t> pos;//底面每个点在底面中的位置
        for (uint8_t i=0; i<4; ++i)
            pos.insert(make_pair(poly.bottomface[i], i));
        for (uint8_t i=0; i<faces.size(); ++i) {
            if (faces[i] == bfnum) continue;
            vector<uint8_t> face{fc[faces[i]*4], fc[faces[i]*4+3], fc[faces[i]*4+2], fc[faces[i]*4+1]};
            for (uint8_t i=0; i<4; ++i) {
                if (pos.find(face[i])!=pos.end() and pos.find(face[(i+1)%4])!=pos.end()) {
                    uint8_t pos1 = pos[face[i]];
                    uint8_t pos2 = (pos[face[i]]+1)%4;
                    uint8_t pos3 = pos[face[(i+1)%4]];
                    uint8_t pos4 = (pos[face[(i+1)%4]]+3)%4;
                    if (poly.otherpoints.find(face[(i+3)%4]) == poly.otherpoints.end())
                        poly.otherpoints.insert(make_pair(face[(i+3)%4], vector<uint8_t>{poly.bottomface[pos1]}));
                    poly.otherpoints[face[(i+3)%4]].push_back(poly.bottomface[pos2]);
                    if (poly.otherpoints.find(face[(i+2)%4]) == poly.otherpoints.end())
                        poly.otherpoints.insert(make_pair(face[(i+2)%4], vector<uint8_t>{poly.bottomface[pos3]}));
                    poly.otherpoints[face[(i+2)%4]].push_back(poly.bottomface[pos4]);
                }
            }
        }
        if (poly.polytype == 3) {
            vector<uint8_t> tf{fc[bfnum*4+24], fc[bfnum*4+3+24], fc[bfnum*4+2+24], fc[bfnum*4+1+24]};
            uint8_t np = 0;
            for (uint8_t i=0; i<4; ++i) {
                if (poly.otherpoints.find(tf[i]) == poly.otherpoints.end()) {
                    np = i;
                    break;
                }
            }
            vector<uint8_t> bf(poly.bottomface);
            uint8_t p0 = bf[(np+2)%4], p1 = tf[(np+2)%4], p2 = bf[(np+3)%4], p3 = bf[(np+1)%4];
            Coord cs[4] = {coords[p0], coords[p1], coords[p2], coords[p3]};
            Algvec v;
            v.dx = 0, v.dy = 0, v.dz = 0;
            for (uint8_t i=0; i<3; ++i) {
                Algvec v1(cs[0], cs[i+1]);
                Algvec v2(cs[0], cs[(i+1)%3+1]);
                v = v + crossProduct(v1, v2);
            }
            v.Unit();
            v = v*1.732;
            coords[tf[np]] = v.getToPoint(cs[0]);
        }
        UntangleforNewCell(poly);
    }
    else if (faces.size() == 4) {
        set<uint8_t> ftmp;
        Polygon poly;
        uint8_t bfnum;
        for (auto x : faces) ftmp.insert(x);
        vector<uint8_t> pfaces = {0, 5, 1, 3, 2, 4};
        poly.polytype = 5;
        for (uint8_t i=0; i<4; ++i) {
            uint8_t f1 = pfaces[i*2], f2 = pfaces[i*2+1];
            if (ftmp.find(f1)==ftmp.end() or ftmp.find(f2)==ftmp.end()) continue;
            for (auto x : ftmp)
                if (x!=f1 and x!= f2) bfnum = x;
            poly.bottomface = vector<uint8_t>{fc[bfnum*4], fc[bfnum*4+3], fc[bfnum*4+2], fc[bfnum*4+1]};
        }
        map<uint8_t, uint8_t> pos;//底面每个点在底面中的位置
        for (uint8_t i=0; i<4; ++i)
            pos.insert(make_pair(poly.bottomface[i], i));
        for (uint8_t i=0; i<faces.size(); ++i) {
            if (faces[i] == bfnum) continue;
            vector<uint8_t> face{fc[faces[i]*4], fc[faces[i]*4+3], fc[faces[i]*4+2], fc[faces[i]*4+1]};
            for (uint8_t i=0; i<4; ++i) {
                if (pos.find(face[i])!=pos.end() and pos.find(face[(i+1)%4])!=pos.end()) {
                    uint8_t pos1 = pos[face[i]];
                    uint8_t pos2 = (pos[face[i]]+1)%4;
                    uint8_t pos3 = pos[face[(i+1)%4]];
                    uint8_t pos4 = (pos[face[(i+1)%4]]+3)%4;
                    if (poly.otherpoints.find(face[(i+3)%4]) == poly.otherpoints.end())
                        poly.otherpoints.insert(make_pair(face[(i+3)%4], vector<uint8_t>{poly.bottomface[pos1]}));
                    poly.otherpoints[face[(i+3)%4]].push_back(poly.bottomface[pos2]);
                    if (poly.otherpoints.find(face[(i+2)%4]) == poly.otherpoints.end())
                        poly.otherpoints.insert(make_pair(face[(i+2)%4], vector<uint8_t>{poly.bottomface[pos3]}));
                    poly.otherpoints[face[(i+2)%4]].push_back(poly.bottomface[pos4]);
                }
            }
        }
        UntangleforNewCell(poly);
    }
    else if (faces.size() == 5) {
        set<uint8_t> ftmp;
        Polygon poly;
        uint8_t bfnum;
        for (auto x : faces) ftmp.insert(x);
        vector<uint8_t> pfaces = {0, 5, 1, 3, 2, 4};
        poly.polytype = 6;
        for (uint8_t i=0; i<5; ++i) {
            uint8_t f1 = pfaces[i*2], f2 = pfaces[i*2+1];
            if (ftmp.find(f1)!=ftmp.end() and ftmp.find(f2)!=ftmp.end()) continue;
            if (ftmp.find(f1) == ftmp.end()) bfnum = f1;
            else if (ftmp.find(f2) == ftmp.end()) bfnum = f2;
            poly.bottomface = vector<uint8_t>{fc[bfnum*4], fc[bfnum*4+3], fc[bfnum*4+2], fc[bfnum*4+1]};
        }
        map<uint8_t, uint8_t> pos;//底面每个点在底面中的位置
        for (uint8_t i=0; i<4; ++i)
            pos.insert(make_pair(poly.bottomface[i], i));
        for (uint8_t i=0; i<faces.size(); ++i) {
            if (faces[i] == bfnum) continue;
            vector<uint8_t> face{fc[faces[i]*4], fc[faces[i]*4+3], fc[faces[i]*4+2], fc[faces[i]*4+1]};
            for (uint8_t i=0; i<4; ++i) {
                if (pos.find(face[i])!=pos.end() and pos.find(face[(i+1)%4])!=pos.end()) {
                    uint8_t pos1 = pos[face[i]];
                    uint8_t pos2 = (pos[face[i]]+1)%4;
                    uint8_t pos3 = pos[face[(i+1)%4]];
                    uint8_t pos4 = (pos[face[(i+1)%4]]+3)%4;
                    if (poly.otherpoints.find(face[(i+3)%4]) == poly.otherpoints.end())
                        poly.otherpoints.insert(make_pair(face[(i+3)%4], vector<uint8_t>{poly.bottomface[pos1]}));
                    poly.otherpoints[face[(i+3)%4]].push_back(poly.bottomface[pos2]);
                    if (poly.otherpoints.find(face[(i+2)%4]) == poly.otherpoints.end())
                        poly.otherpoints.insert(make_pair(face[(i+2)%4], vector<uint8_t>{poly.bottomface[pos3]}));
                    poly.otherpoints[face[(i+2)%4]].push_back(poly.bottomface[pos4]);
                }
            }
        }
        UntangleforNewCell(poly);
    }
    set<uint8_t> f0 = {cell[0], cell[1], cell[2], cell[3]};
    set<uint8_t> f1 = {cell[0], cell[1], cell[5], cell[4]};
    set<uint8_t> f2 = {cell[1], cell[2], cell[6], cell[5]};
    set<uint8_t> f3 = {cell[2], cell[3], cell[7], cell[6]};
    set<uint8_t> f4 = {cell[3], cell[0], cell[4], cell[7]};
    set<uint8_t> f5 = {cell[4], cell[5], cell[6], cell[7]};
    initedfaces.insert(f0);
    initedfaces.insert(f1);
    initedfaces.insert(f2);
    initedfaces.insert(f3);
    initedfaces.insert(f4);
    initedfaces.insert(f5);
    chosenC += 1;
    for (auto x : cell)
        chosenV.insert(x);
}


vector<uint8_t> fullcell(vector<uint8_t> oc) {
    vector<uint8_t> res;
    res.push_back(oc[0]);
    res.push_back(oc[3]);
    res.push_back(oc[2]);
    res.push_back(oc[1]);
    res.push_back(oc[0]);
    res.push_back(oc[1]);
    res.push_back(oc[5]);
    res.push_back(oc[4]);
    res.push_back(oc[1]);
    res.push_back(oc[2]);
    res.push_back(oc[6]);
    res.push_back(oc[5]);
    res.push_back(oc[2]);
    res.push_back(oc[3]);
    res.push_back(oc[7]);
    res.push_back(oc[6]);
    res.push_back(oc[3]);
    res.push_back(oc[0]);
    res.push_back(oc[4]);
    res.push_back(oc[7]);
    res.push_back(oc[4]);
    res.push_back(oc[5]);
    res.push_back(oc[6]);
    res.push_back(oc[7]);
    //另一组，第一组的对面
    res.push_back(oc[4]);
    res.push_back(oc[7]);
    res.push_back(oc[6]);
    res.push_back(oc[5]);
    res.push_back(oc[3]);
    res.push_back(oc[2]);
    res.push_back(oc[6]);
    res.push_back(oc[7]);
    res.push_back(oc[0]);
    res.push_back(oc[3]);
    res.push_back(oc[7]);
    res.push_back(oc[4]);
    res.push_back(oc[1]);
    res.push_back(oc[0]);
    res.push_back(oc[4]);
    res.push_back(oc[5]);
    res.push_back(oc[2]);
    res.push_back(oc[1]);
    res.push_back(oc[5]);
    res.push_back(oc[6]);
    res.push_back(oc[0]);
    res.push_back(oc[1]);
    res.push_back(oc[2]);
    res.push_back(oc[3]);
    return res;
}

void Modu::UntangleforNewCell(Polygon poly) {
    Mesquite::MsqError err;
    Algvec n1, n2, n;
    Coord bfc[4] = {coords[poly.bottomface[0]], coords[poly.bottomface[1]], 
                    coords[poly.bottomface[2]], coords[poly.bottomface[3]]};
    n1 = crossProduct(Algvec(bfc[0], bfc[1]), Algvec(bfc[1], bfc[2]));
    n2 = crossProduct(Algvec(bfc[2], bfc[3]), Algvec(bfc[3], bfc[0]));
    n = n1 + n2;
    n.Unit();
    for (auto x : poly.otherpoints) {
        Coord p = coords[x.first];
        if (dotProduct(n, Algvec(p, bfc[0])) < 0) continue;
        //Coord target1 = ProjecttoPlane(p, bfc[0], n);
        Coord target2 = FinePoint(x.first, poly);
        Algvec ln = n*0.1;
        Coord target3 = ln.getToPoint(target2);
        //Coord base = coords[x.second[0]];
        //if (dotProduct(Algvec(base, target2), Algvec(target1, target2)) < 0)
            //coords[x.first] = target1;
        //else
            //coords[x.first] = target2;
        coords[x.first] = target3;
    }
}

Coord Modu::ProjecttoPlane(Coord s, Coord b, Algvec n) {
    double para = 1.1;
    double x0, y0, z0, x1, y1, z1, i, j, k;
    x0=b.x, y0=b.y, z0=b.z;
    x1=s.x, y1=s.y, z1=s.z, i=n.dx, j=n.dy, k=n.dz;
    auto l = i*x0+j*y0+k*z0-i*x1-j*y1-k*z1;
    l = l/(i*i+j*j+k*k);
    Coord res{x1+l*i*para, y1+l*j*para, z1+l*k*para};
    return res;
}

Coord Modu::FinePoint(uint8_t p, Polygon poly) {
    Coord base = coords[poly.otherpoints[p][0]];
    Coord p1 = coords[poly.otherpoints[p][1]];
    Coord p2 = base;
    if (poly.otherpoints[p].size() == 3)
        p2 = coords[poly.otherpoints[p][2]];
    Algvec v = Algvec(p1, base) + Algvec(p2, base);
    v.Unit();
    Algvec u(base, p1);
    v = v*u.euLength();
    return v.getToPoint(base);
}

