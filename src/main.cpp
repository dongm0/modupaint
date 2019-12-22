#include "modu.h"
#include "msq.h"

struct resStat {
    /* int moduid; */
    int cellnum;
    bool success;
    int failat;//第？个单元失败
    int failnewcelltype;
    int failcase;//0是优化过一遍没有用，1是untangle失败，2是celltype为1
};

resStat work(string filename) {
    Modu modu;
    modu.ReadTopo(filename);
    modu.AddNewcell();
    int flag = 0;
    while (modu.cnum != modu.chosenC) {
        if (flag == 0)
            modu.SaveCache();
        modu.AddNewcell();
        modu.VtkOut();
        QualityImprover q(modu.chosenC);
        if(q.ExecWrapper() == 1) {
            if (flag == 1)
                return resStat{modu.cnum, false, modu.chosenC, modu.newcelltype, 0};
            if (modu.newcelltype == 1)
                return resStat{modu.cnum, false, modu.chosenC, modu.newcelltype, 2};
            modu.LoadCache();
            for (int i=0; i<5; ++i) {
                modu.LoadCache();
                modu.TryUntangle(1.0/(i+1));
                modu.VtkOut(modu.newcellpoints);//固定
                QualityImprover q1(modu.chosenC);
                if (q1.ExecWrapper() == 2)
                    break;
                if (i == 4)
                    return resStat{modu.cnum, false, modu.chosenC, modu.newcelltype, 1};
            }
            flag = 1;
        }
        else
            flag = 0;
        modu.VtkIn();
    }
    return resStat{modu.cnum, true, -1, -1, -1};
}

int main(int argc, char** argv) {
    ofstream fout;
    int maxcellnum = 11, maxmoduid = 145400;
    fout.open(to_string(maxcellnum) + "_modu_check_result.txt");
    string filename("../meshfile/modu/"+to_string(maxcellnum)+"_raw/");
    for (int i=1; i<maxmoduid; ++i) {
        string f = filename + to_string(maxcellnum) + "_raw.txt_" + to_string(i);
        auto ans = work(f);
        fout << i << " " << ans.cellnum << " ";
        fout << ans.success << " " << ans.failat << " ";
        fout << ans.failnewcelltype << " " << ans.failcase << endl;
        if (ans.success == true) {
            string celln(to_string(ans.cellnum));
            string moduid(to_string(i));//这里的moduid与modugenerate的topoid不是同一个东西
            string cmd1;
            cmd1 = "cp ./vtk/" + celln + "_tmp_opt.vtk ./result/" + moduid + "_" + celln + ".vtk";
            system(cmd1.c_str());
        }
        if (i%100 == 0)
            cout << i << endl;
    }
    fout.close();
    return 0;
}
