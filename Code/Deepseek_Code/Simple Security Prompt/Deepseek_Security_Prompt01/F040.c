// F040.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct { char *id; int hlen; int plen; char *header; char *payload; } Packet;
Packet *packets = NULL;
int pc = 0, pcap = 0;

int find_packet(const char *id) {
    for (int i = 0; i < pc; i++)
        if (strcmp(packets[i].id, id) == 0) return i;
    return -1;
}

void add_packet(const char *id, int hlen, int plen, const char *header, const char *payload) {
    if (find_packet(id) >= 0) return;
    if (pc == pcap) { pcap = pcap ? pcap * 2 : 4; packets = realloc(packets, sizeof(Packet) * pcap); }
    packets[pc].id = strdup(id);
    packets[pc].hlen = hlen;
    packets[pc].plen = plen;
    packets[pc].header = strdup(header);
    packets[pc].payload = strdup(payload);
    pc++;
}

void drop_packet(const char *id) {
    int idx = find_packet(id);
    if (idx < 0) return;
    free(packets[idx].id);
    free(packets[idx].header);
    free(packets[idx].payload);
    for (int i = idx; i < pc - 1; i++) packets[i] = packets[i + 1];
    pc--;
}

void print_header(const char *id) {
    int idx = find_packet(id);
    if (idx < 0) return;
    printf("%s\n", packets[idx].header);
}

void print_payload(const char *id) {
    int idx = find_packet(id);
    if (idx < 0) return;
    printf("%s\n", packets[idx].payload);
}

void stats(void) {
    int total = 0;
    for (int i = 0; i < pc; i++) total += packets[i].plen;
    printf("%d\n", total);
}

int main(void) {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        char cmd[32], id[256], header[4096], payload[4096];
        int hlen, plen;
        if (sscanf(line, "%31s", cmd) != 1) continue;
        if (strcmp(cmd, "PACKET") == 0) {
            if (sscanf(line, "%*s %255s %d %d %4095[^ ] %4095[^\n]", id, &hlen, &plen, header, payload) == 5) {
                // header and payload may contain spaces? Actually header is one string without spaces? The prompt says header and payload are strings, but fields are separated by spaces, so they cannot contain spaces. We'll use simple sscanf.
                add_packet(id, hlen, plen, header, payload);
            } else {
                // Try alternative if header/payload have spaces? Not possible with sscanf.
            }
        } else if (strcmp(cmd, "DROP") == 0) {
            if (sscanf(line, "%*s %255s", id) == 1) drop_packet(id);
        } else if (strcmp(cmd, "HEADER") == 0) {
            if (sscanf(line, "%*s %255s", id) == 1) print_header(id);
        } else if (strcmp(cmd, "PAYLOAD") == 0) {
            if (sscanf(line, "%*s %255s", id) == 1) print_payload(id);
        } else if (strcmp(cmd, "STATS") == 0) stats();
    }
    for (int i = 0; i < pc; i++) {
        free(packets[i].id);
        free(packets[i].header);
        free(packets[i].payload);
    }
    free(packets);
    return 0;
}