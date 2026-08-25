/* F040.c */
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
int total_payload = 0;

Packet *find_packet(const char *id) {
    Packet *p = packets;
    while (p) {
        if (strcmp(p->id, id) == 0) return p;
        p = p->next;
    }
    return NULL;
}

void add_packet(const char *id, int hlen, int plen, const char *header, const char *payload) {
    if (find_packet(id)) return;
    if (hlen < 0 || plen < 0) return;
    if ((int)strlen(header) != hlen || (int)strlen(payload) != plen) return;
    Packet *p = malloc(sizeof(Packet));
    p->id = strdup(id);
    p->hlen = hlen;
    p->plen = plen;
    p->header = strdup(header);
    p->payload = strdup(payload);
    p->next = packets;
    packets = p;
    total_payload += plen;
}

void drop_packet(const char *id) {
    Packet *p = packets, *prev = NULL;
    while (p) {
        if (strcmp(p->id, id) == 0) {
            if (prev) prev->next = p->next;
            else packets = p->next;
            total_payload -= p->plen;
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

void print_header(const char *id) {
    Packet *p = find_packet(id);
    if (!p) { printf("NOT_FOUND\n"); return; }
    for (char *c = p->header; *c; c++) putchar(*c == ' ' ? '_' : *c);
    putchar('\n');
}

void print_payload(const char *id) {
    Packet *p = find_packet(id);
    if (!p) { printf("NOT_FOUND\n"); return; }
    for (char *c = p->payload; *c; c++) putchar(*c == ' ' ? '_' : *c);
    putchar('\n');
}

void stats() {
    printf("%d\n", total_payload);
}

int main() {
    char line[1024];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        char cmd[16], id[256], header[1024], payload[1024];
        int hlen, plen;
        if (sscanf(line, "%15s", cmd) < 1) continue;
        if (strcmp(cmd, "STATS") == 0) { stats(); continue; }
        if (strcmp(cmd, "PACKET") == 0) {
            if (sscanf(line, "%*s %255s %d %d", id, &hlen, &plen) < 3) continue;
            char *rest = line + strlen(cmd) + 1 + strlen(id) + 1 + (hlen >= 0 ? snprintf(NULL,0,"%d",hlen) : 0) + 1 + (plen >= 0 ? snprintf(NULL,0,"%d",plen) : 0) + 1;
            if (*rest == ' ') rest++;
            // need to split header and payload: header length is hlen
            if ((int)strlen(rest) < hlen) continue;
            strncpy(header, rest, hlen);
            header[hlen] = '\0';
            char *payload_start = rest + hlen;
            if ((int)strlen(payload_start) < plen) continue;
            strncpy(payload, payload_start, plen);
            payload[plen] = '\0';
            add_packet(id, hlen, plen, header, payload);
        } else if (strcmp(cmd, "DROP") == 0) {
            if (sscanf(line, "%*s %255s", id) == 1) drop_packet(id);
        } else if (strcmp(cmd, "HEADER") == 0) {
            if (sscanf(line, "%*s %255s", id) == 1) print_header(id);
        } else if (strcmp(cmd, "PAYLOAD") == 0) {
            if (sscanf(line, "%*s %255s", id) == 1) print_payload(id);
        }
    }
    return 0;
}