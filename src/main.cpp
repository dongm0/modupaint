#include "modu.h"
#include "msq.h"

int main(int argc, char** argv) {
    Modu modu;
    modu.ReadTopo(string(argv[1]));
    while (modu.cnum != modu.chosenC) {
        modu.AddNewcell();
        modu.VtkOut();
        QualityImprover q(modu.chosenC);
        if(!q.exec()) {
            cout << "msquite error\n";
            return 0;
        }
        modu.VtkIn();
    }
    return 0;
}
