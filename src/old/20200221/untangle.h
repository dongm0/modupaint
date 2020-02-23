#include "resource.h"
#include "modu.h"

class Polygon {
    //我想不出更好的名字了，这个名字是指新的六面体加入的时候可能的截面形状，
    //一共6种
public:
    uint32_t facenum;
    vector<uint32_t> faces;//每个面右手向外
    map<uint32_t, vector<double> > coords;
};

pair<uint8_t, Polygon> is_tangled(Modu &modu);
pair<uint8_t, Polygon> untangle(Modu*, vector<set<uint8_t> >, vector<uint8_t>);
uint8_t findcentface(vector<uint8_t>);
Coord movetoplane(Coord, Algvec, Coord);
