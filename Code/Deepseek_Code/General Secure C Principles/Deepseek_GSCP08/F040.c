// F040.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Packet { char *id; int hlen; int plen; char *header; char *payload; struct Packet *next; } Packet;
Packet *packets = NULL;

Packet *find_packet(const char *id) {
    for (Packet *p = packets; p; p = p->next)
        if (strcmp(p->id, id) == 0) return p;
    return NULL;
}

void packet_add(const char *id, int hlen, int plen, const char *header, const char *payload) {
    if (find_packet(id)) return;
    if (hlen < 0 || plen < 0 || (int)strlen(header) != hlen || (int)strlen(payload) != plen) return;
    Packet *p = malloc(sizeof(Packet));
    if (!p) return;
    p->id = strdup(id);
    p->hlen = hlen;
    p->plen = plen;
    p->header = strdup(header);
    p->payload = strdup(payload);
    p->next = packets;
    packets = p;
}

void drop_packet(const char *id) {
    Packet *prev = NULL;
    for (Packet *p = packets; p; prev = p, p = p->next) {
        if (strcmp(p->id, id) == 0) {
            if (prev) prev->next = p->next; else packets = p->next;
            free(p->id); free(p->header); free(p->payload); free(p);
            return;
        }
    }
}

void print_header(const char *id) {
    Packet *p = find_packet(id);
    if (!p) { printf("NOT_FOUND\n"); return; }
    printf("%s\n", p->header);
}

void print_payload(const char *id) {
    Packet *p = find_packet(id);
    if (!p) { printf("NOT_FOUND\n"); return; }
    printf("%s\n", p->payload);
}

void stats_packets(void) {
    int total = 0;
    for (Packet *p = packets; p; p = p->next)
        total += p->plen;
    printf("%d\n", total);
}

int main() {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        if (strcmp(line, "END") == 0) break;
        char cmd[16];
        if (sscanf(line, "%15s", cmd) != 1) continue;
        char *rest = line + strlen(cmd);
        while (*rest == ' ') rest++;
        if (strcmp(cmd, "PACKET") == 0) {
            char id[256]; int hlen, plen; char header[4096], payload[4096];
            if (sscanf(rest, "%255s %d %d", id, &hlen, &plen) == 3) {
                char *h = rest;
                while (*h == ' ') h++;
                while (*h && *h != ' ') h++;
                while (*h == ' ') h++;
                while (*h && *h != ' ') h++;
                while (*h == ' ') h++;
                while (*h && *h != ' ') h++;
                while (*h == ' ') h++;
                // header is next hlen chars
                int hlen_actual = 0;
                char *hstart = h;
                while (hlen_actual < hlen && *h && *h != ' ') { h++; hlen_actual++; }
                if (hlen_actual != hlen) continue;
                strncpy(header, hstart, hlen);
                header[hlen] = 0;
                while (*h == ' ') h++;
                // payload is rest up to plen
                int plen_actual = 0;
                char *pstart = h;
                while (plen_actual < plen && *h) { h++; plen_actual++; }
                if (plen_actual != plen) continue;
                strncpy(payload, pstart, plen);
                payload[plen] = 0;
                packet_add(id, hlen, plen, header, payload);
            }
        } else if (strcmp(cmd, "DROP") == 0) {
            char id[256];
            if (sscanf(rest, "%255s", id) == 1) drop_packet(id);
        } else if (strcmp(cmd, "HEADER") == 0) {
            char id[256];
            if (sscanf(rest, "%255s", id) == 1) print_header(id);
        } else if (strcmp(cmd, "PAYLOAD") == 0) {
            char id[256];
            if (sscanf(rest, "%255s", id) == 1) print_payload(id);
        } else if (strcmp(cmd, "STATS") == 0) {
            stats_packets();
        }
    }
    while (packets) { Packet *p = packets; packets = p->next; free(p->id); free(p->header); free(p->payload); free(p); }
    return 0;
}