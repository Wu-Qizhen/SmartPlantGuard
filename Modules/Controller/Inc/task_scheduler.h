#ifndef TASK_SCHEDULER_H
#define TASK_SCHEDULER_H

// 任务 ID
typedef enum {
    TASK_ID_SENSOR_READ = 0,     // 传感器读取任务
    TASK_ID_CONTROL_LOGIC,       // 控制逻辑任务
    TASK_ID_COMMUNICATION,       // 通信任务
    TASK_ID_SAFETY_CHECK,        // 安全检查任务
    TASK_ID_STATISTICS,          // 统计任务
    TASK_ID_COUNT                // 任务总数
} TaskIDEnum;

// 任务状态
typedef enum {
    TASK_READY = 0,
    TASK_RUNNING,
    TASK_WAITING,
    TASK_ERROR
} TaskStateEnum;

// 任务配置
typedef struct {
    TaskIDEnum id;
    char name[16];
    uint32_t intervalMs;      // 执行间隔
    uint32_t lastRunTime;     // 上次执行时间
    TaskStateEnum state;
    bool enabled;             // 是否启用
    void (*taskFunction)(void); // 任务函数
} TaskConfig;

// 初始化任务调度器
bool TaskScheduler_Init(void);

// 添加任务
bool TaskScheduler_AddTask(TaskConfig* config);

// 启动调度器
void TaskScheduler_Start(void);

// 停止调度器
void TaskScheduler_Stop(void);

// 任务执行（在主循环中调用）
void TaskScheduler_Run(void);

// 获取任务状态
TaskStateEnum TaskScheduler_GetTaskState(TaskIDEnum id);

// 启用/禁用任务
void TaskScheduler_EnableTask(TaskIDEnum id, bool enable);

#endif // TASK_SCHEDULER_H