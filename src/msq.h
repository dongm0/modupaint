#ifndef MSQ_H
#define MSQ_H
#include "resource.h"

class QualityImprover {
public:
    uint8_t cellnum;

    QualityImprover(uint8_t c);
    ~QualityImprover();
    uint8_t ExecWrapper();
    uint8_t isTangledWrapper();

private:
    uint8_t exec(Mesquite::MeshImpl, Mesquite::MsqError);
    uint8_t isTangled(Mesquite::MeshImpl, Mesquite::MsqError);
};

bool Untangle(int cellnum);

#endif
