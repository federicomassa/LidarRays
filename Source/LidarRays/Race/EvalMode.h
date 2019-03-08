
#ifndef EVAL_MODE_H
#define EVAL_MODE_H

#include <iostream>



// Evaluation mode
enum EvalMode { OR, NOR, SINGLE, NSINGLE };

std::ostream& operator<<(std::ostream&, EvalMode);

#endif
