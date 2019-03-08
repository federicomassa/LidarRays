
#ifndef EXT_VALUE_H
#define EXT_VALUE_H

#include <iostream>

#include "EvalMode.h"

// Extended boolean (false, uncertain, true)
enum ExtBool { F, U, T };

// Extended value: { bool, extBool }
class ExtValue
{
public:
    bool omniscientValue;
    ExtBool nonOmniscientValue;
    /* constructor */
    ExtValue(bool oV = false, ExtBool nOV = F)
    {
        omniscientValue = oV;
        nonOmniscientValue = nOV;
    }
    ExtValue(const ExtValue& ev)
      {
	omniscientValue = ev.omniscientValue;
	nonOmniscientValue = ev.nonOmniscientValue;
      }
    
    /* destructor */
    ~ExtValue() { }
    /* ``extend'' the value depending on evaluation mode (cfr. doc) */
    void extendValue(EvalMode);
    /* reset the value to (true, true) or (false, false) */
    void reset(bool v = false)
    {
        omniscientValue = v;
        nonOmniscientValue = v ? T : F;
    }
};

ExtValue operator&&(const ExtValue&, const ExtValue&);
ExtValue operator||(const ExtValue&, const ExtValue&);
ExtValue operator!(const ExtValue&);
std::ostream& operator<<(std::ostream&, const ExtValue&);
std::ostream& operator<<(std::ostream&, const ExtBool&);

ExtBool operator!(const ExtBool&);
ExtBool operator&&(const ExtBool&, const ExtBool&);
ExtBool operator||(const ExtBool&, const ExtBool&);
#endif
