#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>



#define KERNEL_VIRTUAL_BASE 0xC0000000

uint32_t next_virtual_address = KERNEL_VIRTUAL_BASE;

uint32_t allocate_virtual_address(uint32_t size) {
    uint32_t virtual_address = next_virtual_address;
    next_virtual_address += size;
    return virtual_address;
}

void deallocate_virtual_address(uint32_t virtual_address, uint32_t size) {
    // Aquí puedes implementar la lógica para liberar la dirección virtual asignada
    // En esta implementación básica, simplemente se decrementa el puntero de próxima dirección virtual
    if (virtual_address + size == next_virtual_address) {
        next_virtual_address -= size;
    }
}

// Ejemplo de uso de la asignación de direcciones virtuales
void example_usage() {
    uint32_t virtual_address1 = allocate_virtual_address(4096); // Asignar 4 KB
    uint32_t virtual_address2 = allocate_virtual_address(8192); // Asignar 8 KB

    // Utilizar las direcciones virtuales asignadas

    deallocate_virtual_address(virtual_address1, 4096); // Liberar la primera dirección virtual
    deallocate_virtual_address(virtual_address2, 8192); // Liberar la segunda dirección virtual
}


// Tamaño de un marco de página en bytes
#define PAGE_SIZE 4096

// Dirección base del área de memoria física del sistema
#define PHYSICAL_MEMORY_BASE 0x100000

// Estructura para representar un marco de página libre
typedef struct {
    uint32_t address; // Dirección física base del marco de página libre
} FreePage;

// Arreglo de marcos de página libres
FreePage free_pages[256]; // Suponemos un máximo de 256 marcos de página

// Variable global que guarda el índice del próximo marco de página libre disponible
uint8_t next_free_page = 0;

// Función para inicializar la gestión de marcos de página libres
void initialize_memory() {
    // Calcular la cantidad total de marcos de página disponibles en el sistema
    uint32_t total_pages = (0x1000000 - PHYSICAL_MEMORY_BASE) / PAGE_SIZE; // Suponemos 16 MB de memoria física

    // Inicializar el arreglo de marcos de página libres
    for (uint32_t i = 0; i < total_pages; i++) {
        free_pages[i].address = PHYSICAL_MEMORY_BASE + (i * PAGE_SIZE);
    }

    // Actualizar el índice del próximo marco de página libre disponible
    next_free_page = 0;
}

// Función para asignar un marco de página libre
uint32_t allocate_page() {
    if (next_free_page == 256) {
        // No hay más marcos de página libres disponibles
        return 0;
    }

    uint32_t page_address = free_pages[next_free_page].address;
    next_free_page++;

    return page_address;
}

// Función para liberar un marco de página asignado
void deallocate_page(uint32_t page_address) {
    if (next_free_page == 0) {
        // No se pueden liberar más marcos de página
        return;
    }

    // Buscar la posición correcta para insertar el marco de página liberado
    uint8_t insert_position = next_free_page - 1;
    while (insert_position > 0 && free_pages[insert_position - 1].address > page_address) {
        free_pages[insert_position] = free_pages[insert_position - 1];
        insert_position--;
    }

    // Insertar el marco de página liberado en su posición correcta
    free_pages[insert_position].address = page_address;
    next_free_page--;
}

// Ejemplo de uso de la gestión de marcos de página libres
void example_usage() {

    uint32_t page1 = allocate_page();
    uint32_t page2 = allocate_page();

    // Utilizar los marcos de página asignados

    deallocate_page(page1);
    deallocate_page(page2);
}

// Definición de una entrada de tabla de páginas
typedef struct {
    uint32_t present : 1;  // Bit de presencia
    uint32_t rw : 1;       // Bit de lectura/escritura
    uint32_t user : 1;     // Bit de nivel de privilegio
    uint32_t accessed : 1; // Bit de acceso
    uint32_t dirty : 1;    // Bit de modificación
    uint32_t unused : 7;   // Bits no utilizados
    uint32_t frame : 20;   // Dirección base del marco de página
} page_table_entry_t;

// Tabla de páginas
page_table_entry_t page_table[1024];

// Directorio de tablas de páginas
page_table_entry_t page_directory[1024] __attribute__((aligned(4096)));

// Función para inicializar la paginación
void init_paging() {
    // Inicializar todas las entradas del directorio de tablas de páginas como no presentes
    for (size_t i = 0; i < 1024; ++i) {
        page_directory[i].present = 0;
    }

    // Mapear la tabla de páginas al inicio de la memoria física
    page_directory[0].present = 1;
    page_directory[0].rw = 1;
    page_directory[0].frame = (uint32_t)page_table >> 12;

    // Activar paginación
    asm volatile("mov %0, %%cr3" ::"r" (page_directory));
    uint32_t cr0;
    asm volatile("mov %%cr0, %0" : "=r" (cr0));
    cr0 |= 0x80000000; // Establecer el bit de paginación en CR0
    asm volatile("mov %0, %%cr0" ::"r" (cr0));
}

// Función para mapear una página virtual a una dirección física específica
void map_page(uint32_t virtual_address, uint32_t physical_address, bool is_user_page) {
    // Calcular el índice de la tabla de páginas y el desplazamiento dentro de la tabla
    uint32_t directory_index = (virtual_address >> 22) & 0x3FF;
    uint32_t table_index = (virtual_address >> 12) & 0x3FF;

    // Verificar si la tabla de páginas correspondiente al directorio ya está presente
    if (!page_directory[directory_index].present) {
        // Si no está presente, asignar un marco de página para la tabla de páginas
        uint32_t page_table_address = allocate_page();
        if (page_table_address == 0) {
            // No hay marcos de página libres disponibles, manejar el error
            // (por ejemplo, mostrar un mensaje o terminar el sistema)
            return;
        }

        // Configurar la entrada de la tabla de páginas correspondiente al directorio
        page_directory[directory_index].present = 1;
        page_directory[directory_index].rw = 1; // Permisos de lectura/escritura
        page_directory[directory_index].user = is_user_page ? 1 : 0; // Permisos de nivel de privilegio

        // Establecer la dirección física base de la tabla de páginas
        page_directory[directory_index].frame = page_table_address >> 12;
    }

    // Obtener la dirección base de la tabla de páginas correspondiente al directorio
    uint32_t* page_table_base = (uint32_t*)(page_directory[directory_index].frame << 12);

    // Configurar la entrada de la tabla de páginas correspondiente al índice de la tabla
    page_table_base[table_index] = physical_address;
    page_table_base[table_index] |= 0x1; // Establecer el bit de presencia

    // Realizar un flush de la entrada de la caché de traducción de direcciones (TLB)
    asm volatile("invlpg (%0)" ::"r" (virtual_address));
}

#define MAX_PHYSICAL_PAGES 256

typedef struct {
    uint32_t address;
    bool allocated;
} PhysicalPage;

PhysicalPage physical_pages[MAX_PHYSICAL_PAGES];
uint32_t total_physical_pages;

void init_physical_memory() {
    // Inicializar todas las páginas físicas como no asignadas
    for (uint32_t i = 0; i < MAX_PHYSICAL_PAGES; ++i) {
        physical_pages[i].address = PHYSICAL_MEMORY_BASE + (i * PAGE_SIZE);
        physical_pages[i].allocated = false;
    }
    
    // Calcular el número total de páginas físicas disponibles
    total_physical_pages = (0x1000000 - PHYSICAL_MEMORY_BASE) / PAGE_SIZE;
}

uint32_t allocate_physical_page() {
    for (uint32_t i = 0; i < total_physical_pages; ++i) {
        if (!physical_pages[i].allocated) {
            physical_pages[i].allocated = true;
            return physical_pages[i].address;
        }
    }
    return 0;
}

void deallocate_physical_page(uint32_t address) {
    for (uint32_t i = 0; i < total_physical_pages; ++i) {
        if (physical_pages[i].address == address) {
            physical_pages[i].allocated = false;
            break;
        }
    }
}

void handle_page_fault(uint32_t virtual_address) {
    // Calcula la dirección física correspondiente al virtual_address
    uint32_t physical_address = translate_virtual_to_physical(virtual_address);
    
    // Verifica si la página física está asignada
    if (!is_page_allocated(physical_address)) {
        // Asigna una página física
        uint32_t new_physical_page = allocate_physical_page();
        
        // Mapea la página virtual a la nueva página física
        map_virtual_to_physical(virtual_address, new_physical_page);
    }
    
    // Actualiza la entrada de la tabla de páginas para marcarla como presente
    update_page_table_entry(virtual_address);
}

