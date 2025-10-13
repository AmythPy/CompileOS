/**
 * CompileOS PCI Driver - Stub Implementation
 * Minimal PCI support for network drivers
 */

#ifndef PCI_H
#define PCI_H

#include <stdint.h>

// Basic PCI types
typedef struct {
    uint16_t vendor_id;
    uint16_t device_id;
    uint16_t command;
    uint16_t status;
    uint8_t revision_id;
    uint8_t prog_if;
    uint8_t subclass;
    uint8_t class_code;
    uint8_t cache_line_size;
    uint8_t latency_timer;
    uint8_t header_type;
    uint8_t bist;
    uint32_t base_address[6];
    uint32_t cardbus_cis_ptr;
    uint16_t subsystem_vendor_id;
    uint16_t subsystem_id;
    uint32_t expansion_rom_base;
    uint8_t capabilities_ptr;
    uint8_t interrupt_line;
    uint8_t interrupt_pin;
    uint8_t min_grant;
    uint8_t max_latency;
} pci_device_t;

// PCI constants
#define PCI_COMMAND 0x04
#define PCI_COMMAND_MASTER 0x04
#define PCI_COMMAND_IO 0x01

// PCI functions (stub implementations)
int pci_scan_devices(void);
pci_device_t* pci_find_device(uint16_t vendor_id, uint16_t device_id);
pci_device_t* pci_get_devices(void);
uint32_t pci_read_config_dword(uint8_t bus, uint8_t device, uint8_t function, uint8_t offset);
void pci_write_config_dword(uint8_t bus, uint8_t device, uint8_t function, uint8_t offset, uint32_t value);
int pci_get_device_count(void);

#endif // PCI_H
