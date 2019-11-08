#include "resource.h"
#include "modu.h"
#include "hexmesh.h"

using namespace std;

int main(int argc, char** argv) {
    vector<uint8_t> raw, rawlong;
    ReadModu(raw, rawlong, string(argv[1]), string(argv[2]));
    Hexmesh mymesh;
    mymesh.addFirstCell(vector<uint8_t>(raw.begin(), raw.begin()+8));
    for (uint8_t i=1; i<raw.size()/8; ++i) {
        vector<uint8_t> r1(raw.begin()+i*8, raw.begin()+i*8+8),
                        rl1(rawlong.begin()+i*24, rawlong.begin()+i*24+24);
        mymesh.addCell(r1, rl1);
    }
    
    ofstream fout;
    string ofname = string(argv[1]) + "_" + string(argv[2]);
    fout.open(ofname);
    fout << (int32_t)raw.size()/8 << endl;
    for (uint8_t i=0; i<raw.size()/8; ++i) {
        for (uint8_t j=0; j<8; ++j)
            fout << (uint32_t)mymesh.structure[i*8+j] << " ";
        fout << endl;
    }
    fout.close();
    return 0;
}
