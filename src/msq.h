#include "resource.h"

class QualityImprover {
public:
    uint8_t cellnum;

    QualityImprover(uint8_t c);
    ~QualityImprover();
    uint8_t exec_output();

private:
    uint8_t exec();

};

bool Untangle(int cellnum);
