#include "modu.h"

Modu::Modu() {}
Modu::~Modu() {}

void ReadModu(vector<uint8_t> &raw, vector<uint8_t> &rawlong, string filename, string mode) {
    FILE *fp;
    fp = fopen(filename.c_str(), "r");
    uint32_t toponum;
    if (mode == "random") {
        srand(time(NULL));
        toponum = rand();
    }
    else
        toponum = stoi(mode);
    char buffer[256];
    while (!feof(fp)) {
        fgets(buffer, 256, fp);
        uint32_t tn = stoi(string(buffer));
        if (feof(fp)) return;
        if (tn != toponum) {
            do {
                fgets(buffer, 256, fp);
            } while (string(buffer).size() > 1);
            continue;
        }
        do {
            vector<uint8_t> r1(0), rl1(24);
            fgets(buffer, 256, fp);
            if (string(buffer).size() <= 1)
                break;
            uint32_t tmp[24];
            sscanf(buffer, "%d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d", 
                   &tmp[0], &tmp[1], &tmp[2], &tmp[3], &tmp[4], &tmp[5], &tmp[6],
                   &tmp[7], &tmp[8], &tmp[9], &tmp[10], &tmp[11], &tmp[12], &tmp[13],
                   &tmp[14], &tmp[15], &tmp[16], &tmp[17], &tmp[18], &tmp[19], &tmp[20],
                   &tmp[21], &tmp[22], &tmp[23]);
            for (int i=0; i<24; ++i) {
                rl1[i] = tmp[i];
            }
            r1.insert(r1.end(), rl1.begin(), rl1.begin()+4);
            r1.insert(r1.end(), rl1.begin()+20, rl1.begin()+24);
            auto tmpa = r1[7];
            r1[7] = r1[6];
            r1[6] = tmpa;
            tmpa = r1[5];
            r1[5] = r1[4];
            r1[4] = tmpa;
            raw.insert(raw.end(), r1.begin(), r1.end());
            rawlong.insert(rawlong.end(), rl1.begin(), rl1.end());
            /*
            for (uint32_t i=0; i<6; ++i) {
                for (uint32_t j=0; j<4; ++j) {
                    modu._vertexset.insert(tmp[i*4+j]);
                    auto it = modu._vertexset.find(tmp[i*4+j]);
                    if (modu._nbhpoints.find(it) == modu._nbhpoints.end())
                        modu._nbhpoints.insert(make_pair(it, set<uint8_t>()));
                    modu._nbhpoints[it].insert(tmp[i*4+(j+1)%4]);
                    modu._nbhpoints[it].insert(tmp[i*4+(j+3)%4]);
                }
            }
            for (uint32_t i=0; i<6; ++i) {
                for (uint32_t j=0; j<4; ++j) {
                    set<uint8_t> tmpset;
                    tmpset.insert(tmp[i*4+j]);
                    tmpset.insert(tmp[i*4+(j+1)%4]);
                    modu._edgeset.insert(tmpset);
                    auto it = modu._edgeset.find(tmpset);
                    if (modu._dihedral.find(it) == modu._dihedral.end())
                        modu._dihedral.insert(make_pair(it, vector<uint8_t>(0)));
                    modu._dihedral[it].push_back(tmp[i*4+(j+2)%4]);
                    modu._dihedral[it].push_back(tmp[i*4+(j+3)%4]);
                    //这里明天重写
                }
                set<uint8_t> tmpset;
                for (uint32_t j=0; j<4; ++j) {
                    tmpset.insert(tmp[i*4+j]);
                }
                modu._faceset.insert(tmpset);
                auto it = modu._faceset.find(tmpset);
                if (modu._diagonal.find(it) == modu._diagonal.end()) {
                    modu._diagonal.insert(make_pair(it, vector<uint8_t>(0)));
                    modu._diagonal[it].push_back(tmp[i*4]);
                    modu._diagonal[it].push_back(tmp[i*4+1]);
                    modu._diagonal[it].push_back(tmp[i*4+2]);
                    modu._diagonal[it].push_back(tmp[i*4+3]);
                }
            }
            if (modu.fixedpoints.size() == 0) {
                modu.fixedpoints.push_back(tmp[0]);
                modu.fixedpoints.push_back(tmp[1]);
                modu.fixedpoints.push_back(tmp[3]);
                modu.fixedpoints.push_back(tmp[7]);
            }
            */
        } while (string(buffer).size() > 1);
        //modu.vn = modu._vertexset.size();
        //modu.en = modu._edgeset.size();
        //modu.fn = modu._faceset.size();
        break;
    }
}
