#include "resource.h"
#include "modu.h"
#include "hexmesh.h"

void work(string filename, string mode) {
    vector<uint8_t> ar, arl;
    ReadModu(ar, arl, filename, mode);
    int cellnum = arl.size()/24;
    Hexmesh mesh;
    mesh.addFirstCell(vector<uint8_t>(ar.begin(), ar.begin()+8));
    for (int i=1; i<cellnum; ++i) {
        mesh.addCell(vector<uint8_t>(ar.begin()+i*8, ar.begin()+i*8+8), 
                     vector<uint8_t>(arl.begin()+i*24, arl.begin()+i*24+24));
    }
    ofstream fo;
    fo.open("modu.vtk");
    mesh.vtkWrite(fo);
    //预定output
}

int main(int argc, char** argv) {
    if (argc != 3) {
        cout << "输入的参数个数错误。\n";
        return 0;
    }
    string filename(argv[1]), mode(argv[2]);
    work(filename, mode);
    return 0;
}
