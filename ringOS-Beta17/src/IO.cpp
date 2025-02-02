#include "IO.hpp"

void outb(uint16_t port, uint8_t value)
{
    asm volatile ("outb %0, %1" : : "a"(value), "Nd"(port));
}

void outw(unsigned short int port, unsigned short int value)
{
    asm volatile("outw %%ax, %%dx"
				 :
				 : "d"(port), "a"(value));
}

uint8_t inb(uint16_t port)
{
    uint8_t returnVal;
    asm volatile ("inb %1, %0"
    : "=a"(returnVal)
    : "Nd"(port));
    return returnVal;
}

void io_wait()
{
    asm volatile ("outb %%al, $0x80" : : "a"(0));
}