#include "task_scheduler.h"

static TaskConfig tasks[TASK_ID_COUNT];
static bool isSchedulerRunning = false;

// 初始化任务调度器
bool TaskScheduler_Init(void) {
    // 初始化任务数组
    for (int i = 0; i < TASK_ID_COUNT; i++) {
        tasks[i].id = (TaskIDEnum)i;
        tasks[i].name[0] = '\0';
        tasks[i].intervalMs = 1000;
        tasks[i].lastRunTime = 0;
        tasks[i].state = TASK_READY;
        tasks[i].enabled = false;
        tasks[i].taskFunction = NULL;
    }
    
    return true;
}

// 添加任务
bool TaskScheduler_AddTask(TaskConfig* config) {
    if (!config || config->id >= TASK_ID_COUNT) {
        return false;
    }
    
    tasks[config->id] = *config;
    tasks[config->id].state = TASK_READY;
    tasks[config->id].lastRunTime = HAL_GetTick();
    
    return true;
}

// 启动调度器
void TaskScheduler_Start(void) {
    isSchedulerRunning = true;
    
    // 初始化所有任务的最后运行时间
    for (int i = 0; i < TASK_ID_COUNT; i++) {
        if (tasks[i].enabled) {
            tasks[i].lastRunTime = HAL_GetTick();
        }
    }
}

// 停止调度器
void TaskScheduler_Stop(void) {
    isSchedulerRunning = false;
}

// 任务执行
void TaskScheduler_Run(void) {
    if (!isSchedulerRunning) {
        return;
    }
    
    uint32_t currentTime = HAL_GetTick();
    
    // 遍历所有任务
    for (int i = 0; i < TASK_ID_COUNT; i++) {
        if (tasks[i].enabled && tasks[i].taskFunction) {
            // 检查是否需要执行
            if (currentTime - tasks[i].lastRunTime >= tasks[i].intervalMs) {
                // 更新状态
                tasks[i].state = TASK_RUNNING;
                
                // 执行任务
                tasks[i].taskFunction();
                
                // 更新最后运行时间
                tasks[i].lastRunTime = currentTime;
                
                // 恢复状态
                tasks[i].state = TASK_READY;
            }
        }
    }
}

// 获取任务状态
TaskStateEnum TaskScheduler_GetTaskState(TaskIDEnum id) {
    if (id >= TASK_ID_COUNT) {
        return TASK_ERROR;
    }
    
    return tasks[id].state;
}

// 启用 / 禁用任务
void TaskScheduler_EnableTask(TaskIDEnum id, bool enable) {
    if (id >= TASK_ID_COUNT) {
        return;
    }
    
    tasks[id].enabled = enable;
    if (enable) {
        tasks[id].lastRunTime = HAL_GetTick();
        tasks[id].state = TASK_READY;
    }
}