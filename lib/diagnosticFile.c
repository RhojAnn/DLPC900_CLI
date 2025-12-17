/*
* diagnosticFile.c
*
* Diagnostic logging to file and console
*
*/

#include "diagnosticwindow.h"
#include "usb.h"
#include <stdio.h>
#include <string.h>
#include <time.h>

#ifndef DIAG_LOG_SIZE
#define DIAG_LOG_SIZE 256
#endif

typedef struct {
    DIAG_CommandInfo_t info;
    unsigned char outBuf[USB_MAX_PACKET_SIZE+1];
    unsigned char inBuf[USB_MAX_PACKET_SIZE+1];
    int result;
    time_t ts;
} DIAG_LogEntry_t;

static DIAG_LogEntry_t diagLog[DIAG_LOG_SIZE];
static int diagHead = 0;
static int diagCount = 0;
static FILE *diagFile = NULL;
static int diagConsole = 0;

int DIAG_Init(const char *logfile, int toConsole)
{
    if (logfile) {
        diagFile = fopen(logfile, "a");
        if (!diagFile) return -1;
    }
    diagConsole = toConsole ? 1 : 0;
    return 0;
}

void DIAG_ClearLog(void)
{
    diagHead = 0;
    diagCount = 0;
    if (diagFile) {
        fflush(diagFile);
    }
}

void DIAG_DumpLog(void)
{
    int i, idx;
    for (i = 0; i < diagCount; ++i) {
        idx = (diagHead + i) % DIAG_LOG_SIZE;
        char tsbuf[32] = {0};
        struct tm tmv;
        localtime_s(&tmv, &diagLog[idx].ts);
        strftime(tsbuf, sizeof(tsbuf), "%Y-%m-%d %H:%M:%S", &tmv);
        printf("[%s] CMD=0x%04X (%s) rw=%d res=%d\n",
               tsbuf,
               diagLog[idx].info.USBCmd,
               diagLog[idx].info.CmdName ? diagLog[idx].info.CmdName : "<noname>",
               diagLog[idx].info.Read,
               diagLog[idx].result);
        if (diagFile) {
            fprintf(diagFile, "[%s] CMD=0x%04X (%s) rw=%d res=%d\n",
                    tsbuf,
                    diagLog[idx].info.USBCmd,
                    diagLog[idx].info.CmdName ? diagLog[idx].info.CmdName : "<noname>",
                    diagLog[idx].info.Read,
                    diagLog[idx].result);
        }
    }
    if (diagFile) fflush(diagFile);
}

void DIAG_updateProjectorControl(DIAG_CommandInfo_t *info, int result)
{
    if (!info) return;
    int idx = (diagHead + diagCount) % DIAG_LOG_SIZE;
    if (diagCount == DIAG_LOG_SIZE) {
        /* overwrite oldest */
        diagHead = (diagHead + 1) % DIAG_LOG_SIZE;
        idx = (diagHead + diagCount - 1) % DIAG_LOG_SIZE;
    } else {
        diagCount++;
    }

    diagLog[idx].info = *info;
    diagLog[idx].result = result;
    diagLog[idx].ts = time(NULL);

    if (diagConsole) printf("DIAG: CMD=0x%04X (%s) result=%d\n", 
                            info->USBCmd, 
                            info->CmdName ? info->CmdName : "<noname>", 
                            result);
    
    if (diagFile) {
        char tsbuf[32] = {0};
        struct tm tmv;
        localtime_s(&tmv, &diagLog[idx].ts);
        strftime(tsbuf, sizeof(tsbuf), "%Y-%m-%d %H:%M:%S", &tmv);
        fprintf(diagFile, "[%s] DIAG: CMD=0x%04X (%s) result=%d\n", 
                tsbuf, 
                info->USBCmd, 
                info->CmdName ? info->CmdName : "<noname>", 
                result);
        fflush(diagFile);
    }
}

void DIAG_updateCommandLog(DIAG_CommandInfo_t *info, unsigned char *outData, unsigned char *inData, int result)
{
    if (!info) return;
    int idx = (diagHead + diagCount - 1 + DIAG_LOG_SIZE) % DIAG_LOG_SIZE;
    /* if no log entries yet, create one */
    if (diagCount == 0) {
        idx = (diagHead + diagCount) % DIAG_LOG_SIZE;
        diagCount = 1;
    }

    diagLog[idx].info = *info;
    diagLog[idx].result = result;
    diagLog[idx].ts = time(NULL);

    if (outData && info->wrBytes) {
        int copyLen = (info->wrBytes + 2) > (int)sizeof(diagLog[idx].outBuf) 
                    ? (int)sizeof(diagLog[idx].outBuf) : (info->wrBytes + 2);
        
        memcpy(diagLog[idx].outBuf, outData, copyLen);
    }

    if (inData && info->rdBytes) {
        int copyLen = (info->rdBytes + 2) > (int)sizeof(diagLog[idx].inBuf) 
                    ? (int)sizeof(diagLog[idx].inBuf) : (info->rdBytes + 2);
        
        memcpy(diagLog[idx].inBuf, inData, copyLen);
    }

    if (diagConsole) printf("DIAG_BUF: CMD=0x%04X outBytes=%d inBytes=%d res=%d\n", 
                            info->USBCmd, 
                            info->wrBytes, 
                            info->rdBytes, 
                            result);
    
    if (diagFile) {
        char tsbuf[32] = {0};
        struct tm tmv;
        localtime_s(&tmv, &diagLog[idx].ts);
        strftime(tsbuf, sizeof(tsbuf), "%Y-%m-%d %H:%M:%S", &tmv);
        fprintf(diagFile, "[%s] DIAG_BUF: CMD=0x%04X outBytes=%d inBytes=%d res=%d\n", 
                tsbuf, 
                info->USBCmd, 
                info->wrBytes, 
                info->rdBytes, 
                result);
        fflush(diagFile);
    }
}