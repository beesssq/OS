#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/time.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Name");
MODULE_DESCRIPTION("A Linux kernel module for TSU to calculate year time ratio");

static int __init tsu_module_init(void) {
    struct timespec64 ts;
    struct tm tm;
    unsigned long total_seconds_in_year = 365 * 24 * 60 * 60; // Количество секунд в обычном году
    unsigned long passed_seconds, remaining_seconds;

    // Получаем текущее время
    ktime_get_real_ts64(&ts);
    time64_to_tm(ts.tv_sec, 0, &tm);

    // Считаем прошедшие секунды с начала года
    passed_seconds = (tm.tm_yday * 24 * 60 * 60) + (tm.tm_hour * 60 * 60) + (tm.tm_min * 60) + tm.tm_sec;

    // Считаем оставшиеся секунды до конца года
    remaining_seconds = total_seconds_in_year - passed_seconds;

    // Вычисляем отношение в целых числах
    if (passed_seconds > 0) {
        unsigned long ratio_integer = remaining_seconds / passed_seconds;
        unsigned long ratio_fraction = (remaining_seconds * 100 / passed_seconds) % 100; // Дробная часть в сотых

        pr_info("TSU Module: Passed seconds = %lu\n", passed_seconds);
        pr_info("TSU Module: Remaining seconds = %lu\n", remaining_seconds);
        pr_info("TSU Module: Ratio = %lu.%02lu\n", ratio_integer, ratio_fraction);
    } else {
        pr_info("TSU Module: Invalid time calculation\n");
    }

    return 0;
}

static void __exit tsu_module_exit(void) {
    pr_info("TSU Module: Unloading module\n");
}

module_init(tsu_module_init);
module_exit(tsu_module_exit);

