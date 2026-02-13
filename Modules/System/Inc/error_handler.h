/**
* 代码不注释，同事两行泪！（给！爷！写！）
 * Elegance is not a dispensable luxury but a quality that decides between success and failure!
 * Created by Wu Qizhen on 2026.02.02
 * Updated by Wu Qizhen on 2026.02.13
 */
#ifndef ERROR_HANDLER_H
#define ERROR_HANDLER_H

#include <stdint.h>
#include "system_state.h"

// 错误处理函数
void ErrorHandler_HandleError(ErrorCodeEnum error);

void ErrorHandler_LogError(ErrorCodeEnum error, const char *message);

void ErrorHandler_ResetErrors(void);

uint32_t ErrorHandler_GetErrorCount(void);

ErrorCodeEnum ErrorHandler_GetLastError(void);

#endif // ERROR_HANDLER_H
