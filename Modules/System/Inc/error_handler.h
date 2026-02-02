#ifndef ERROR_HANDLER_H
#define ERROR_HANDLER_H

#include "system_state.h"

// 错误处理函数
void ErrorHandler_HandleError(ErrorCodeEnum error);
void ErrorHandler_LogError(ErrorCodeEnum error, const char* message);
void ErrorHandler_ResetErrors(void);
uint32_t ErrorHandler_GetErrorCount(void);
ErrorCodeEnum ErrorHandler_GetLastError(void);

#endif // ERROR_HANDLER_H