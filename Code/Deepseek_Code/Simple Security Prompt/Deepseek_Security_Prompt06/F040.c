// F040.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Packet {
    char *id;
    int headerLen;
    int payloadLen;
    char *header;
    char *payload;
    struct Packet *next;
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

void add_packet(const char *id, int hlen, int plen, const char *hdr, const char *pld) {
    if (find_packet(id)) return;
    if (hlen < 0 || plen < 0) return;
    if ((int)strlen(hdr) != hlen || (int)strlen(pld) != plen) return;
    Packet *p = malloc(sizeof(Packet));
    p->id = malloc(strlen(id) + 1);
    strcpy(p->id, id);
    p->headerLen = hlen;
    p->payloadLen = plen;
    p->header = malloc(hlen + 1);
    strcpy(p->header, hdr);
    p->payload = malloc(plen + 1);
    strcpy(p->payload, pld);
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
        total += p->payloadLen;
        p = p->next;
    }
    printf("%d\n", total);
}

void free_all(void) {
    Packet *p = packets;
    while (p) {
        Packet *tmp = p->next;
        free(p->id);
        free(p->header);
        free(p->payload);
        free(p);
        p = tmp;
    }
}

int main() {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        char cmd[32];
        char id[256], hdr[4096], pld[4096];
        int hlen, plen;
        if (sscanf(line, "%31s", cmd) < 1) continue;
        const char *rest = line + strlen(cmd) + 1;
        while (*rest == ' ') rest++;
        if (strcmp(cmd, "PACKET") == 0) {
            if (sscanf(rest, "%255s %d %d", id, &hlen, &plen) == 3) {
                const char *hstart = rest;
                while (*hstart && *hstart != ' ') hstart++;
                while (*hstart == ' ') hstart++;
                while (*hstart && *hstart != ' ') hstart++;
                while (*hstart == ' ') hstart++;
                while (*hstart && *hstart != ' ') hstart++;
                while (*hstart == ' ') hstart++;
                // now hstart points to header string
                const char *pend = hstart;
                while (*pend && *pend != ' ') pend++;
                size_t hlen_actual = pend - hstart;
                if ((int)hlen_actual != hlen) { /* ignore */ }
                char header[4096];
                strncpy(header, hstart, hlen);
                header[hlen] = '\0';
                const char *pstart = pend;
                while (*pstart == ' ') pstart++;
                if ((int)strlen(pstart) != plen) { /* ignore */ }
                add_packet(id, hlen, plen, header, pstart);
            }
        } else if (strcmp(cmd, "DROP") == 0) {
            if (sscanf(rest, "%255s", id) == 1) drop_packet(id);
        } else if (strcmp(cmd, "HEADER") == 0) {
            if (sscanf(rest, "%255s", id) == 1) header_packet(id);
        } else if (strcmp(cmd, "PAYLOAD") == 0) {
            if (sscanf(rest, "%255s", id) == 1) payload_packet(id);
        } else if (strcmp(cmd, "STATS") == 0) {
            stats_packets();
        }
    }
    free_all();
    return 0;
}