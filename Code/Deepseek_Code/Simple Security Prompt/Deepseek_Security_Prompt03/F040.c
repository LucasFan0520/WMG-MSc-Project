/*
 * F040.c
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct packet {
    char *id;
    int header_len;
    int payload_len;
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

void packet_store(const char *id, int hlen, int plen, const char *header, const char *payload) {
    if (find_packet(id)) return;
    if (hlen < 0 || plen < 0) return;
    if ((int)strlen(header) < hlen || (int)strlen(payload) < plen) return;
    Packet *p = malloc(sizeof(Packet));
    p->id = strdup(id);
    p->header_len = hlen;
    p->payload_len = plen;
    p->header = malloc(hlen + 1);
    strncpy(p->header, header, hlen);
    p->header[hlen] = 0;
    p->payload = malloc(plen + 1);
    strncpy(p->payload, payload, plen);
    p->payload[plen] = 0;
    p->next = packets;
    packets = p;
}

void packet_drop(const char *id) {
    Packet *prev = NULL, *p = packets;
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

void packet_header(const char *id) {
    Packet *p = find_packet(id);
    if (!p) {
        printf("NOT_FOUND\n");
        return;
    }
    printf("%s\n", p->header);
}

void packet_payload(const char *id) {
    Packet *p = find_packet(id);
    if (!p) {
        printf("NOT_FOUND\n");
        return;
    }
    printf("%s\n", p->payload);
}

void packet_stats(void) {
    int total = 0;
    Packet *p = packets;
    while (p) {
        total += p->payload_len;
        p = p->next;
    }
    printf("%d\n", total);
}

int main(void) {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        if (strcmp(line, "END") == 0) break;
        char cmd[32], id[256];
        if (sscanf(line, "%31s %255s", cmd, id) < 1) continue;
        char *rest = line + strlen(cmd) + strlen(id) + 1;
        while (*rest == ' ') rest++;
        if (strcmp(cmd, "PACKET") == 0) {
            int hlen, plen;
            if (sscanf(rest, "%d %d", &hlen, &plen) == 2) {
                char *hstart = rest;
                while (*hstart == ' ') hstart++;
                while (*hstart >= '0' && *hstart <= '9') hstart++;
                while (*hstart == ' ') hstart++;
                while (*hstart >= '0' && *hstart <= '9') hstart++;
                while (*hstart == ' ') hstart++;
                char *pstart = hstart + hlen;
                while (*pstart == ' ') pstart++;
                char header[4096], payload[4096];
                strncpy(header, hstart, hlen);
                header[hlen] = 0;
                strncpy(payload, pstart, plen);
                payload[plen] = 0;
                packet_store(id, hlen, plen, header, payload);
            }
        } else if (strcmp(cmd, "DROP") == 0) {
            packet_drop(id);
        } else if (strcmp(cmd, "HEADER") == 0) {
            packet_header(id);
        } else if (strcmp(cmd, "PAYLOAD") == 0) {
            packet_payload(id);
        } else if (strcmp(cmd, "STATS") == 0) {
            packet_stats();
        }
    }
    return 0;
}