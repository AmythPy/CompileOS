/*
 * Cleaned RTL8139 driver for CompileOS
 * Provides device probe/init/send/recv/isr and uses kernel allocator
 * Keeps all driver-local symbols static where possible to avoid collisions
 */

#include "network.h"
#include "../io/io.h"
#include "../pci/pci.h"
#include "../../kernel/memory/kmalloc.h"
#include <string.h>

/* forward declaration of central registry function (declared in src/net/net.h) */
int network_register_device(net_device_t* dev);

// RTL8139 receive buffer
#define RTL8139_RX_BUFFER_SIZE 8192
static uint8_t *rtl_rx_buffer = NULL;

// RTL8139 transmit buffers (4 descriptors)
#define RTL8139_TX_BUFFERS 4
#define RTL8139_TX_BUFFER_SIZE 2048
static uint8_t *rtl_tx_buffers[RTL8139_TX_BUFFERS] = { NULL };

// Internal list of devices managed by this driver (for driver-local bookkeeping)
// driver-local bookkeeping is unnecessary now that we register centrally

// Forward declarations (driver-local helpers)
static net_device_t* drv_device_register(net_device_type_t type, const char* name);
static void drv_device_unregister(net_device_t* dev);

// Probe function called by PCI code when a device is found
net_device_t* rtl8139_probe(uint16_t io_base, uint8_t irq) {
    net_device_t* dev = drv_device_register(NET_DEVICE_RTL8139, "eth0");
    if (!dev) return NULL;

    dev->io_base = io_base;
    dev->irq = irq;

    // Assign driver operations
    dev->init = rtl8139_init;
    dev->send = rtl8139_send;
    dev->recv = rtl8139_recv;
    dev->shutdown = NULL;
    dev->open = NULL;
    dev->close = NULL;
    dev->ioctl = NULL;

    return dev;
}

int rtl8139_init(net_device_t* dev) {
    if (!dev) return -1;

    // Allocate driver buffers if not already
    if (!rtl_rx_buffer) {
        rtl_rx_buffer = (uint8_t*)kmalloc(RTL8139_RX_BUFFER_SIZE);
        if (!rtl_rx_buffer) return -1;
    }
    for (int i = 0; i < RTL8139_TX_BUFFERS; i++) {
        if (!rtl_tx_buffers[i]) {
            rtl_tx_buffers[i] = (uint8_t*)kmalloc(RTL8139_TX_BUFFER_SIZE);
            if (!rtl_tx_buffers[i]) return -1;
        }
    }

    // Basic device init: reset and program MAC and buffers
    // Note: pci_read_config_dword etc are expected to exist in pci.h
    // For safety, operate only on the I/O base provided by the probe caller

    // Software reset
    io_outb(dev->io_base + RTL8139_CHIPCMD, RTL8139_CMD_RESET);
    while (io_inb(dev->io_base + RTL8139_CHIPCMD) & RTL8139_CMD_RESET) {
        // wait
    }

    // Program MAC address (if not already set)
    // Copy first 4 bytes
    uint32_t mac0 = *(uint32_t*)dev->config.mac_addr;
    uint16_t mac1 = *(uint16_t*)(dev->config.mac_addr + 4);
    io_outl(dev->io_base + RTL8139_IDR0, mac0);
    io_outw(dev->io_base + RTL8139_IDR1, mac1);

    // Program transmit buffers
    for (int i = 0; i < RTL8139_TX_BUFFERS; i++) {
        io_outl(dev->io_base + RTL8139_TXADDR0 + i * 4, (uint32_t)rtl_tx_buffers[i]);
    }

    // Program receive buffer
    io_outl(dev->io_base + RTL8139_RXBUF, (uint32_t)rtl_rx_buffer);

    // Set multicast filter to accept all
    io_outl(dev->io_base + RTL8139_MAR0, 0xFFFFFFFF);
    io_outl(dev->io_base + RTL8139_MAR1, 0xFFFFFFFF);

    // Enable interrupts we care about
    io_outw(dev->io_base + RTL8139_IMR, RTL8139_INT_RX_OK | RTL8139_INT_TX_OK | RTL8139_INT_RX_ERR);

    // Enable receiver and transmitter
    io_outb(dev->io_base + RTL8139_CHIPCMD, RTL8139_CMD_RX_ENABLE | RTL8139_CMD_TX_ENABLE);

    dev->link_state = NET_LINK_UP;
    return 0;
}

int rtl8139_send(net_device_t* dev, const void* data, size_t len) {
    if (!dev || !data || len == 0 || len > RTL8139_TX_BUFFER_SIZE) return -1;

    // Find a free descriptor
    int desc = -1;
    for (int i = 0; i < RTL8139_TX_BUFFERS; i++) {
        uint32_t status = io_inl(dev->io_base + RTL8139_TXSTAT0 + i * 4);
        if ((status & 0x1FFF) == 0) { // descriptor free
            desc = i;
            break;
        }
    }
    if (desc < 0) return -1;

    memcpy(rtl_tx_buffers[desc], data, len);
    io_outl(dev->io_base + RTL8139_TXSTAT0 + desc * 4, (uint32_t)len);

    dev->stats.tx_packets++;
    dev->stats.tx_bytes += len;

    return (int)len;
}

int rtl8139_recv(net_device_t* dev, void* buffer, size_t len) {
    if (!dev || !buffer || len == 0) return -1;

    // Very simplified: check Rx status register and copy from rtl_rx_buffer
    uint16_t rxstat = io_inw(dev->io_base + RTL8139_RXSTAT);
    if (!(rxstat & 0x01)) return 0; // no packet

    // read packet length from buffer header
    uint16_t offset = 0; // driver should track a read pointer; simplified here
    uint16_t pkt_len = *(uint16_t*)(rtl_rx_buffer + offset + 2);
    uint16_t copy_len = (pkt_len < len) ? pkt_len : (uint16_t)len;
    memcpy(buffer, rtl_rx_buffer + offset + 4, copy_len);

    // Update stats
    dev->stats.rx_packets++;
    dev->stats.rx_bytes += pkt_len;

    // Advance read pointer and tell NIC (simplified)
    // TODO: properly manage the ring buffer read pointer

    return copy_len;
}

void rtl8139_isr(void) {
    // ISR should read device status from its I/O base; since ISR is global
    // we keep this minimal. In a full implementation the ISR would find the
    // device instance and process RX/TX events.
}

// Driver-local device registration helpers
static net_device_t* drv_device_register(net_device_type_t type, const char* name) {
    net_device_t* dev = (net_device_t*)kmalloc(sizeof(net_device_t));
    if (!dev) return NULL;
    memset(dev, 0, sizeof(net_device_t));

    dev->type = type;
    strncpy(dev->name, name, sizeof(dev->name) - 1);
    dev->link_state = NET_LINK_DOWN;

    // Default MAC (placeholder)
    dev->config.mac_addr[0] = 0x52;
    dev->config.mac_addr[1] = 0x54;
    dev->config.mac_addr[2] = 0x00;
    dev->config.mac_addr[3] = 0x12;
    dev->config.mac_addr[4] = 0x34;
    dev->config.mac_addr[5] = 0x56;

    // Register with central registry
    if (network_register_device(dev) != 0) {
        kfree(dev);
        return NULL;
    }

    return dev;
}

static void drv_device_unregister(net_device_t* dev) {
    // Unregistering would require a central unregister API; keep as noop for now
    (void)dev;
}

// Note: this driver intentionally does not define kernel-global helpers like
// network_send_packet or network_get_device. The kernel's net stack should
// reference the registered net_device_t instances (this repo currently lacks
// a central registry; we'll keep the driver self-contained and safe to compile).
    // Set MAC address (would read from EEPROM in real implementation)
