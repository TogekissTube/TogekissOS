#include <stdint.h>

struct idt_entry {
    uint16_t base_low;
    uint16_t selector;
    uint8_t reserved;
    uint8_t flags;
    uint16_t base_high;
};

struct idt_ptr {
    uint16_t limit;
    uint32_t base;
};

struct idt_entry idt[256];
struct idt_ptr ip;

extern void idt_load(struct idt_ptr*);

void idt_set_gate(int num, uint32_t base, uint16_t selector, uint8_t flags) {
    // Rellena el descriptor de la IDT
    idt[num].base_low = base & 0xFFFF;
    idt[num].base_high = (base >> 16) & 0xFFFF;

    idt[num].selector = selector;
    idt[num].reserved = 0;
    idt[num].flags = flags;
}

void idt_install() {
    // Configura la IDT
    ip.limit = sizeof(struct idt_entry) * 256 - 1;
    ip.base = (uint32_t)&idt;

    // Rellena la IDT con descriptores nulos
    for (int i = 0; i < 256; i++) {
        idt_set_gate(i, 0, 0, 0);
    }

    // Cargar la IDT
    idt_load(&ip);
}
