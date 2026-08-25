// F040.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct packet {
    char *id;
    int hlen;
    int plen;
    char *header;
    char *payload;
    struct packet *next;
} Packet;

Packet *packets = NULL;

Packet *find_packet(const char *id) {
    Packet *p = packets;
    while (p) {
        if (strcmp(p->id, id) == 0) return p;
        p = p->next;
    }
    return NULL;
}

void store_packet(const char *id, int hlen, int plen, const char *header, const char *payload) {
    if (find_packet(id)) return;
    if (hlen < 0 || plen < 0 || (int)strlen(header) != hlen || (int)strlen(payload) != plen) return;
    Packet *p = malloc(sizeof(Packet));
    p->id = strdup(id);
    p->hlen = hlen;
    p->plen = plen;
    p->header = strdup(header);
    p->payload = strdup(payload);
    p->next = packets;
    packets = p;
}

void drop_packet(const char *id) {
    Packet *p = packets, *prev = NULL;
    while (p) {
        if (strcmp(p->id, id) == 0) {
            if (prev) prev->next = p->next;
            else packets = p->next;
            free(p->id);
            free(p->header);
            free(p->payload);
            free(p);
            return;
        }
        prev = p;
        p = p->next;
    }
}

void header_packet(const char *id) {
    Packet *p = find_packet(id);
    if (p) printf("%s\n", p->header);
}

void payload_packet(const char *id) {
    Packet *p = find_packet(id);
    if (p) printf("%s\n", p->payload);
}

void stats_packets(void) {
    int total = 0;
    Packet *p = packets;
    while (p) {
        total += p->plen;
        p = p->next;
    }
    printf("%d\n", total);
}

int main(void) {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        if (strcmp(line, "END\n") == 0 || strcmp(line, "END") == 0) break;
        char cmd[32];
        char id[256];
        int hlen, plen;
        char header[4096], payload[4096];
        if (sscanf(line, "%31s", cmd) != 1) continue;
        if (strcmp(cmd, "PACKET") == 0) {
            if (sscanf(line, "%*s %255s %d %d %[^ ] %[^\n]", id, &hlen, &plen, header, payload) == 5)
                store_packet(id, hlen, plen, header, payload);
        } else if (strcmp(cmd, "DROP") == 0) {
            if (sscanf(line, "%*s %255s", id) == 1)
                drop_packet(id);
        } else if (strcmp(cmd, "HEADER") == 0) {
            if (sscanf(line, "%*s %255s", id) == 1)
                header_packet(id);
        } else if (strcmp(cmd, "PAYLOAD") == 0) {
            if (sscanf(line, "%*s %255s", id) == 1)
                payload_packet(id);
        } else if (strcmp(cmd, "STATS") == 0) {
            stats_packets();
        }
    }
    return 0;
}