#include "resource.h"
#include "mesh.h"

int main(int argc, char** argv) {
    /* if (argc != 2) { */
    /*     cout << "arg num must be 1." << endl; */
    /*     return 1; */
    /* } */
    Mesh mymesh;
    string a("./6_tmp.vtk");
    mymesh.ReadVtk(a);
    mymesh.BuildGeom();
    string filename(a, 0, a.size()-4);
    filename += "_opt.vtk";
    mymesh.WriteVtk(filename);
    return 0;
}
