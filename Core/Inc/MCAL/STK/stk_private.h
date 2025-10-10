/***********************************************************************/
/* Author           : Ameer Alwadiya                                   */
/* Version          : V1.0.0                                           */
/* Date             : 16 Jul 2025                                      */
/* Description      : stk_private.h --> Register Definitions           */
/***********************************************************************/


/***********************************************************************/
/*                           Include Guard                             */
/***********************************************************************/
#ifndef STK_PRIVATE_H_
#define STK_PRIVATE_H_


/***********************************************************************/
/*                        Register Definitions                         */
/*---------------------------------------------------------------------*/
/*     - Private register definitions for internal use only            */
/*     - Memory-mapped register access                                 */
/*     - Hardware abstraction layer                                    */
/***********************************************************************/


/***********************************************************************/
/*                        SysTick Base Address                         */
/***********************************************************************/
#define STK_BASE_ADDRESS                 0xE000E010UL


/***********************************************************************/
/*                     SysTick Register Structure                      */
/***********************************************************************/
typedef struct
{
   volatile uint32_t CTRL;    /* SysTick Control and Status Register  */
   volatile uint32_t LOAD;    /* SysTick Reload Value Register        */
   volatile uint32_t VAL;     /* SysTick Current Value Register       */
   volatile uint32_t CALIB;   /* SysTick Calibration Register         */
} STK_RegisterMap_t;


/***********************************************************************/
/*                     SysTick Register Access                         */
/***********************************************************************/
#define STK  ((volatile STK_RegisterMap_t*)STK_BASE_ADDRESS)


/***********************************************************************/
/*            SysTick Control Register Bit Definitions                 */
/***********************************************************************/
#define STK_CTRL_ENABLE_BIT              0U      /* Counter enable bit */
#define STK_CTRL_TICKINT_BIT             1U      /* Interrupt enable bit */
#define STK_CTRL_CLKSOURCE_BIT           2U      /* Clock source selection bit */
#define STK_CTRL_COUNTFLAG_BIT           16U     /* Count flag bit */


/***********************************************************************/
/*                    SysTick Clock Source Options                     */
/***********************************************************************/
#define STK_CLOCK_SOURCE_AHB_DIV8        0U      /* AHB/8 */
#define STK_CLOCK_SOURCE_AHB             1U      /* AHB */


/***********************************************************************/
/*                     SysTick Maximum Values                          */
/***********************************************************************/
#define STK_MAX_RELOAD_VALUE             0x00FFFFFFUL    /* 24-bit reload value */
#define STK_MAX_CURRENT_VALUE            0x00FFFFFFUL    /* 24-bit current value */


/***********************************************************************/
/*                     SysTick Timing Constants                        */
/***********************************************************************/
/* Assuming HSI = 16MHz, AHB = 16MHz, SysTick Clock = AHB/8 = 2MHz */
#define STK_CLOCK_FREQUENCY_HZ           2000000UL       /* 2MHz */
#define SYSCLK_FREQ_HZ  			     16000000UL
#define STK_TICKS_PER_MICROSECOND        2UL             /* 2 ticks per microsecond */
#define STK_TICKS_PER_MILLISECOND        2000UL          /* 2000 ticks per millisecond */


/***********************************************************************/
/*                        Register Bit Masks                           */
/***********************************************************************/
#define STK_CTRL_ENABLE_MASK             (1UL << STK_CTRL_ENABLE_BIT)
#define STK_CTRL_TICKINT_MASK            (1UL << STK_CTRL_TICKINT_BIT)
#define STK_CTRL_CLKSOURCE_MASK          (1UL << STK_CTRL_CLKSOURCE_BIT)
#define STK_CTRL_COUNTFLAG_MASK          (1UL << STK_CTRL_COUNTFLAG_BIT)


#endif /* STK_PRIVATE_H_ */



