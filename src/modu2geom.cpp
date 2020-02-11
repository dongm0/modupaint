#include "resource.h"
#include "mesh.h"

int main() {
    Mesh mymesh;
    string a("");
    mymesh.ReadVtk(a);
    mymesh.BuildGeom();
    mymesh.WriteVtk(a);
    return 0;
}