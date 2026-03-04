/**
 * 适用于 RTOS 的微秒级延时
 * 建议在 main() 中先调用 delay_init()
 * 代码不注释，同事两行泪！（给！爷！写！）
 * Elegance is not a dispensable luxury but a quality that decides between success and failure!
 * Created by Wu Qizhen on 2026.02.14
 */
#ifndef DELAY_H
#define DELAY_H

#include <stdint.h>

void delay_us(uint32_t us); // 微秒延时
void delay_init(void); // 初始化延时硬件（如 DWT）

#endif // DELAY_H
