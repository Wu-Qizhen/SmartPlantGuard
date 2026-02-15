/**
 * 适用于 RTOS 的微秒级延时
 * 建议在 main() 中先调用 delay_init()
 * 代码不注释，同事两行泪！（给！爷！写！）
 * Elegance is not a dispensable luxury but a quality that decides between success and failure!
 * Created by Wu Qizhen on 2026.02.14
 */
#include "delay.h"
#include "stm32f1xx.h"

// DWT 寄存器
#define DWT_CYCCNT   (*((volatile uint32_t *)0xE0001004))
#define DWT_CONTROL  (*((volatile uint32_t *)0xE0001000))
#define SCB_DEMCR    (*((volatile uint32_t *)0xE000EDFC))

static uint32_t cpu_freq_mhz = 0; // CPU 频率（MHz）

// 初始化 DWT（必须在系统时钟初始化后调用）
void delay_init(void) {
    // 使能 DWT 跟踪单元
    SCB_DEMCR |= CoreDebug_DEMCR_TRCENA_Msk;
    // 使能 CYCCNT 计数器
    DWT_CONTROL |= DWT_CTRL_CYCCNTENA_Msk;
    DWT_CYCCNT = 0;

    // 获取 CPU 频率（MHz），假设 SystemCoreClock 已由 HAL 初始化
    cpu_freq_mhz = SystemCoreClock / 1000000;
}

// 微秒延时（阻塞）
void delay_us(uint32_t us) {
    uint32_t start = DWT_CYCCNT;
    uint32_t ticks = us * cpu_freq_mhz; // 需要计数的周期数
    while ((DWT_CYCCNT - start) < ticks);
}
