#include "error_handler.h"

static uint32_t errorCount = 0;

// 处理错误
void ErrorHandler_HandleError(ErrorCodeEnum error) {
    // 设置系统错误状态
    SystemState_SetError(error);
    errorCount++;
}

// 记录错误
void ErrorHandler_LogError(ErrorCodeEnum error, const char* message) {
    // 这里可以添加错误日志记录功能
    // 例如通过串口发送错误信息
    ErrorHandler_HandleError(error);
}

// 重置错误计数
void ErrorHandler_ResetErrors(void) {
    errorCount = 0;
    SystemState_ClearError();
}

// 获取错误计数
uint32_t ErrorHandler_GetErrorCount(void) {
    return errorCount;
}

// 获取最后一个错误
ErrorCodeEnum ErrorHandler_GetLastError(void) {
    return gSystemState.lastError;
}