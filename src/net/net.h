/**
 * CompileOS TCP/IP Protocol Stack - Hero Level
 * Complete networking protocol implementation
 */

#ifndef NET_H
#define NET_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include "../drivers/network/network.h"

// Network protocol numbers
#define NET_PROTO_ICMP 1
#define NET_PROTO_TCP  6
#define NET_PROTO_UDP  17

// Ethernet frame types
#define ETH_TYPE_IP    0x0800
#define ETH_TYPE_ARP   0x0806
#define ETH_TYPE_IPV6  0x86DD

// ARP opcodes
#define ARP_REQUEST 1
#define ARP_REPLY   2

// TCP flags
#define TCP_FLAG_FIN 0x01
#define TCP_FLAG_SYN 0x02
#define TCP_FLAG_RST 0x04
#define TCP_FLAG_PSH 0x08
#define TCP_FLAG_ACK 0x10
#define TCP_FLAG_URG 0x20

// TCP states
typedef enum {
    TCP_STATE_CLOSED = 0,
    TCP_STATE_LISTEN,
    TCP_STATE_SYN_SENT,
    TCP_STATE_SYN_RECEIVED,
    TCP_STATE_ESTABLISHED,
    TCP_STATE_FIN_WAIT_1,
    TCP_STATE_FIN_WAIT_2,
    TCP_STATE_CLOSE_WAIT,
    TCP_STATE_CLOSING,
    TCP_STATE_LAST_ACK,
    TCP_STATE_TIME_WAIT
} tcp_state_t;

// Ethernet header
typedef struct {
    uint8_t dst_mac[6];
    uint8_t src_mac[6];
    uint16_t ethertype;
} __attribute__((packed)) eth_header_t;

// ARP header
typedef struct {
    uint16_t hardware_type;
    uint16_t protocol_type;
    uint8_t hardware_len;
    uint8_t protocol_len;
    uint16_t opcode;
    uint8_t sender_mac[6];
    uint32_t sender_ip;
    uint8_t target_mac[6];
    uint32_t target_ip;
} __attribute__((packed)) arp_header_t;

// IP header
typedef struct {
    uint8_t version_ihl;
    uint8_t tos;
    uint16_t total_length;
    uint16_t identification;
    uint16_t flags_fragment;
    uint8_t ttl;
    uint8_t protocol;
    uint16_t checksum;
    uint32_t src_ip;
    uint32_t dst_ip;
} __attribute__((packed)) ip_header_t;

// ICMP header
typedef struct {
    uint8_t type;
    uint8_t code;
    uint16_t checksum;
    uint16_t identifier;
    uint16_t sequence;
} __attribute__((packed)) icmp_header_t;

// UDP header
typedef struct {
    uint16_t src_port;
    uint16_t dst_port;
    uint16_t length;
    uint16_t checksum;
} __attribute__((packed)) udp_header_t;

// TCP header
typedef struct {
    uint16_t src_port;
    uint16_t dst_port;
    uint32_t seq_num;
    uint32_t ack_num;
    uint8_t data_offset;
    uint8_t flags;
    uint16_t window_size;
    uint16_t checksum;
    uint16_t urgent_ptr;
} __attribute__((packed)) tcp_header_t;

// TCP pseudo header for checksum calculation
typedef struct {
    uint32_t src_ip;
    uint32_t dst_ip;
    uint8_t reserved;
    uint8_t protocol;
    uint16_t tcp_length;
} __attribute__((packed)) tcp_pseudo_header_t;

// TCP connection tracking
typedef struct tcp_connection_t {
    uint32_t local_ip;
    uint32_t remote_ip;
    uint16_t local_port;
    uint16_t remote_port;
    tcp_state_t state;
    uint32_t seq_num;
    uint32_t ack_num;
    uint32_t snd_una;      // Send unacknowledged
    uint32_t snd_nxt;      // Send next
    uint32_t snd_wnd;      // Send window
    uint32_t rcv_nxt;      // Receive next
    uint32_t rcv_wnd;      // Receive window
    uint32_t iss;          // Initial send sequence number
    struct tcp_connection_t* next;
} tcp_connection_t;

// Network socket structure
typedef struct {
    int domain;
    int type;
    int protocol;
    bool connected;
    uint32_t local_ip;
    uint16_t local_port;
    uint32_t remote_ip;
    uint16_t remote_port;
    tcp_connection_t* tcp_conn;
    void* private_data;
} net_socket_t;

// ARP cache entry
typedef struct arp_entry_t {
    uint32_t ip_addr;
    uint8_t mac_addr[6];
    uint32_t timestamp;
    struct arp_entry_t* next;
} arp_entry_t;

// Protocol stack functions
int net_stack_init(void);
int net_stack_process_packet(net_device_t* dev, const void* data, size_t len);

// Ethernet functions
void eth_send_frame(net_device_t* dev, uint8_t* dst_mac, uint16_t ethertype, const void* data, size_t len);
int eth_handle_frame(net_device_t* dev, const void* data, size_t len);

// ARP functions
void arp_send_request(net_device_t* dev, uint32_t target_ip);
void arp_send_reply(net_device_t* dev, uint8_t* target_mac, uint32_t target_ip);
int arp_handle_packet(net_device_t* dev, const arp_header_t* arp, size_t len);
arp_entry_t* arp_lookup(uint32_t ip_addr);
void arp_cache_add(uint32_t ip_addr, uint8_t* mac_addr);
void arp_cache_aging(void);

// IP functions
uint16_t ip_calculate_checksum(const void* data, size_t len);
void ip_send_packet(net_device_t* dev, uint8_t protocol, uint32_t dst_ip, const void* data, size_t len);
int ip_handle_packet(net_device_t* dev, const ip_header_t* ip, size_t len);

// ICMP functions
void icmp_send_echo_reply(net_device_t* dev, uint32_t dst_ip, uint16_t id, uint16_t seq);
void icmp_send_echo_request(net_device_t* dev, uint32_t dst_ip, uint16_t id, uint16_t seq);
int icmp_handle_packet(net_device_t* dev, const ip_header_t* ip, const icmp_header_t* icmp, size_t len);

// UDP functions
void udp_send_packet(net_device_t* dev, uint32_t dst_ip, uint16_t src_port, uint16_t dst_port, const void* data, size_t len);
int udp_handle_packet(net_device_t* dev, const ip_header_t* ip, const udp_header_t* udp, size_t len);

// TCP functions
tcp_connection_t* tcp_create_connection(uint32_t local_ip, uint32_t remote_ip, uint16_t local_port, uint16_t remote_port);
void tcp_destroy_connection(tcp_connection_t* conn);
void tcp_send_packet(net_device_t* dev, tcp_connection_t* conn, uint8_t flags, const void* data, size_t len);
void tcp_send_syn(net_device_t* dev, tcp_connection_t* conn);
void tcp_send_ack(net_device_t* dev, tcp_connection_t* conn);
void tcp_send_fin(net_device_t* dev, tcp_connection_t* conn);
int tcp_handle_packet(net_device_t* dev, const ip_header_t* ip, const tcp_header_t* tcp, size_t len);
tcp_connection_t* tcp_find_connection(uint32_t local_ip, uint32_t remote_ip, uint16_t local_port, uint16_t remote_port);

// Socket API
net_socket_t* net_socket_create(int domain, int type, int protocol);
int net_socket_bind(net_socket_t* sock, uint32_t ip, uint16_t port);
int net_socket_connect(net_socket_t* sock, uint32_t ip, uint16_t port);
int net_socket_listen(net_socket_t* sock, int backlog);
net_socket_t* net_socket_accept(net_socket_t* sock);
int net_socket_send(net_socket_t* sock, const void* data, size_t len, int flags);
int net_socket_recv(net_socket_t* sock, void* buffer, size_t len, int flags);
int net_socket_close(net_socket_t* sock);

// Minimal socket constants for freestanding build
#ifndef AF_INET
#define AF_INET 2
#endif
#ifndef SOCK_STREAM
#define SOCK_STREAM 1
#endif

// HTTP client
typedef struct {
    char host[256];
    uint16_t port;
    char path[1024];
    char headers[4096];
    char body[8192];
    size_t body_len;
    int status_code;
    net_socket_t* sock;
} http_request_t;

http_request_t* http_request_create(const char* url);
int http_request_send(http_request_t* req);
int http_request_recv(http_request_t* req);
void http_request_destroy(http_request_t* req);

// DNS client
int dns_resolve_hostname(const char* hostname, uint32_t* ip_addr);

// Network utilities
uint32_t net_htonl(uint32_t hostlong);
uint16_t net_htons(uint16_t hostshort);
uint32_t net_ntohl(uint32_t netlong);
uint16_t net_ntohs(uint16_t netshort);
char* net_ip_to_string(uint32_t ip, char* buffer, size_t len);
uint32_t net_string_to_ip(const char* string);

// Network debugging
void net_dump_eth_header(const eth_header_t* eth);
void net_dump_ip_header(const ip_header_t* ip);
void net_dump_tcp_header(const tcp_header_t* tcp);
void net_dump_udp_header(const udp_header_t* udp);

// Central device registry (drivers should call this to register devices)
int network_register_device(net_device_t* dev);
net_device_t* network_get_device(const char* name);
// Send/receive wrapper used by protocol stack to hand frames to devices
int network_send_packet(net_device_t* dev, const void* data, size_t len);
int network_recv_packet(net_device_t* dev, void* buffer, size_t len);


#endif // NET_H
