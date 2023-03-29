#ifndef WATCHDOG_THREAD_H_
#define WATCHDOG_THREAD_H_

/// the analyzer thread start function
void *watchdog_routine();
void watchdog_notify(int);

#endif