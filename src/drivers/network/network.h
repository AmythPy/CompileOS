/**
 * CompileOS Network Interface Driver - Hero Level
 * RTL8139 PCI Ethernet Controller Driver
 */

#ifndef NETWORK_H
#define NETWORK_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

// (Kernel allocator is used by drivers; drivers should include kmalloc directly)

// Network device types
typedef enum {
    NET_DEVICE_NONE = 0,
    NET_DEVICE_RTL8139,
    NET_DEVICE_NE2000,
    NET_DEVICE_E1000,
    NET_DEVICE_VIRTIO_NET
} net_device_type_t;

// Network link states
typedef enum {
    NET_LINK_DOWN = 0,
    NET_LINK_UP,
    NET_LINK_NEGOTIATING
} net_link_state_t;

// Network statistics
typedef struct {
    uint64_t rx_packets;
    uint64_t tx_packets;
    uint64_t rx_bytes;
    uint64_t tx_bytes;
    uint64_t rx_errors;
    uint64_t tx_errors;
    uint64_t rx_dropped;
    uint64_t tx_dropped;
} net_stats_t;

// Generic network buffer used by drivers and stack
typedef struct net_buffer_t {
    uint8_t* data;
    size_t len;
    size_t capacity;
    struct net_buffer_t* next;
} net_buffer_t;

// Network device configuration
typedef struct {
    uint8_t mac_addr[6];           // MAC address
    uint32_t ip_addr;              // IPv4 address (network byte order)
    uint32_t netmask;              // Subnet mask (network byte order)
    uint32_t gateway;              // Default gateway (network byte order)
    uint32_t dns_server;           // DNS server (network byte order)
    bool dhcp_enabled;             // DHCP enabled
    bool auto_negotiation;         // Auto-negotiation enabled
    uint32_t speed;                // Link speed (Mbps)
    uint32_t duplex;               // Duplex mode (0=half, 1=full)
} net_config_t;

// Network device structure
typedef struct net_device_t {
    net_device_type_t type;
    char name[16];                 // Interface name (e.g., "eth0")
    net_link_state_t link_state;
    net_config_t config;
    net_stats_t stats;
    uint16_t io_base;              // I/O base address
    uint8_t irq;                   // IRQ number
    void* priv;                    // Device-specific data
    struct net_device_t* next;     // Next device in list

    // Device operations
    int (*init)(struct net_device_t* dev);
    int (*shutdown)(struct net_device_t* dev);
    int (*open)(struct net_device_t* dev);
    int (*close)(struct net_device_t* dev);
    int (*send)(struct net_device_t* dev, const void* data, size_t len);
    int (*recv)(struct net_device_t* dev, void* buffer, size_t len);
    int (*ioctl)(struct net_device_t* dev, uint32_t cmd, void* arg);
} net_device_t;

// RTL8139 specific registers
#define RTL8139_IDR0    0x00    // MAC address bytes 0-3
#define RTL8139_IDR1    0x04    // MAC address bytes 4-5
#define RTL8139_MAR0    0x08    // Multicast filter 0-3
#define RTL8139_MAR1    0x0C    // Multicast filter 4-7
#define RTL8139_TXSTAT0 0x10    // Transmit status 0
#define RTL8139_TXSTAT1 0x14    // Transmit status 1
#define RTL8139_TXSTAT2 0x18    // Transmit status 2
#define RTL8139_TXSTAT3 0x1C    // Transmit status 3
#define RTL8139_TXADDR0 0x20    // Transmit descriptor 0
#define RTL8139_TXADDR1 0x24    // Transmit descriptor 1
#define RTL8139_TXADDR2 0x28    // Transmit descriptor 2
#define RTL8139_TXADDR3 0x2C    // Transmit descriptor 3
#define RTL8139_RXBUF   0x30    // Receive buffer start address
#define RTL8139_RXEARLY 0x34    // Early receive byte count
#define RTL8139_RXSTAT  0x36    // Early receive status
#define RTL8139_CHIPCMD 0x37    // Command register
#define RTL8139_RXCFG   0x44    // Receive configuration
#define RTL8139_TXCFG   0x40    // Transmit configuration
#define RTL8139_IMR     0x3C    // Interrupt mask
#define RTL8139_ISR     0x3E    // Interrupt status
#define RTL8139_TXCFG1  0x41    // Transmit configuration 1
#define RTL8139_CFG9346 0x50    // 9346 command register
#define RTL8139_CONFIG0 0x51    // Configuration register 0
#define RTL8139_CONFIG1 0x52    // Configuration register 1
#define RTL8139_CONFIG2 0x53    // Configuration register 2
#define RTL8139_CONFIG3 0x54    // Configuration register 3
#define RTL8139_CONFIG4 0x55    // Configuration register 4
#define RTL8139_CONFIG5 0x56    // Configuration register 5

// RTL8139 command bits
#define RTL8139_CMD_TX_ENABLE  (1 << 2)
#define RTL8139_CMD_RX_ENABLE  (1 << 3)
#define RTL8139_CMD_RESET      (1 << 4)

// RTL8139 interrupt bits
#define RTL8139_INT_RX_OK      (1 << 0)
#define RTL8139_INT_RX_ERR     (1 << 1)
#define RTL8139_INT_TX_OK      (1 << 2)
#define RTL8139_INT_TX_ERR     (1 << 3)
#define RTL8139_INT_RX_OVERRUN (1 << 4)
#define RTL8139_INT_RX_UNDERRUN (1 << 5)
#define RTL8139_INT_RX_FIFO_OVERFLOW (1 << 6)
#define RTL8139_INT_PCSTIMEOUT (1 << 14)
#define RTL8139_INT_PCSC       (1 << 15)

// Driver-visible functions should not collide with kernel-level network API.
// Drivers provide device-specific probes and send/recv operations below.

// RTL8139 specific functions
net_device_t* rtl8139_probe(uint16_t io_base, uint8_t irq);
int rtl8139_init(net_device_t* dev);
int rtl8139_recv(net_device_t* dev, void* buffer, size_t len);
int rtl8139_send(net_device_t* dev, const void* data, size_t len);
void rtl8139_isr(void);

#endif // NETWORK_H
