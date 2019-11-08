#include <Mesquite_all_headers.hpp>
using namespace Mesquite;
int main() {
    MsqError err;
    MeshImpl mymesh;
    mymesh.read_vtk("tmp4.vtk", err);
    UntangleWrapper untangle;
    ShapeImprovementWrapper shape;
    untangle.run_instructions(&mymesh, err);
    shape.run_instructions(&mymesh, err);
    mymesh.write_vtk("tmp5.vtk", err);
    return 0;
}
