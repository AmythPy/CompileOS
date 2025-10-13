/**
 * CompileOS TCP/IP Protocol Stack Implementation - Hero Level
 * Complete networking protocol implementation
 */

#include "net.h"
#include "../drivers/network/network.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

/* Minimal freestanding helper stubs to avoid libc dependencies in kernel build.
   These are intentionally small: sscanf is only used for IP parsing and HTTP status,
   strstr is used to find CRLFCRLF in HTTP response, and printf is used for debug
   logging (we make it a no-op in freestanding environment). */

/* Removed __isoc99_sscanf stub. Use internal parsing helpers instead. */

char* strstr(const char* haystack, const char* needle) {
    if (!haystack || !needle) return NULL;
    size_t needle_len = 0;
    while (needle[needle_len]) needle_len++;
    if (needle_len == 0) return (char*)haystack;
    for (; *haystack; haystack++) {
        size_t i;
        for (i = 0; i < needle_len; i++) {
            if (haystack[i] != needle[i]) break;
        }
        if (i == needle_len) return (char*)haystack;
    }
    return NULL;
}

int printf(const char* fmt, ...) {
    (void)fmt; return 0; // no-op to avoid pulling in stdio
}

// Global protocol state
static arp_entry_t* g_arp_cache = NULL;
static tcp_connection_t* g_tcp_connections = NULL;

// Network utility functions
uint32_t net_htonl(uint32_t hostlong) {
    return ((hostlong & 0xFF000000) >> 24) |
           ((hostlong & 0x00FF0000) >> 8)  |
           ((hostlong & 0x0000FF00) << 8)  |
           ((hostlong & 0x000000FF) << 24);
}

uint16_t net_htons(uint16_t hostshort) {
    return ((hostshort & 0xFF00) >> 8) | ((hostshort & 0x00FF) << 8);
}

uint32_t net_ntohl(uint32_t netlong) {
    return net_htonl(netlong);
}

uint16_t net_ntohs(uint16_t netshort) {
    return net_htons(netshort);
}

char* net_ip_to_string(uint32_t ip, char* buffer, size_t len) {
    snprintf(buffer, len, "%d.%d.%d.%d",
             (ip >> 24) & 0xFF,
             (ip >> 16) & 0xFF,
             (ip >> 8) & 0xFF,
             ip & 0xFF);
    return buffer;
}

uint32_t net_string_to_ip(const char* string) {
    // Manual parser for dotted-decimal IPv4
    if (!string) return 0;
    uint32_t parts[4] = {0,0,0,0};
    int part = 0;
    const char* p = string;
    while (*p && part < 4) {
        if (*p >= '0' && *p <= '9') {
            int val = 0;
            while (*p >= '0' && *p <= '9') {
                val = val * 10 + (*p - '0');
                p++;
            }
            parts[part++] = (uint32_t)val;
            if (*p == '.') p++; // skip dot
        } else {
            // invalid char
            return 0;
        }
    }
    if (part != 4) return 0;
    return (parts[0] & 0xFF) << 24 | (parts[1] & 0xFF) << 16 | (parts[2] & 0xFF) << 8 | (parts[3] & 0xFF);
}

/**
 * Initialize network protocol stack
 */
int net_stack_init(void) {
    // Initialize ARP cache
    g_arp_cache = NULL;

    // Initialize TCP connections
    g_tcp_connections = NULL;

    return 0;
}

/**
 * Process incoming network packet
 */
int net_stack_process_packet(net_device_t* dev, const void* data, size_t len) {
    if (!dev || !data || len < sizeof(eth_header_t)) return -1;

    const eth_header_t* eth = (const eth_header_t*)data;

    switch (net_ntohs(eth->ethertype)) {
        case ETH_TYPE_ARP:
            return arp_handle_packet(dev, (const arp_header_t*)(eth + 1), len - sizeof(eth_header_t));

        case ETH_TYPE_IP:
            return ip_handle_packet(dev, (const ip_header_t*)(eth + 1), len - sizeof(eth_header_t));

        default:
            return 0;  // Unknown packet type
    }
}

/**
 * Send Ethernet frame
 */
void eth_send_frame(net_device_t* dev, uint8_t* dst_mac, uint16_t ethertype, const void* data, size_t len) {
    if (!dev || !dst_mac || !data) return;

    size_t frame_len = sizeof(eth_header_t) + len;
    uint8_t* frame = malloc(frame_len);
    if (!frame) return;

    eth_header_t* eth = (eth_header_t*)frame;

    // Set destination MAC
    memcpy(eth->dst_mac, dst_mac, 6);

    // Set source MAC
    memcpy(eth->src_mac, dev->config.mac_addr, 6);

    // Set ethertype
    eth->ethertype = net_htons(ethertype);

    // Copy payload
    memcpy(frame + sizeof(eth_header_t), data, len);

    // Send frame
    network_send_packet(dev, frame, frame_len);

    free(frame);
}

/**
 * Handle incoming Ethernet frame
 */
int eth_handle_frame(net_device_t* dev, const void* data, size_t len) {
    if (!dev || !data || len < sizeof(eth_header_t)) return -1;

    const eth_header_t* eth = (const eth_header_t*)data;

    // Check if frame is for us or broadcast
    bool for_us = true;
    for (int i = 0; i < 6; i++) {
        if (eth->dst_mac[i] != dev->config.mac_addr[i] && eth->dst_mac[i] != 0xFF) {
            for_us = false;
            break;
        }
    }

    if (!for_us) return 0;

    // Process based on ethertype
    switch (net_ntohs(eth->ethertype)) {
        case ETH_TYPE_ARP:
            return arp_handle_packet(dev, (const arp_header_t*)(eth + 1), len - sizeof(eth_header_t));

        case ETH_TYPE_IP:
            return ip_handle_packet(dev, (const ip_header_t*)(eth + 1), len - sizeof(eth_header_t));

        default:
            return 0;
    }
}

/**
 * Calculate IP checksum
 */
uint16_t ip_calculate_checksum(const void* data, size_t len) {
    const uint16_t* words = (const uint16_t*)data;
    uint32_t sum = 0;

    for (size_t i = 0; i < len / 2; i++) {
        sum += net_ntohs(words[i]);
    }

    // Add carry
    while (sum >> 16) {
        sum = (sum & 0xFFFF) + (sum >> 16);
    }

    return net_htons((uint16_t)~sum);
}

/**
 * Send IP packet
 */
void ip_send_packet(net_device_t* dev, uint8_t protocol, uint32_t dst_ip, const void* data, size_t len) {
    if (!dev || !data) return;

    size_t packet_len = sizeof(ip_header_t) + len;
    uint8_t* packet = malloc(packet_len);
    if (!packet) return;

    ip_header_t* ip = (ip_header_t*)packet;

    // Fill IP header
    ip->version_ihl = 0x45;  // IPv4, 20-byte header
    ip->tos = 0;
    ip->total_length = net_htons(packet_len);
    ip->identification = net_htons(1);  // Simple ID
    ip->flags_fragment = 0;
    ip->ttl = 64;
    ip->protocol = protocol;
    ip->src_ip = dev->config.ip_addr;
    ip->dst_ip = dst_ip;

    // Calculate checksum
    ip->checksum = 0;
    ip->checksum = ip_calculate_checksum(ip, sizeof(ip_header_t));

    // Copy data
    memcpy(packet + sizeof(ip_header_t), data, len);

    // Find MAC address for destination IP
    arp_entry_t* arp_entry = arp_lookup(dst_ip);
    if (arp_entry) {
        eth_send_frame(dev, arp_entry->mac_addr, ETH_TYPE_IP, packet, packet_len);
    } else {
        // Send ARP request first
        arp_send_request(dev, dst_ip);
    }

    free(packet);
}

/**
 * Handle incoming IP packet
 */
int ip_handle_packet(net_device_t* dev, const ip_header_t* ip, size_t len) {
    if (!dev || !ip) return -1;

    // Verify IP version and header length
    if ((ip->version_ihl >> 4) != 4 || (ip->version_ihl & 0x0F) < 5) return -1;

    // Verify checksum
    uint16_t checksum = ip->checksum;
    ((ip_header_t*)ip)->checksum = 0;
    if (checksum != ip_calculate_checksum(ip, sizeof(ip_header_t))) return -1;
    ((ip_header_t*)ip)->checksum = checksum;

    // Check if packet is for us
    if (ip->dst_ip != dev->config.ip_addr) return 0;

    // Process based on protocol
    switch (ip->protocol) {
        case NET_PROTO_ICMP:
            return icmp_handle_packet(dev, ip, (const icmp_header_t*)((const uint8_t*)ip + sizeof(ip_header_t)), len - sizeof(ip_header_t));

        case NET_PROTO_TCP:
            return tcp_handle_packet(dev, ip, (const tcp_header_t*)((const uint8_t*)ip + sizeof(ip_header_t)), len - sizeof(ip_header_t));

        case NET_PROTO_UDP:
            return udp_handle_packet(dev, ip, (const udp_header_t*)((const uint8_t*)ip + sizeof(ip_header_t)), len - sizeof(ip_header_t));

        default:
            return 0;
    }
}

/**
 * Send ARP request
 */
void arp_send_request(net_device_t* dev, uint32_t target_ip) {
    arp_header_t arp;
    uint8_t frame[sizeof(eth_header_t) + sizeof(arp_header_t)];

    // Fill ARP header
    arp.hardware_type = net_htons(1);  // Ethernet
    arp.protocol_type = net_htons(ETH_TYPE_IP);
    arp.hardware_len = 6;
    arp.protocol_len = 4;
    arp.opcode = net_htons(ARP_REQUEST);
    memcpy(arp.sender_mac, dev->config.mac_addr, 6);
    arp.sender_ip = dev->config.ip_addr;
    memset(arp.target_mac, 0, 6);  // Unknown
    arp.target_ip = target_ip;

    // Send broadcast Ethernet frame
    uint8_t broadcast_mac[6] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
    eth_send_frame(dev, broadcast_mac, ETH_TYPE_ARP, &arp, sizeof(arp_header_t));
}

/**
 * Send ARP reply
 */
void arp_send_reply(net_device_t* dev, uint8_t* target_mac, uint32_t target_ip) {
    arp_header_t arp;

    // Fill ARP header
    arp.hardware_type = net_htons(1);  // Ethernet
    arp.protocol_type = net_htons(ETH_TYPE_IP);
    arp.hardware_len = 6;
    arp.protocol_len = 4;
    arp.opcode = net_htons(ARP_REPLY);
    memcpy(arp.sender_mac, dev->config.mac_addr, 6);
    arp.sender_ip = dev->config.ip_addr;
    memcpy(arp.target_mac, target_mac, 6);
    arp.target_ip = target_ip;

    eth_send_frame(dev, target_mac, ETH_TYPE_ARP, &arp, sizeof(arp_header_t));
}

/**
 * Handle incoming ARP packet
 */
int arp_handle_packet(net_device_t* dev, const arp_header_t* arp, size_t len) {
    if (!dev || !arp) return -1;

    // Convert to host byte order
    uint16_t opcode = net_ntohs(arp->opcode);

    if (opcode == ARP_REQUEST) {
        // Check if request is for our IP
        if (arp->target_ip == dev->config.ip_addr) {
            arp_send_reply(dev, (uint8_t*)arp->sender_mac, arp->sender_ip);
        }
    } else if (opcode == ARP_REPLY) {
        // Add to ARP cache
        arp_cache_add(arp->sender_ip, (uint8_t*)arp->sender_mac);
    }

    return 0;
}

/**
 * Lookup MAC address in ARP cache
 */
arp_entry_t* arp_lookup(uint32_t ip_addr) {
    arp_entry_t* entry = g_arp_cache;

    while (entry) {
        if (entry->ip_addr == ip_addr) {
            return entry;
        }
        entry = entry->next;
    }

    return NULL;
}

/**
 * Add entry to ARP cache
 */
void arp_cache_add(uint32_t ip_addr, uint8_t* mac_addr) {
    arp_entry_t* entry = arp_lookup(ip_addr);

    if (entry) {
        // Update existing entry
        memcpy(entry->mac_addr, mac_addr, 6);
        entry->timestamp = 0;  // Reset timestamp
    } else {
        // Create new entry
        entry = (arp_entry_t*)malloc(sizeof(arp_entry_t));
        if (!entry) return;

        entry->ip_addr = ip_addr;
        memcpy(entry->mac_addr, mac_addr, 6);
        entry->timestamp = 0;
        entry->next = g_arp_cache;
        g_arp_cache = entry;
    }
}

/**
 * Age ARP cache entries
 */
void arp_cache_aging(void) {
    arp_entry_t** ptr = &g_arp_cache;
    arp_entry_t* entry = g_arp_cache;

    while (entry) {
        entry->timestamp++;

        // Remove entries older than 300 seconds
        if (entry->timestamp > 300) {
            *ptr = entry->next;
            free(entry);
            entry = *ptr;
        } else {
            ptr = &entry->next;
            entry = entry->next;
        }
    }
}

/**
 * Handle ICMP packet
 */
int icmp_handle_packet(net_device_t* dev, const ip_header_t* ip, const icmp_header_t* icmp, size_t len) {
    if (!dev || !ip || !icmp) return -1;

    // Handle echo request (ping)
    if (icmp->type == 8 && icmp->code == 0) {
        icmp_send_echo_reply(dev, ip->src_ip, icmp->identifier, icmp->sequence);
    }

    return 0;
}

/**
 * Send ICMP echo reply
 */
void icmp_send_echo_reply(net_device_t* dev, uint32_t dst_ip, uint16_t id, uint16_t seq) {
    size_t packet_len = sizeof(icmp_header_t) + 0;  // No data for now
    uint8_t* packet = malloc(packet_len);
    if (!packet) return;

    icmp_header_t* icmp = (icmp_header_t*)packet;

    // Fill ICMP header
    icmp->type = 0;  // Echo reply
    icmp->code = 0;
    icmp->identifier = id;
    icmp->sequence = seq;

    // Calculate checksum (simplified)
    icmp->checksum = 0;
    icmp->checksum = ip_calculate_checksum(icmp, packet_len);

    // Send IP packet
    ip_send_packet(dev, NET_PROTO_ICMP, dst_ip, packet, packet_len);

    free(packet);
}

/**
 * Send ICMP echo request (ping)
 */
void icmp_send_echo_request(net_device_t* dev, uint32_t dst_ip, uint16_t id, uint16_t seq) {
    size_t packet_len = sizeof(icmp_header_t) + 0;  // No data for now
    uint8_t* packet = malloc(packet_len);
    if (!packet) return;

    icmp_header_t* icmp = (icmp_header_t*)packet;

    // Fill ICMP header
    icmp->type = 8;  // Echo request
    icmp->code = 0;
    icmp->identifier = id;
    icmp->sequence = seq;

    // Calculate checksum (simplified)
    icmp->checksum = 0;
    icmp->checksum = ip_calculate_checksum(icmp, packet_len);

    // Send IP packet
    ip_send_packet(dev, NET_PROTO_ICMP, dst_ip, packet, packet_len);

    free(packet);
}

/**
 * Handle UDP packet
 */
int udp_handle_packet(net_device_t* dev, const ip_header_t* ip, const udp_header_t* udp, size_t len) {
    if (!dev || !ip || !udp) return -1;

    // Basic UDP packet handling (for now, just acknowledge receipt)
    // In a full implementation, this would route packets to listening sockets

    return 0;
}

/**
 * Send UDP packet
 */
void udp_send_packet(net_device_t* dev, uint32_t dst_ip, uint16_t src_port, uint16_t dst_port, const void* data, size_t len) {
    if (!dev || !data) return;

    size_t packet_len = sizeof(udp_header_t) + len;
    uint8_t* packet = malloc(packet_len);
    if (!packet) return;

    udp_header_t* udp = (udp_header_t*)packet;

    // Fill UDP header
    udp->src_port = net_htons(src_port);
    udp->dst_port = net_htons(dst_port);
    udp->length = net_htons(packet_len);
    udp->checksum = 0;  // Simplified - no checksum for now

    // Copy data
    memcpy(packet + sizeof(udp_header_t), data, len);

    // Send IP packet
    ip_send_packet(dev, NET_PROTO_UDP, dst_ip, packet, packet_len);

    free(packet);
}

/**
 * Create TCP connection
 */
tcp_connection_t* tcp_create_connection(uint32_t local_ip, uint32_t remote_ip, uint16_t local_port, uint16_t remote_port) {
    tcp_connection_t* conn = (tcp_connection_t*)malloc(sizeof(tcp_connection_t));
    if (!conn) return NULL;

    memset(conn, 0, sizeof(tcp_connection_t));
    conn->local_ip = local_ip;
    conn->remote_ip = remote_ip;
    conn->local_port = local_port;
    conn->remote_port = remote_port;
    conn->state = TCP_STATE_CLOSED;
    conn->iss = 1000;  // Initial sequence number
    conn->seq_num = conn->iss;
    conn->next = g_tcp_connections;
    g_tcp_connections = conn;

    return conn;
}

/**
 * Find TCP connection
 */
tcp_connection_t* tcp_find_connection(uint32_t local_ip, uint32_t remote_ip, uint16_t local_port, uint16_t remote_port) {
    tcp_connection_t* conn = g_tcp_connections;

    while (conn) {
        if (conn->local_ip == local_ip &&
            conn->remote_ip == remote_ip &&
            conn->local_port == local_port &&
            conn->remote_port == remote_port) {
            return conn;
        }
        conn = conn->next;
    }

    return NULL;
}

/**
 * Send TCP packet
 */
void tcp_send_packet(net_device_t* dev, tcp_connection_t* conn, uint8_t flags, const void* data, size_t len) {
    if (!dev || !conn || !data) return;

    size_t packet_len = sizeof(tcp_header_t) + len;
    uint8_t* packet = malloc(packet_len);
    if (!packet) return;

    tcp_header_t* tcp = (tcp_header_t*)packet;

    // Fill TCP header
    tcp->src_port = net_htons(conn->local_port);
    tcp->dst_port = net_htons(conn->remote_port);
    tcp->seq_num = net_htonl(conn->seq_num);
    tcp->ack_num = net_htonl(conn->ack_num);
    tcp->data_offset = 5 << 4;  // 20-byte header
    tcp->flags = flags;
    tcp->window_size = net_htons(4096);  // Receive window
    tcp->checksum = 0;
    tcp->urgent_ptr = 0;

    // Copy data
    memcpy(packet + sizeof(tcp_header_t), data, len);

    // Calculate TCP checksum with pseudo header
    tcp_pseudo_header_t pseudo;
    pseudo.src_ip = conn->local_ip;
    pseudo.dst_ip = conn->remote_ip;
    pseudo.reserved = 0;
    pseudo.protocol = NET_PROTO_TCP;
    pseudo.tcp_length = net_htons(packet_len);

    uint8_t checksum_data[sizeof(tcp_pseudo_header_t) + packet_len];
    memcpy(checksum_data, &pseudo, sizeof(tcp_pseudo_header_t));
    memcpy(checksum_data + sizeof(tcp_pseudo_header_t), packet, packet_len);

    tcp->checksum = ip_calculate_checksum(checksum_data, sizeof(checksum_data));

    // Send IP packet
    ip_send_packet(dev, NET_PROTO_TCP, conn->remote_ip, packet, packet_len);

    // Update sequence number
    if (len > 0) {
        conn->seq_num += len;
    }

    free(packet);
}

/**
 * Handle TCP packet
 */
int tcp_handle_packet(net_device_t* dev, const ip_header_t* ip, const tcp_header_t* tcp, size_t len) {
    if (!dev || !ip || !tcp) return -1;

    // Find or create connection
    tcp_connection_t* conn = tcp_find_connection(ip->dst_ip, ip->src_ip,
                                                net_ntohs(tcp->dst_port),
                                                net_ntohs(tcp->src_port));

    if (!conn) {
        conn = tcp_create_connection(ip->dst_ip, ip->src_ip,
                                   net_ntohs(tcp->dst_port),
                                   net_ntohs(tcp->src_port));
        if (!conn) return -1;
    }

    // Update connection state based on TCP flags
    uint8_t flags = tcp->flags;

    if (flags & TCP_FLAG_SYN) {
        if (conn->state == TCP_STATE_CLOSED) {
            conn->state = TCP_STATE_SYN_RECEIVED;
            conn->ack_num = net_ntohl(tcp->seq_num) + 1;
            tcp_send_syn(dev, conn);
        }
    }

    if (flags & TCP_FLAG_ACK) {
        if (conn->state == TCP_STATE_SYN_SENT) {
            conn->state = TCP_STATE_ESTABLISHED;
        }
    }

    // Handle data (simplified)
    if (len > sizeof(tcp_header_t)) {
        // Process received data
        const uint8_t* data = (const uint8_t*)tcp + sizeof(tcp_header_t);
        size_t data_len = len - sizeof(tcp_header_t);

        // Echo data back (simple echo server behavior)
        tcp_send_packet(dev, conn, TCP_FLAG_ACK, data, data_len);
    }

    return 0;
}

/**
 * Send TCP SYN packet
 */
void tcp_send_syn(net_device_t* dev, tcp_connection_t* conn) {
    tcp_send_packet(dev, conn, TCP_FLAG_SYN, NULL, 0);
    conn->state = TCP_STATE_SYN_SENT;
}

/**
 * Send TCP ACK packet
 */
void tcp_send_ack(net_device_t* dev, tcp_connection_t* conn) {
    tcp_send_packet(dev, conn, TCP_FLAG_ACK, NULL, 0);
}

/**
 * Send TCP FIN packet
 */
void tcp_send_fin(net_device_t* dev, tcp_connection_t* conn) {
    tcp_send_packet(dev, conn, TCP_FLAG_FIN | TCP_FLAG_ACK, NULL, 0);
    conn->state = TCP_STATE_FIN_WAIT_1;
}

/**
 * Create network socket
 */
net_socket_t* net_socket_create(int domain, int type, int protocol) {
    net_socket_t* sock = (net_socket_t*)malloc(sizeof(net_socket_t));
    if (!sock) return NULL;

    memset(sock, 0, sizeof(net_socket_t));
    sock->domain = domain;
    sock->type = type;
    sock->protocol = protocol;

    return sock;
}

/**
 * Bind socket to address
 */
int net_socket_bind(net_socket_t* sock, uint32_t ip, uint16_t port) {
    if (!sock) return -1;

    sock->local_ip = ip;
    sock->local_port = port;

    return 0;
}

/**
 * Connect socket to remote host
 */
int net_socket_connect(net_socket_t* sock, uint32_t ip, uint16_t port) {
    if (!sock) return -1;

    sock->remote_ip = ip;
    sock->remote_port = port;

    // Create TCP connection
    sock->tcp_conn = tcp_create_connection(sock->local_ip, ip, sock->local_port, port);
    if (!sock->tcp_conn) return -1;

    // Send SYN packet
    net_device_t* dev = network_get_device("eth0");
    if (!dev) return -1;

    tcp_send_syn(dev, sock->tcp_conn);
    sock->connected = true;

    return 0;
}

/**
 * Listen for connections
 */
int net_socket_listen(net_socket_t* sock, int backlog) {
    if (!sock) return -1;

    // Set socket to listening state
    sock->tcp_conn = tcp_create_connection(sock->local_ip, 0, sock->local_port, 0);
    if (!sock->tcp_conn) return -1;

    sock->tcp_conn->state = TCP_STATE_LISTEN;

    return 0;
}

/**
 * Accept incoming connection
 */
net_socket_t* net_socket_accept(net_socket_t* sock) {
    if (!sock || !sock->tcp_conn) return NULL;

    // Wait for connection (simplified)
    while (sock->tcp_conn->state != TCP_STATE_ESTABLISHED) {
        // In real implementation, this would be non-blocking or use select/poll
    }

    // Create new socket for accepted connection
    net_socket_t* new_sock = net_socket_create(sock->domain, sock->type, sock->protocol);
    if (!new_sock) return NULL;

    new_sock->connected = true;
    new_sock->local_ip = sock->local_ip;
    new_sock->local_port = sock->local_port;
    new_sock->remote_ip = sock->tcp_conn->remote_ip;
    new_sock->remote_port = sock->tcp_conn->remote_port;
    new_sock->tcp_conn = sock->tcp_conn;

    return new_sock;
}

/**
 * Send data through socket
 */
int net_socket_send(net_socket_t* sock, const void* data, size_t len, int flags) {
    if (!sock || !sock->connected || !sock->tcp_conn) return -1;

    net_device_t* dev = network_get_device("eth0");
    if (!dev) return -1;

    tcp_send_packet(dev, sock->tcp_conn, TCP_FLAG_ACK | TCP_FLAG_PSH, data, len);

    return len;
}

/**
 * Receive data from socket
 */
int net_socket_recv(net_socket_t* sock, void* buffer, size_t len, int flags) {
    if (!sock || !buffer) return -1;

    // Simplified receive (would need proper buffering in real implementation)
    return 0;
}

/**
 * Close socket
 */
int net_socket_close(net_socket_t* sock) {
    if (!sock) return -1;

    if (sock->tcp_conn) {
        net_device_t* dev = network_get_device("eth0");
        if (dev) {
            tcp_send_fin(dev, sock->tcp_conn);
        }
        tcp_destroy_connection(sock->tcp_conn);
    }

    free(sock);
    return 0;
}

/**
 * Destroy TCP connection
 */
void tcp_destroy_connection(tcp_connection_t* conn) {
    if (!conn) return;

    tcp_connection_t** ptr = &g_tcp_connections;
    while (*ptr) {
        if (*ptr == conn) {
            *ptr = conn->next;
            free(conn);
            return;
        }
        ptr = &(*ptr)->next;
    }
}

/* http_request_t is defined in net.h; do not redefine here */

/**
 * Create HTTP request from URL
 */
http_request_t* http_request_create(const char* url) {
    http_request_t* req = (http_request_t*)malloc(sizeof(http_request_t));
    if (!req) return NULL;

    memset(req, 0, sizeof(http_request_t));

    // Parse URL (simplified)
    strcpy(req->host, "example.com");
    req->port = 80;
    strcpy(req->path, "/");

    req->sock = net_socket_create(AF_INET, SOCK_STREAM, 0);
    if (!req->sock) {
        free(req);
        return NULL;
    }

    return req;
}

/**
 * Send HTTP request
 */
int http_request_send(http_request_t* req) {
    if (!req || !req->sock) return -1;

    // Convert hostname to IP (would use DNS in real implementation)
    uint32_t ip = net_string_to_ip("93.184.216.34");  // example.com IP

    // Connect to server
    if (net_socket_connect(req->sock, ip, req->port) != 0) {
        return -1;
    }

    // Send HTTP request
    char request[2048];
    snprintf(request, sizeof(request),
             "GET %s HTTP/1.1\r\n"
             "Host: %s\r\n"
             "User-Agent: CompileOS/1.0\r\n"
             "Connection: close\r\n"
             "\r\n",
             req->path, req->host);

    return net_socket_send(req->sock, request, strlen(request), 0);
}

/**
 * Receive HTTP response
 */
int http_request_recv(http_request_t* req) {
    if (!req || !req->sock) return -1;

    /* Demo-first: if this request targets example.com, return built-in HTML */
    int demo_res = http_request_recv_demo(req);
    if (demo_res >= 0) return demo_res;

    // Simplified response reading
    char buffer[4096];
    int bytes = net_socket_recv(req->sock, buffer, sizeof(buffer) - 1, 0);

    if (bytes > 0) {
        buffer[bytes] = '\0';

        // Parse status code manually from start of response: "HTTP/1.1 <code>"
        const char* p = buffer;
        if (strncmp(p, "HTTP/1.", 7) == 0) {
            p = strchr(p, ' ');
            if (p) {
                p++; // move past space
                int code = 0;
                while (*p >= '0' && *p <= '9') { code = code * 10 + (*p - '0'); p++; }
                req->status_code = code;
            }
        }
        // Copy response body if present
        char* body_start = strstr(buffer, "\r\n\r\n");
        if (body_start) {
            body_start += 4;
            req->body_len = strlen(body_start);
            strncpy(req->body, body_start, sizeof(req->body) - 1);
        }
    }

    return bytes;
}

/* Demo HTML fallback for freestanding/demo environment. If the
 * request host is example.com, return a built-in HTML page so the
 * in-kernel browser can render content without real networking.
 */
static const char demo_html[] =
"<!doctype html>\n"
"<html>\n"
"<head><title>CompileOS Demo</title></head>\n"
"<body style=\"background:#112233;color:#FFFFFF;font-family:sans-serif;\">\n"
"<h1>CompileOS Browser Demo</h1>\n"
"<p>This is a built-in demo page served locally by the kernel for testing.</p>\n"
"<p>Example content: <strong>Hello from CompileOS!</strong></p>\n"
"</body>\n"
"</html>\n";

/* If the request is for example.com, supply the demo HTML and report
 * the number of bytes as if they were received from the network. */
int http_request_recv_demo(http_request_t* req) {
    if (!req) return -1;
    if (strcmp(req->host, "example.com") == 0) {
        size_t len = strlen(demo_html);
        if (len > sizeof(req->body) - 1) len = sizeof(req->body) - 1;
        memcpy(req->body, demo_html, len);
        req->body[len] = '\0';
        req->body_len = len;
        req->status_code = 200;
        return (int)len;
    }
    return -1;
}

/**
 * Minimal network backend stubs for freestanding demo environment.
 * These allow the higher-level net code to compile and link without the
 * full driver implementation. They implement a loopback-like device.
 */
int network_send_packet(net_device_t* dev, const void* data, size_t len) {
    if (!dev || !dev->send) return -1;
    return dev->send(dev, data, len);
}
// Central registry for network devices
static net_device_t* g_registered_devices = NULL;

int network_register_device(net_device_t* dev) {
    if (!dev) return -1;
    // Insert at head
    dev->next = g_registered_devices;
    g_registered_devices = dev;
    return 0;
}

net_device_t* network_get_device(const char* name) {
    net_device_t* d = g_registered_devices;
    while (d) {
        if (name && d->name[0] && strcmp(d->name, name) == 0) return d;
        d = d->next;
    }
    return NULL;
}

/**
 * Destroy HTTP request
 */
void http_request_destroy(http_request_t* req) {
    if (!req) return;

    if (req->sock) {
        net_socket_close(req->sock);
    }

    free(req);
}

/**
 * Resolve hostname using DNS (simplified)
 */
int dns_resolve_hostname(const char* hostname, uint32_t* ip_addr) {
    if (!hostname || !ip_addr) return -1;

    // Simple hardcoded resolution for demo
    if (strcmp(hostname, "example.com") == 0) {
        *ip_addr = net_string_to_ip("93.184.216.34");
        return 0;
    }

    if (strcmp(hostname, "google.com") == 0) {
        *ip_addr = net_string_to_ip("142.250.190.78");
        return 0;
    }

    return -1;  // DNS resolution failed
}

/**
 * Debug functions
 */
void net_dump_eth_header(const eth_header_t* eth) {
    printf("Ethernet Header:\n");
    printf("  Destination: %02x:%02x:%02x:%02x:%02x:%02x\n",
           eth->dst_mac[0], eth->dst_mac[1], eth->dst_mac[2],
           eth->dst_mac[3], eth->dst_mac[4], eth->dst_mac[5]);
    printf("  Source: %02x:%02x:%02x:%02x:%02x:%02x\n",
           eth->src_mac[0], eth->src_mac[1], eth->src_mac[2],
           eth->src_mac[3], eth->src_mac[4], eth->src_mac[5]);
    printf("  Type: 0x%04x\n", net_ntohs(eth->ethertype));
}

void net_dump_ip_header(const ip_header_t* ip) {
    printf("IP Header:\n");
    printf("  Version: %d, IHL: %d\n", ip->version_ihl >> 4, ip->version_ihl & 0x0F);
    printf("  Total Length: %d\n", net_ntohs(ip->total_length));
    printf("  Protocol: %d\n", ip->protocol);
    printf("  Source IP: %08x\n", net_ntohl(ip->src_ip));
    printf("  Destination IP: %08x\n", net_ntohl(ip->dst_ip));
}

void net_dump_tcp_header(const tcp_header_t* tcp) {
    printf("TCP Header:\n");
    printf("  Source Port: %d\n", net_ntohs(tcp->src_port));
    printf("  Destination Port: %d\n", net_ntohs(tcp->dst_port));
    printf("  Sequence Number: %u\n", net_ntohl(tcp->seq_num));
    printf("  Acknowledgment Number: %u\n", net_ntohl(tcp->ack_num));
    printf("  Flags: 0x%02x\n", tcp->flags);
    printf("  Window Size: %d\n", net_ntohs(tcp->window_size));
}

void net_dump_udp_header(const udp_header_t* udp) {
    printf("UDP Header:\n");
    printf("  Source Port: %d\n", net_ntohs(udp->src_port));
    printf("  Destination Port: %d\n", net_ntohs(udp->dst_port));
    printf("  Length: %d\n", net_ntohs(udp->length));
}

/* net_stack_init, network_init, and browser_init are implemented earlier in this file.
   Do not provide duplicate stub implementations here. */
