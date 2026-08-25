// F040.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

char* read_line() {
    size_t cap = 256, len = 0;
    char *buf = malloc(cap);
    if (!buf) return NULL;
    int c;
    while ((c = getchar()) != EOF && c != '\n') {
        if (len + 1 >= cap) {
            if (cap > SIZE_MAX / 2) { free(buf); return NULL; }
            cap *= 2;
            char *tmp = realloc(buf, cap);
            if (!tmp) { free(buf); return NULL; }
            buf = tmp;
        }
        buf[len++] = (char)c;
    }
    if (len == 0 && c == EOF) { free(buf); return NULL; }
    buf[len] = '\0';
    return buf;
}

typedef struct { char *id; char *header; char *payload; } Pkt;
Pkt *pkts = NULL;
size_t pc = 0, pcap = 0;

void add_pkt(const char *id, size_t hlen, size_t plen, const char *header, const char *payload) {
    if (strlen(header) != hlen || strlen(payload) != plen) return;
    if (pc == pcap) {
        size_t nc = pcap ? pcap * 2 : 4;
        if (nc > SIZE_MAX / sizeof(Pkt)) { fprintf(stderr, "Overflow\n"); exit(1); }
        Pkt *tmp = realloc(pkts, nc * sizeof(Pkt));
        if (!tmp) { perror("realloc"); exit(1); }
        pkts = tmp;
        pcap = nc;
    }
    pkts[pc].id = strdup(id);
    pkts[pc].header = strdup(header);
    pkts[pc].payload = strdup(payload);
    if (!pkts[pc].id || !pkts[pc].header || !pkts[pc].payload) { perror("strdup"); exit(1); }
    pc++;
}

void drop_pkt(const char *id) {
    for (size_t i = 0; i < pc; i++) {
        if (strcmp(pkts[i].id, id) == 0) {
            free(pkts[i].id); free(pkts[i].header); free(pkts[i].payload);
            pkts[i] = pkts[--pc];
            return;
        }
    }
}

int main() {
    char *line;
    while ((line = read_line()) != NULL) {
        if (strcmp(line, "END") == 0) { free(line); break; }
        char cmd[32];
        if (sscanf(line, "%31s", cmd) != 1) { free(line); continue; }
        if (strcmp(cmd, "PACKET") == 0) {
            char id[256], header[1024], payload[1024];
            size_t hlen, plen;
            if (sscanf(line, "%*s %255s %zu %zu %1023s %1023s", id, &hlen, &plen, header, payload) == 5) {
                add_pkt(id, hlen, plen, header, payload);
            }
        } else if (strcmp(cmd, "DROP") == 0) {
            char id[256];
            if (sscanf(line, "%*s %255s", id) == 1) drop_pkt(id);
        } else if (strcmp(cmd, "HEADER") == 0) {
            char id[256];
            if (sscanf(line, "%*s %255s", id) == 1) {
                for (size_t i = 0; i < pc; i++) {
                    if (strcmp(pkts[i].id, id) == 0) { printf("%s\n", pkts[i].header); break; }
                }
            }
        } else if (strcmp(cmd, "PAYLOAD") == 0) {
            char id[256];
            if (sscanf(line, "%*s %255s", id) == 1) {
                for (size_t i = 0; i < pc; i++) {
                    if (strcmp(pkts[i].id, id) == 0) { printf("%s\n", pkts[i].payload); break; }
                }
            }
        } else if (strcmp(cmd, "STATS") == 0) {
            size_t total = 0;
            for (size_t i = 0; i < pc; i++) total += strlen(pkts[i].payload);
            printf("%zu\n", total);
        }
        free(line);
    }
    for (size_t i = 0; i < pc; i++) { free(pkts[i].id); free(pkts[i].header); free(pkts[i].payload); }
    free(pkts);
    return 0;
}