#include "modu.h"
#include "msq.h"




int main(int argc, char** argv) {
    Modu modu;
    modu.ReadTopo(string(argv[1]));
    modu.AddNewcell();
    while (modu.cnum != modu.chosenC) {
        modu.SaveCache();
        modu.AddNewcell();
        modu.VtkOut();
        QualityImprover q(modu.chosenC);
        if(q.ExecWrapper() == 1) {
            modu.LoadCache();
        }
        modu.VtkIn();
    }
    return 0;
}
