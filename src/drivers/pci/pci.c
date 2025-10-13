/**
 * CompileOS PCI Driver - Stub Implementation
 * Minimal PCI support for network drivers
 */

#include "pci.h"
#include <stddef.h>
#include <stdint.h>

// I/O port helpers
static inline void outb(uint16_t port, uint8_t val) {
    __asm__ volatile ("outb %0, %1" : : "a"(val), "Nd"(port));
}

static inline uint8_t inb(uint16_t port) {
    uint8_t val;
    __asm__ volatile ("inb %1, %0" : "=a"(val) : "Nd"(port));
    return val;
}

static void serial_write_char_local(char c) {
    while (!(inb(0x3F8 + 5) & 0x20)) {}
    outb(0x3F8, (uint8_t)c);
}

static void serial_puts_local(const char* s) {
    while (*s) serial_write_char_local(*s++);
}

// Max devices we will record
#define MAX_PCI_DEVICES 32
static pci_device_t pci_devices[MAX_PCI_DEVICES];
static int pci_device_count = 0;

// Build a config address and read via IO ports (legacy PCI config mechanism)
uint32_t pci_read_config_dword(uint8_t bus, uint8_t device, uint8_t function, uint8_t offset) {
    uint32_t address = (uint32_t)((1 << 31) | (bus << 16) | (device << 11) | (function << 8) | (offset & 0xFC));
    outb(0xCF8, (uint8_t)(address & 0xFF));
    outb(0xCF9, (uint8_t)((address >> 8) & 0xFF));
    outb(0xCFC, (uint8_t)((address >> 16) & 0xFF));
    outb(0xCFD, (uint8_t)((address >> 24) & 0xFF));
    // read via 32-bit IO port 0xCFC..0xCFF
    uint32_t value;
    __asm__ volatile ("inl %%dx, %0" : "=a" (value) : "d" (0xCFC));
    return value;
}

void pci_write_config_dword(uint8_t bus, uint8_t device, uint8_t function, uint8_t offset, uint32_t value) {
    uint32_t address = (uint32_t)((1 << 31) | (bus << 16) | (device << 11) | (function << 8) | (offset & 0xFC));
    outb(0xCF8, (uint8_t)(address & 0xFF));
    outb(0xCF9, (uint8_t)((address >> 8) & 0xFF));
    outb(0xCFC, (uint8_t)((address >> 16) & 0xFF));
    outb(0xCFD, (uint8_t)((address >> 24) & 0xFF));
    __asm__ volatile ("outl %0, %%dx" :: "a" (value), "d" (0xCFC));
}

int pci_scan_devices(void) {
    pci_device_count = 0;
    serial_puts_local("[PCI] pci_scan_devices() called\n");
    for (uint8_t bus = 0; bus < 1; bus++) { // scan only bus 0 for now
        for (uint8_t dev = 0; dev < 32; dev++) {
            uint32_t id = pci_read_config_dword(bus, dev, 0, 0x00);
            uint16_t vendor = id & 0xFFFF;
            uint16_t device = (id >> 16) & 0xFFFF;
            if (vendor == 0xFFFF || vendor == 0x0000) continue;
            if (pci_device_count >= MAX_PCI_DEVICES) break;
            pci_devices[pci_device_count].vendor_id = vendor;
            pci_devices[pci_device_count].device_id = device;
            uint32_t classcode = pci_read_config_dword(bus, dev, 0, 0x08);
            pci_devices[pci_device_count].revision_id = classcode & 0xFF;
            pci_devices[pci_device_count].prog_if = (classcode >> 8) & 0xFF;
            pci_devices[pci_device_count].subclass = (classcode >> 16) & 0xFF;
            pci_devices[pci_device_count].class_code = (classcode >> 24) & 0xFF;
            // Read BAR0
            uint32_t bar0 = pci_read_config_dword(bus, dev, 0, 0x10);
            pci_devices[pci_device_count].base_address[0] = bar0;
            // Read interrupt line/pin
            uint32_t intr = pci_read_config_dword(bus, dev, 0, 0x3C);
            pci_devices[pci_device_count].interrupt_line = intr & 0xFF;
            pci_devices[pci_device_count].interrupt_pin = (intr >> 8) & 0xFF;
            serial_puts_local("[PCI] found device: ");
            // print vendor/device as hex
            const char* hex = "0123456789ABCDEF";
            char buf[32]; int p=0;
            uint16_t v = vendor;
            for (int i=12;i>=0;i-=4) buf[p++] = hex[(v >> i) & 0xF];
            buf[p++] = ':';
            uint16_t d = device;
            for (int i=12;i>=0;i-=4) buf[p++] = hex[(d >> i) & 0xF];
            buf[p]=0; serial_puts_local(buf); serial_puts_local("\n");
            pci_device_count++;
        }
    }
    if (pci_device_count == 0) {
        // No devices found - install fallback stub
        serial_puts_local("[PCI] no devices found - using stub\n");
        pci_devices[0].vendor_id = 0x10EC;
        pci_devices[0].device_id = 0x8139;
        pci_devices[0].base_address[0] = 0xC000;
        pci_devices[0].interrupt_line = 11;
        pci_device_count = 1;
    }
    return pci_device_count;
}

int pci_get_device_count(void) {
    return pci_device_count;
}

pci_device_t* pci_find_device(uint16_t vendor_id, uint16_t device_id) {
    for (int i = 0; i < pci_device_count; i++) {
        if (pci_devices[i].vendor_id == vendor_id && pci_devices[i].device_id == device_id) return &pci_devices[i];
    }
    return NULL;
}

pci_device_t* pci_get_devices(void) {
    return pci_devices;
}
