
#include "ExtValue.h"

/**
 * COMMENT
 */

using namespace std;

void ExtValue::extendValue(EvalMode mode)
{
    switch(mode)
    {
        case NOR:
            if(omniscientValue)
                nonOmniscientValue = U;
            else
                nonOmniscientValue = F;
            break;
        case OR:
            if (omniscientValue)
                nonOmniscientValue = T;
            else
                nonOmniscientValue = U;
            break;
        case SINGLE:
        case NSINGLE:
            nonOmniscientValue = (omniscientValue == true) ? T : F;
            break;
    }
}

ExtValue operator&&(const ExtValue& v1, const ExtValue& v2)
{
    ExtValue newVal;

    /* default logic and */
    newVal.omniscientValue = v1.omniscientValue && v2.omniscientValue;

    /* and between ExtBool */
    switch(v1.nonOmniscientValue)
    {
        case T:
            newVal.nonOmniscientValue = v2.nonOmniscientValue;
            break;
        case U:
            if(v2.nonOmniscientValue == T)
                newVal.nonOmniscientValue = U;
            else
                newVal.nonOmniscientValue = v2.nonOmniscientValue;
            break;
        case F:
            newVal.nonOmniscientValue = F;
            break;
    }

    return newVal;
}

ExtValue operator||(const ExtValue &v1, const ExtValue &v2)
{
    ExtValue newVal;
    
    /* default logic or */
    newVal.omniscientValue = v1.omniscientValue || v2.omniscientValue;
    
    /* or between ExtBool */
    switch (v1.nonOmniscientValue)
    {
        case T:
            newVal.nonOmniscientValue = T;
            break;
        case U:
            if(v2.nonOmniscientValue == F)
                newVal.nonOmniscientValue = U;
            else
                newVal.nonOmniscientValue = v2.nonOmniscientValue;
            break;
        case F:
            newVal.nonOmniscientValue = v2.nonOmniscientValue;
            break;
    }

    return newVal;
}

ExtValue operator!(const ExtValue& v)
{
  ExtValue newVal;

  newVal.omniscientValue = !v.omniscientValue;

  switch (v.nonOmniscientValue)
    {
    case T:
      newVal.nonOmniscientValue = F;
      break;
    case U:
      newVal.nonOmniscientValue = U;
      break;
    case F:
      newVal.nonOmniscientValue = T;
      break;
    }

  return newVal;
}

ostream& operator<<(ostream& os, const ExtValue& value)
{
    os << "(";
    if(value.omniscientValue)
        os << "true";
    else
        os << "false";
    os << ", ";
    switch(value.nonOmniscientValue)
    {
        case T:
            os << "true";
            break;
        case F:
            os << "false";
            break;
        case U:
            os << "uncertain";
            break;
    }
    os << ")";
    
    return os;
}

ostream& operator<<(ostream& os, const ExtBool& value)
{
    os << "(";
    switch(value)
    {
        case T:
            os << "true";
            break;
        case F:
            os << "false";
            break;
        case U:
            os << "uncertain";
            break;
    }
    os << ")";
    
    return os;
}


ExtBool operator!(const ExtBool& b)
{
  switch(b)
    {
    case T:
      return F;
      break;
    case U:
      return U;
      break;
    case F:
      return T;
      break;
    }

  return U;
}


ExtBool operator&&(const ExtBool& b1, const ExtBool& b2)
{
  ExtBool b = F;
  
  /* and between ExtBool */
  switch(b1)
    {
    case T:
      b = b2;
      break;
    case U:
      if(b2 == T)
	b = U;
      else
	b = b2;
      break;
    case F:
      b = F;
      break;
    }
  
  return b;
}

ExtBool operator||(const ExtBool& b1, const ExtBool& b2)
{
  ExtBool b = F;
  
  /* and between ExtBool */
  switch(b1)
    {
    case T:
      b = T;
      break;
    case U:
      if(b2 == T)
	b = T;
      else
	b = U;
      break;
    case F:
      b = b2;
      break;
    }
  
  return b;
}

