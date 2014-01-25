#ifndef TIMER_H
#define TIMER_H

#define TIMER_IRQ 0

void timer_initialise(uint32_t delay);
void timer_clear_interrupt();

#endif
