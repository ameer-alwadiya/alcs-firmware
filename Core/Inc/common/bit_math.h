/*******************************************************************************************************/
/* Author            : Ameer Alwadiya                                                                 */
/* Version           : V0.0.0                                                                          */
/* Date              : 15 July 2025                                                                    */
/* Description       : BIT_MATH.h                                                                      */
/*******************************************************************************************************/

/*******************************************************************************************************/
/*                            Guard of file will call one time in .c file                            */
/*******************************************************************************************************/
#ifndef BIT_MATH_H_
#define BIT_MATH_H_

/*******************************************************************************************************/
/*                                        Function Like Macros                                        */
/*******************************************************************************************************/

#define SET_BIT(REG, BITNUM)          (REG) |=  (1U << (BITNUM))
#define CLR_BIT(REG, BITNUM)          (REG) &= ~(1U << (BITNUM))
#define TOG_BIT(REG, BITNUM)          (REG) ^=  (1U << (BITNUM))
#define GET_BIT(REG, BITNUM)          ((REG) >> (BITNUM) & 1U)

#define SET_ALL_BITS(REG)             (REG) = (0xFFFFFFFF)    // (REG) = (~0)
#define CLR_ALL_BITS(REG)             (REG) = (0x00000000)
#define TOG_ALL_BITS(REG)             (REG) ^= (0xFFFFFFFF)

#endif /* BIT_MATH_H_ */
