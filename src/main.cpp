#include "modu.h"
#include "msq.h"

int main(int argc, char** argv) {
    Modu modu;
    modu.ReadTopo(string(argv[1]));
    modu.AddNewcell();
    int flag = 0;
    while (modu.cnum != modu.chosenC) {
        if (flag == 0)
            modu.SaveCache();
        modu.AddNewcell();
        modu.VtkOut();
        QualityImprover q(modu.chosenC);
        if(q.ExecWrapper() == 1) {
            if (modu.newcelltype != 4)
                return 0;
            modu.LoadCache();
            for (int i=0; i<5; ++i) {
                modu.LoadCache();
                modu.TryUntangle(1.0/(i+1));
                modu.VtkOut(modu.newcellpoints);//固定
                QualityImprover q1(modu.chosenC);
                if (q1.ExecWrapper() == 2)
                    break;
                if (i == 4)
                    return 0;
            }
            flag = 1;
        }
        else
            flag = 0;
        modu.VtkIn();
    }
    return 0;
}
