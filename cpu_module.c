#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/hrtimer.h>
#include <linux/ktime.h>
#include <linux/sched/loadavg.h>
#include <linux/param.h>
// Parametr interwału (czas w sekundach między logami)
static unsigned int log_interval = 10;
module_param(log_interval, uint, 0644);
MODULE_PARM_DESC(log_interval, "Time after which CPU usage will be recorder");
// Timer
static struct hrtimer cpu_timer;

// Funkcja wykonująca zapis obciążenia CPU
static enum hrtimer_restart get_cpu_usage(struct hrtimer *timer) {
    unsigned long load = (avenrun[0] * 100) / 1024;
    pr_info("CPU USAGE MODULE: Obciążenie CPU wynosi %lu%%\n", load);
    // Ustawienie timera na kolejny cykl
    hrtimer_forward_now(timer, ktime_set(log_interval, 0));
    return HRTIMER_RESTART;
}

// Funkcja inicjalizująca moduł
static int __init cpu_usage_module_init(void) {
    ktime_t timer_time;
    pr_info("CPU USAGE MODULE:MODUL WILL REGISTER CPU USAGE ONCE PER
    %u seconds\n", log_interval);
    // Ustalamy czas wyzwolenia timera
    timer_time = ktime_set(log_interval, 0);
    // Inicjalizowanie timera
    hrtimer_init(&cpu_timer, CLOCK_MONOTONIC, HRTIMER_MODE_REL);
    cpu_timer.function = get_cpu_usage;
    // Uruchomienie timera
    hrtimer_start(&cpu_timer, timer_time, HRTIMER_MODE_REL);
    return 0;
}

// Funkcja usuwająca moduł
static void __exit cpu_usage_module_exit(void) {
    // Zatrzymanie działania timera
    hrtimer_cancel(&cpu_timer);
    pr_info("CPU USAGE MODULE: Module was removed\n");
}

module_init(cpu_usage_module_init);
module_exit(cpu_usage_module_exit);
MODULE_LICENSE("GPL");