#ifndef INTC_H
#define INTC_H

void intc_handle();
void intc_unregister(int irqn);
void intc_register(int irqn, void* clear, void* handler);

#endif
