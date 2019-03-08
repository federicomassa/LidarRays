
#include "EvalMode.h"

using namespace std;

ostream& operator<<(ostream& os, EvalMode mode)
{
    switch(mode)
    {
        case OR:
            os << "OR";
            break;
        case NOR:
            os << "NOR";
            break;
        case SINGLE:
            os << "SINGLE";
            break;
        case NSINGLE:
            os << "NSINGLE";
            break;
    }
    
    return os;
}
