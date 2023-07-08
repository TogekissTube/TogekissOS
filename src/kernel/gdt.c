#include <stdint.h>

struct gdt_entry {
    uint16_t limit_low;
    uint16_t base_low;
    uint8_t base_middle;
    uint8_t access;
    uint8_t granularity;
    uint8_t base_high;
};

struct gdt_ptr {
    uint16_t limit;
    uint32_t base;
};

struct gdt_entry gdt[3];
struct gdt_ptr gp;

extern void gdt_flush(struct gdt_ptr*);

void gdt_set_gate(int num, uint32_t base, uint32_t limit, uint8_t access, uint8_t granularity) {
    // Rellena el descriptor de la GDT
    gdt[num].base_low = (base & 0xFFFF);
    gdt[num].base_middle = (base >> 16) & 0xFF;
    gdt[num].base_high = (base >> 24) & 0xFF;

    gdt[num].limit_low = (limit & 0xFFFF);
    gdt[num].granularity = ((limit >> 16) & 0x0F) | (granularity & 0xF0);
    gdt[num].access = access;
}

void gdt_install() {
    // Configura la GDT
    gp.limit = (sizeof(struct gdt_entry) * 3) - 1;
    gp.base = (uint32_t)&gdt;

    // Descriptor nulo
    gdt_set_gate(0, 0, 0, 0, 0);

    // Descriptor de código
    gdt_set_gate(1, 0, 0xFFFFFFFF, 0x9A, 0xCF);

    // Descriptor de datos
    gdt_set_gate(2, 0, 0xFFFFFFFF, 0x92, 0xCF);

    // Cargar la GDT
    gdt_flush(&gp);
}
