/*
* Copyright (C) 2015 Texas Instruments Incorporated - http://www.ti.com/
* ALL RIGHTS RESERVED
*
*/

#ifndef DIAGWNDW_H_
#define DIAGWNDW_H_

/**
 * @brief This module handles batchfile related functions
 *
 * @note:
 *
 */

#include "common.h"
#include "API.h"

typedef struct
{
    uint16 USBCmd;          /**< USB Command number */
    uint08 Read;			/**< Read Command / Write Command */
    char const *CmdName;	/**< Command Name */
    uint08 wrBytes;         /**< Number of bytes to write */
    uint08 rdBytes;         /**< Number of bytes to read */
} DIAG_CommandInfo_t;

#ifdef __cplusplus
extern "C" {
#endif

void DIAG_updateProjectorControl(DIAG_CommandInfo_t*, int);
void DIAG_updateCommandLog(DIAG_CommandInfo_t*, unsigned char* , unsigned char*, int);

#ifdef __cplusplus
}
#endif
#endif /* BATCHFILE_H_ */
