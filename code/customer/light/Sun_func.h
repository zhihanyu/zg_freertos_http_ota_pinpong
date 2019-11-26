
#ifndef SUN_FUNC_H_
#define SUN_FUNC_H_


//#include "Light_fun.h"

#define CCT_SUN_FUNC   0x02
#define COLD_SUN_FUNC  0x01
#define WARM_SUN_FUNC  0x00

void CCT_sunset_func(colort_struct_t  *color);
void CCT_sunrise_func(colort_struct_t  *color);
/**
 * @brief CCT sun rise init
 *
 * @param[in] Val_flag: Color flag:Cold color  or  warm color or   CCT
 *			     begin_buf: start Values
 *           end_buf  : end Values
 * @return[out] None
 * @see None.
 * @note None.
 */
void  CCT_sunrise_init(unsigned char Val_flag,unsigned char delay_t,unsigned char begin_buf,unsigned char end_buf);
/**
 * @brief CCT sun down init
 *
 * @param[in] Val_flag: Color flag:Cold color  or  warm color or   CCT
 *			     begin_buf: start Values
 *           end_buf  : end Values
 * @return[out] None
 * @see None.
 * @note None.
 */
void  CCT_sunset_init(unsigned char Val_flag,unsigned char delay_t,unsigned char begin_buf,unsigned char end_buf);
#endif