# CLion RTOS 集成

1. CLion：
   1. 设置
   2. 构建、执行、部署
   3. 嵌入式开发
   4. 启用 RTOS 集成
   5. FreeRTOS
2. STM32 CubeMX（项目里已经配置好了）：
   1. Middleware and Software...
   2. FreeRTOS
   3. Config Parameters
   4. RECORD_STACK_HIGH_ADDRESS -> Enabled（开启记录栈顶地址）
   5. GENERATE_RUN_TIME_STAS -> Enabled（生成运行时统计）