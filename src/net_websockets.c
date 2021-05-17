//
// Copyright(C) 2021 Cloudflare - celso@cloudflare.com
//
// DESCRIPTION:
//      Websockets network module for Chocolate Doom Wasm

#include <stdio.h>
#include <stdlib.h>

#include "debug.h"
#include "doomtype.h"
#include "i_system.h"
#include "m_argv.h"
#include "m_misc.h"
#include "net_defs.h"
#include "net_packet.h"
#include "net_websockets.h"
#include "z_zone.h"
#include <emscripten/websocket.h>

#define MAX_QUEUE_SIZE 64

extern uint32_t instanceUID;

// WebSockets packet queue
typedef struct {
    net_packet_t *packets[MAX_QUEUE_SIZE];
    uint32_t froms[MAX_QUEUE_SIZE];
    int head, tail;
} packet_queue_t;

typedef struct {
    net_packet_t *packet;
    uint32_t *from;
} ws_packet_t;

static ws_packet_t wsp;

static packet_queue_t client_queue;

// addresses table
static int addrs_index = 0;
net_addr_t addrs[MAX_QUEUE_SIZE];
uint32_t ips[MAX_QUEUE_SIZE];

static void WebsocketsQueueInit(packet_queue_t *queue) { queue->head = queue->tail = 0; }

// Pushes one packet into the queue
static void WebsocketsQueuePush(packet_queue_t *queue, net_packet_t *packet, uint32_t from)
{
    int new_tail;

    new_tail = (queue->tail + 1) % MAX_QUEUE_SIZE;

    if (new_tail == queue->head) {
        // queue is full

        return;
    }

    queue->packets[queue->tail] = packet;
    queue->froms[queue->tail] = from;
    queue->tail = new_tail;
}

// WebSockets code

static EMSCRIPTEN_WEBSOCKET_T websocket;
static EmscriptenWebSocketCreateAttributes attr;
static boolean inittedWebSockets = false;

// Pops one packet from the queue
static ws_packet_t *WebsocketsQueuePop(packet_queue_t *queue)
{
    if (queue->tail == queue->head) {
        // queue empty
        return NULL;
    }

    wsp.packet = queue->packets[queue->head];
    wsp.from = &queue->froms[queue->head];
    queue->head = (queue->head + 1) % MAX_QUEUE_SIZE;

    return &wsp;
}

EM_BOOL WebSocketOpen(int eventType, const EmscriptenWebSocketOpenEvent *e, void *userData) { return 0; }

EM_BOOL WebSocketClose(int eventType, const EmscriptenWebSocketCloseEvent *e, void *userData)
{
    printf("doom: 5, ws close(eventType=%d, wasClean=%d, code=%d, reason=%s, userData=%d)\n", eventType, e->wasClean,
           e->code, e->reason, (int)userData);
    inittedWebSockets = false;
    return 0;
}

EM_BOOL WebSocketError(int eventType, const EmscriptenWebSocketErrorEvent *e, void *userData)
{
    printf("doom: 4, ws error(eventType=%d, userData=%d)\n", eventType, (int)userData);
    return 0;
}

EM_BOOL WebSocketMessage(int eventType, const EmscriptenWebSocketMessageEvent *e, void *userData)
{
    // printf("message(eventType=%d, userData=%d, data=%p, numBytes=%d, isText=%d)\n", eventType, (int)userData,
    // e->data, e->numBytes, e->isText);

    net_packet_t *packet;

    packet = NET_NewPacket(e->numBytes - 4);
    memcpy(packet->data, &e->data[4], e->numBytes - 4);
    packet->len = e->numBytes - 4;

    uint32_t ip = 0;
    ip = ip | *(&e->data[3]) << 24;
    ip = ip | *(&e->data[2]) << 16;
    ip = ip | *(&e->data[1]) << 8;
    ip = ip | *(&e->data[0]);

    WebsocketsQueuePush(&client_queue, packet, ip);

    return 0;
}

static boolean InitWebSockets(void)
{
    int wss;

    if (inittedWebSockets == true) {
        return (true);
    }

    wss = M_CheckParmWithArgs("-wss", 1);
    printf("%d\n", wss);

    if (wss) {
        emscripten_websocket_init_create_attributes(&attr);

        attr.url = myargv[wss + 1];
        // this doesn't work with some ws servers
        // attr.protocols = "binary";

        websocket = emscripten_websocket_new(&attr);
        if (websocket <= 0) {
            return (false);
        }

        emscripten_websocket_set_onopen_callback(websocket, (void *)42, WebSocketOpen);
        emscripten_websocket_set_onclose_callback(websocket, (void *)43, WebSocketClose);
        emscripten_websocket_set_onerror_callback(websocket, (void *)44, WebSocketError);
        emscripten_websocket_set_onmessage_callback(websocket, (void *)45, WebSocketMessage);

        uint16_t readyState = 0;
        int retries = 6;
        do {
            emscripten_websocket_get_ready_state(websocket, &readyState);
            emscripten_sleep(1000);
            if (retries-- == 0 && readyState == 0) {
                printf("doom: 1, failed to connect to websockets server\n");
                return (false);
            }
        } while (readyState == 0);

        if (readyState == 1) {
            printf("doom: 2, connected to %s\n", attr.url);
            inittedWebSockets = true;
            return (true);
        }
        else {
            printf("doom: 1, failed to connect to websockets server\n");
            return (false);
        }
    }
    else {
        printf("-wss is missing\n");
        return (false);
    }
}

static boolean NET_Websockets_InitClient(void)
{
    if (InitWebSockets() == false) return false;
    WebsocketsQueueInit(&client_queue);
    return true;
}

static uint32_t to_ip;

static boolean NET_Websockets_InitServer(void)
{
    if (InitWebSockets() == false) return false;
    WebsocketsQueueInit(&client_queue);
    char *wspacket = malloc(8);
    to_ip = 0;
    memcpy(&wspacket[0], &to_ip, 4);       // to
    memcpy(&wspacket[4], &instanceUID, 4); // from
    emscripten_websocket_send_binary(websocket, wspacket, 8);
    free(wspacket);
    return true;
}

static void NET_Websockets_SendPacket(net_addr_t *addr, net_packet_t *packet)
{
    if (InitWebSockets() == false) return;
    char *wspacket = malloc(packet->len + 8);

    if (addr->handle) {
        to_ip = (*(uint32_t *)(addr->handle));
        memcpy(&wspacket[0], &to_ip, 4);       // to
        memcpy(&wspacket[4], &instanceUID, 4); // from
        memcpy(&wspacket[8], packet->data, packet->len);
        int r = emscripten_websocket_send_binary(websocket, wspacket, packet->len + 8);
        if (r < 0) {
            printf("doom: 6, failed to send ws packet, reconnecting");
            inittedWebSockets = false;
        }
        free(wspacket);
    }
}

static net_addr_t *FindAddressByIp(uint32_t ip)
{
    // looks for address in the addresses table, or create a new one
    // this should be a circular queue, but it's not, it's an hacky version
    // i'm assuming there won't be more than MAX_QUEUE_SIZE address during one gaming session

    // do we have it?
    for (int i = 0; i < MAX_QUEUE_SIZE; i++) {
        if (ips[i] == ip) {
            return (&addrs[i]);
        }
    }
    // nope, we need a new one
    if (addrs_index >= MAX_QUEUE_SIZE) {
        printf("doom: 3, we're out of client addresses");
        return (0);
    }
    else {
        addrs[addrs_index].refcount = 1;
        addrs[addrs_index].module = &net_websockets_module;
        ips[addrs_index] = ip;
        addrs[addrs_index].handle = &ips[addrs_index];
        return (&addrs[addrs_index++]);
    }
}

static boolean NET_Websockets_RecvPacket(net_addr_t **addr, net_packet_t **packet)
{
    ws_packet_t *popped;

    if (InitWebSockets() == false) return false;

    popped = WebsocketsQueuePop(&client_queue);

    if (popped != NULL) {
        *packet = popped->packet;
        *addr = FindAddressByIp((*(uint32_t *)(popped->from)));
        return true;
    }

    return false;
}

static void NET_Websockets_AddrToString(net_addr_t *addr, char *buffer, int buffer_len)
{
    M_snprintf(buffer, buffer_len, "ws client %u", (*(uint32_t *)(addr->handle)));
}

static void NET_Websockets_FreeAddress(net_addr_t *addr) { free(addr); }

// this is only used to resolve the server address - hacky
static net_addr_t *NET_Websockets_ResolveAddress(const char *address)
{
    return FindAddressByIp((uint32_t)atoi(address));
}

net_module_t net_websockets_module = {
    NET_Websockets_InitClient,   NET_Websockets_InitServer,  NET_Websockets_SendPacket,     NET_Websockets_RecvPacket,
    NET_Websockets_AddrToString, NET_Websockets_FreeAddress, NET_Websockets_ResolveAddress,
};

