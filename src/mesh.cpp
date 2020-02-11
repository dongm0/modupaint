#include "mesh.h"

Polygon::Polygon() {}
Polygon::~Polygon() {}

Mesh::Mesh() {}
Mesh::~Mesh() {}
void Mesh::ReadVtk(string filename) {
    Mesquite::MsqError err;
    msqmesh.read_vtk(filename.c_str(), err);
    vector<Mesquite::Mesh::VertexHandle> vhs;
    msqmesh.get_all_vertices(vhs, err);
    for (uint8_t i=0; i<vhs.size(); ++i)
        vertices.insert(make_pair(i, vhs[i]));
}
void Mesh::WriteVtk(string filename) {
    Mesquite::MsqError err;
    msqmesh.write_vtk(filename.c_str(), err);
}
void Mesh::BuildGeom() {
    Mesquite::MsqError err;
    vector<Mesquite::Mesh::ElementHandle> ehs;
    msqmesh.get_all_elements(ehs, err);
    uint8_t elenum = ehs.size();
    for (uint8_t i=0; i<elenum; ++i) {
        InitCellpos(i);
        Optimize_local(i);
    }
    Optimize_global();
}
void Mesh::InitCellpos(uint8_t c) {
    Mesquite::MsqError err;
    vector<uint8_t> cell = cells[c];
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
        msqmesh.vertex_set_coordinates(vertices[0], Mesquite::Vector3D(0, 0, 0), err);
        msqmesh.vertex_set_coordinates(vertices[1], Mesquite::Vector3D(1, 0, 0), err);
        msqmesh.vertex_set_coordinates(vertices[2], Mesquite::Vector3D(1, 1, 0), err);
        msqmesh.vertex_set_coordinates(vertices[3], Mesquite::Vector3D(0, 1, 0), err);
        msqmesh.vertex_set_coordinates(vertices[4], Mesquite::Vector3D(0, 0, 1), err);
        msqmesh.vertex_set_coordinates(vertices[5], Mesquite::Vector3D(1, 0, 1), err);
        msqmesh.vertex_set_coordinates(vertices[6], Mesquite::Vector3D(1, 1, 1), err);
        msqmesh.vertex_set_coordinates(vertices[7], Mesquite::Vector3D(0, 1, 1), err);
    }
    else if (faces.size() == 1) {
        Polygon poly;
        poly.polytype = 1;
        uint8_t bfnum = faces[0];
        poly.bottomface = vector<uint8_t>{fc[bfnum*4], fc[bfnum*4+3], fc[bfnum*4+2], fc[bfnum*4+1]};
        vector<uint8_t> bf = poly.bottomface;
        vector<uint8_t> tf{fc[bfnum*4+24], fc[bfnum*4+3+24], fc[bfnum*4+2+24], fc[bfnum*4+1+24]};
        Mesquite::MsqVertex cs[4];
        Mesquite::Mesh::VertexHandle vs[4] = {vertices[bf[0]], vertices[bf[1]], vertices[bf[2]], vertices[bf[3]]};
        msqmesh.vertices_get_coordinates(vs, &cs[0], 4, err);
        for (uint8_t i=0; i<4; ++i) {
            Mesquite::Vector3D v1 = cs[i] - cs[(i+3)%4];
            Mesquite::Vector3D v2 = cs[(i+1)%4] - cs[i];
            Mesquite::Vector3D vn = v1*v2;
            vn.normalize();
            Mesquite::Vector3D v3 = vn - cs[i];
            msqmesh.vertex_set_coordinates(vertices[tf[i]], v3, err);
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
        for (uint8_t i=0; i<faces.size()-1; ++i) {
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
                    poly.otherpoints[face[(i+3)%4]].push_back(poly.bottomface[pos4]);
                }
            }
        }
        auto bf = poly.bottomface;
        for (uint8_t i=0; i<4; ++i) {//新的点定位置
            if (poly.otherpoints.find(tf[i])!=poly.otherpoints.end() or poly.otherpoints.find(tf[(i+1)%4])!=poly.otherpoints.end()) continue;
            Mesquite::Mesh::VertexHandle p1s[3] = {vertices[bf[(i+3)%4]], vertices[bf[i]], vertices[tf[(i+3)%4]]};
            Mesquite::Mesh::VertexHandle p2s[3] = {vertices[bf[(i+2)%4]], vertices[bf[i+1]], vertices[tf[(i+2)%4]]};
            Mesquite::MsqVertex c1s[3];
            Mesquite::MsqVertex c2s[3];
            msqmesh.vertices_get_coordinates(p1s, c1s, 3, err);
            msqmesh.vertices_get_coordinates(p2s, c2s, 3, err);
            Mesquite::Vector3D v1 = (c1s[1]-c1s[0]) + (c1s[2]-c1s[0]);
            v1.normalize();
            Mesquite::Vector3D n = (c1s[1]-c1s[0])*(c2s[1]-c1s[1]);
            if (v1%n < 0) v1 = v1*(-1);
            Mesquite::Vector3D p1 = v1 - c1s[0];
            msqmesh.vertex_set_coordinates(vertices[tf[i]], p1, err);
            Mesquite::Vector3D v2 = (c2s[1]-c2s[0]) + (c2s[2]-c2s[0]);
            v2.normalize();
            if (v2%n < 0) v2 = v2*(-1);
            Mesquite::Vector3D p2 = v2 - c2s[0];
            msqmesh.vertex_set_coordinates(vertices[tf[(i+1)%4]], p2, err);
        }
        UntangleforNewCell(poly);
    }
    else if (faces.size() == 3) {
        set<uint8_t> ftmp;
        Polygon poly;
        for (auto x : faces) ftmp.insert(x);
        vector<uint8_t> pfaces = {0, 5, 1, 3, 2, 4};
        poly.polytype = 4;
        uint8_t bfnum = faces[0];
        poly.bottomface = vector<uint8_t>{fc[bfnum*4], fc[bfnum*4+3], fc[bfnum*4+2], fc[bfnum*4+1]};
        for (uint8_t i=0; i<3; ++i) {
            uint8_t f1 = pfaces[i*2], f2 = pfaces[i*2+1];
            if (ftmp.find(f1)==ftmp.end() or ftmp.find(f2)==ftmp.end()) continue;
            for (auto x : ftmp)
                if (x!=f1 and x!= f2) bfnum = x;
            poly.polytype = 3;
            poly.bottomface = vector<uint8_t>{fc[bfnum*4], fc[bfnum*4+3], fc[bfnum*4+2], fc[bfnum*4+1]};
        }
        map<uint8_t, uint8_t> pos;//底面每个点在底面中的位置
        for (uint8_t i=0; i<4; ++i)
            pos.insert(make_pair(poly.bottomface[i], i));
        for (uint8_t i=0; i<faces.size()-1; ++i) {
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
                    poly.otherpoints[face[(i+3)%4]].push_back(poly.bottomface[pos4]);
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
            Mesquite::Mesh::VertexHandle vhs[4] = {vertices[p0], vertices[p1], vertices[p2], vertices[p3]};
            Mesquite::MsqVertex cs[4];
            msqmesh.vertices_get_coordinates(vhs, cs, 4, err);
            Mesquite::Vector3D v(0, 0, 0);
            for (uint8_t i=0; i<3; ++i) {
                Mesquite::Vector3D v1 = cs[i+1] - cs[0];
                Mesquite::Vector3D v2 = cs[(i+1)%3+1] - cs[0];
                v += v1*v2;
            }
            v.normalize();
            v *= 1.732;
            msqmesh.vertex_set_coordinates(vertices[tf[np]], v, err);
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
        for (uint8_t i=0; i<faces.size()-1; ++i) {
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
                    poly.otherpoints[face[(i+3)%4]].push_back(poly.bottomface[pos4]);
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
        for (uint8_t i=0; i<faces.size()-1; ++i) {
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
                    poly.otherpoints[face[(i+3)%4]].push_back(poly.bottomface[pos4]);
                }
            }
        }
        UntangleforNewCell(poly);
    }
}

void Mesh::UntangleforNewCell(Polygon poly) {
    Mesquite::MsqError err;
    Mesquite::Vector3D n1, n2, n;
    Mesquite::MsqVertex bfc[4];
    Mesquite::Mesh::VertexHandle bfvh[4] = {vertices[poly.bottomface[0]], 
                                      vertices[poly.bottomface[1]], 
                                      vertices[poly.bottomface[2]], 
                                      vertices[poly.bottomface[3]]};
    msqmesh.vertices_get_coordinates(bfvh, bfc, 4, err);
    n1 = (bfc[1]-bfc[0])*(bfc[2]-bfc[1]);
    n2 = (bfc[3]-bfc[2])*(bfc[0]-bfc[3]);
    n = n1 + n2;
    n.normalize();
    for (auto x : poly.otherpoints) {
        Mesquite::MsqVertex p;
        msqmesh.vertices_get_coordinates(&vertices[x.first], &p, 1, err);
        if (n%(bfc[0]-p) > 0) continue;
        Mesquite::Vector3D target1 = ProjecttoPlane(p, bfc[0], n);
        Mesquite::Vector3D target2 = FinePoint(x.first, poly);
        Mesquite::MsqVertex base;
        msqmesh.vertices_get_coordinates(&vertices[x.second[0]], &base, 1, err);
        if ((target2-base)%(target2-target1) < 0)
            msqmesh.vertex_set_coordinates(vertices[x.first], target1, err);
        else
            msqmesh.vertex_set_coordinates(vertices[x.first], target2, err);
    }
}

Mesquite::Vector3D Mesh::ProjecttoPlane(Mesquite::Vector3D s, Mesquite::Vector3D b, Mesquite::Vector3D n) {
    double para = 1;
    double x0, y0, z0, x1, y1, z1, i, j, k;
    x0=b.x(), y0=b.y(), z0=b.z();
    x1=s.x(), y1=s.y(), z1=s.z(), i=n.x(), j=n.y(), k=n.z();
    auto l = i*x0+j*y0+k*z0-i*x1-j*y1-j*z1;
    l = l/(i*i+j*j+k*k);
    Mesquite::Vector3D res(x1+l*i*para, y1+l*j*para, z1+l*k*para);
    return res;
}

Mesquite::Vector3D Mesh::FinePoint(uint8_t p, Polygon poly) {
    Mesquite::MsqError err;
    Mesquite::MsqVertex base;
    Mesquite::MsqVertex p1, p2;
    msqmesh.vertices_get_coordinates(&vertices[poly.otherpoints[p][0]], &base, 1, err);
    msqmesh.vertices_get_coordinates(&vertices[poly.otherpoints[p][1]], &p1, 1, err);
    p2 = base;
    if (poly.otherpoints[p].size() == 3)
        msqmesh.vertices_get_coordinates(&vertices[poly.otherpoints[p][2]], &p2, 1, err);
    Mesquite::Vector3D res = (base-p1) + (base-p2);
    res.normalize();
    res *= 0.1;
    return res;
}

void Mesh::Optimize_local(uint8_t cellnumber) {
    Mesquite::MsqError err;
    msqmesh.set_all_fixed_flags(true, err);
    Mesquite::Mesh::VertexHandle vert_arr[8];
    for (uint8_t i=0; i<8; ++i)
        vert_arr[i] = vertices[cells[cellnumber][i]];
    vector<bool> fix_val = {0, 0, 0, 0, 0, 0, 0, 0};
    msqmesh.vertices_set_fixed_flag(vert_arr, fix_val, 8, err);
    Mesquite::ShapeImprovementWrapper wrapper;
    Mesquite::UntangleWrapper untangle;
    untangle.run_instructions(&msqmesh, err);
    wrapper.run_instructions(&msqmesh, err);
}

void Mesh::Optimize_global() {
    Mesquite::MsqError err;
    msqmesh.set_all_fixed_flags(false, err);
    Mesquite::ShapeImprovementWrapper wrapper;
    Mesquite::UntangleWrapper untangle;
    untangle.run_instructions(&msqmesh, err);
    wrapper.run_instructions(&msqmesh, err);
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
