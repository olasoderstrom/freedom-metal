/* Copyright 2018 SiFive, Inc */
/* SPDX-License-Identifier: Apache-2.0 */

#include <metal/cpu.h>
#include <metal/machine.h>
#include <metal/timer.h>
#ifdef __ICCRISCV__
#include <time.h>
#else
#ifndef __SEGGER_LIBC__
#include <sys/time.h>
#include <sys/times.h>
#endif
#endif

#if defined(__METAL_DT_MAX_HARTS)
/* This implementation serves as a small shim that interfaces with the first
 * timer on a system. */
int metal_timer_get_cyclecount(int hartid, unsigned long long *mcc) {
    struct metal_cpu *cpu = metal_cpu_get(hartid);

    if (cpu) {
        *mcc = metal_cpu_get_timer(cpu);
        return 0;
    }
    return -1;
}

int metal_timer_get_timebase_frequency(int hartid,
                                       unsigned long long *timebase) {
    struct metal_cpu *cpu = metal_cpu_get(hartid);

    if (cpu) {
        *timebase = metal_cpu_get_timebase(cpu);
        return 0;
    }
    return -1;
}

int metal_timer_get_machine_time(int hartid) {
    struct metal_cpu *cpu = metal_cpu_get(hartid);

    if (cpu) {
        return metal_cpu_get_mtime(cpu);
    }
    return 0;
}

int metal_timer_set_machine_time(int hartid, unsigned long long time) {
    struct metal_cpu *cpu = metal_cpu_get(hartid);

    if (cpu) {
        return metal_cpu_set_mtimecmp(cpu, time);
    }
    return -1;
}

#else

/* This implementation of gettimeofday doesn't actually do anything, it's just
 * there to provide a shim and return 0 so we can ensure that everything can
 * link to _gettimeofday.
 */
int nop_cyclecount(int id, unsigned long long *c)
    __attribute__((section(".text.metal.nop.cyclecount")));
int nop_cyclecount(int id, unsigned long long *c) { return -1; }
int nop_timebase(unsigned long long *t)
    __attribute__((section(".text.metal.nop.timebase")));
int nop_timebase(unsigned long long *t) { return -1; }
int nop_tick(int second) __attribute__((section(".text.metal.nop.tick")));
int nop_tick(int second) { return -1; }
#ifdef __IAR_SYSTEMS_ICC__
int metal_timer_get_cyclecount(int hartid, unsigned long long *c);
#pragma weak metal_timer_get_cyclecount = nop_cyclecount
int metal_timer_get_timebase_frequency(unsigned long long *t);
#pragma weak metal_timer_get_timebase_frequency = nop_timebase
int metal_timer_set_tick(int second);
#pragma weak metal_timer_set_tick = nop_tick
#else
int metal_timer_get_cyclecount(int hartid, unsigned long long *c)
    __attribute__((weak, alias("nop_cyclecount"))) {
#pragma message(                                                               \
    "There is no default timer device, metal_timer_get_cyclecount() will always return cyclecount -1.")
}
int metal_timer_get_timebase_frequency(unsigned long long *t)
    __attribute__((weak, alias("nop_timebase"))) {
#pragma message(                                                               \
    "There is no default timer device, metal_timer_get_timebase_frequency() will always return timebase -1.")
}
int metal_timer_set_tick(int second) __attribute__((weak, alias("nop_tick"))) {
#pragma message(                                                               \
    "There is no default timer device, metal_timer_set_tick) will always return -1.")
}
#endif

#endif
