#DEFINE precision 2 // number of decimal places 

#include math.h

/* This file outlines function to perform with fixed-point arithmetic on financial data
   for precise outputs/results. Include decimal precision AS AN ARGUMENT. Built to deal
   with financial data.  
*/

/* These should probably also all be defined to be inline functions since they 
   could be embedded deep in a large loop. */

typedef long fp; 

inline long fixed_to_reg(fixed_point num);
inline fixed_point reg_to_fixed(long num);

/* arithmetic. for subtraction just multiply by -1 to avoid confusion. */
inline fixed_point add(fixed_point num1, fixed_point num2);
inline fixed_point mult(fixed_point num1, fixed_point num2);
inline fixed_point div(fixed_point num1, fixed_point num2);





