#ifndef SCHEDULER_H
#define SCHEDULER_H

#define SCHEDULER_FREQUENCY 0xFFFFF

void scheduler_add(void* entry);
void scheduler_next();
void* scheduler_get_current();
void scheduler_set_current(void* context);

#endif
