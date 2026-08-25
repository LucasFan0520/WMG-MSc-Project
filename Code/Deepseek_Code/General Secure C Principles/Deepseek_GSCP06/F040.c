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

void add_packet(const char *id, int hlen, int plen, const char *header, const char *payload) {
    if (find_packet(id)) return;
    Packet *p = malloc(sizeof(Packet));
    p->id = strdup(id);
    p->headerLen = hlen;
    p->payloadLen = plen;
    p->header = strdup(header);
    p->payload = strdup(payload);
    p->next = packets;
    packets = p;
}

void drop_packet(const char *id) {
    Packet *cur = packets, *prev = NULL;
    while (cur) {
        if (strcmp(cur->id, id) == 0) {
            if (prev) prev->next = cur->next;
            else packets = cur->next;
            free(cur->id);
            free(cur->header);
            free(cur->payload);
            free(cur);
            return;
        }
        prev = cur;
        cur = cur->next;
    }
}

void print_header(const char *id) {
    Packet *p = find_packet(id);
    if (p) printf("%s\n", p->header);
}

void print_payload(const char *id) {
    Packet *p = find_packet(id);
    if (p) printf("%s\n", p->payload);
}

void stats(void) {
    int total = 0;
    Packet *p = packets;
    while (p) {
        total += p->payloadLen;
        p = p->next;
    }
    printf("%d\n", total);
}

void cleanup_packets(void) {
    Packet *p = packets;
    while (p) {
        Packet *tmp = p;
        p = p->next;
        free(tmp->id);
        free(tmp->header);
        free(tmp->payload);
        free(tmp);
    }
}

int main(void) {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        if (strcmp(line, "END") == 0) break;
        char cmd[32], id[256], rest[4096];
        int n = sscanf(line, "%31s %255s %4095s", cmd, id, rest);
        if (n < 2) continue;
        if (strcmp(cmd, "PACKET") == 0 && n >= 3) {
            int hlen, plen;
            char header[2048], payload[2048];
            if (sscanf(rest, "%d %d %s %s", &hlen, &plen, header, payload) == 4) {
                // note: header/payload may contain spaces? But spec says "header payload" are strings, but no spaces? They might contain spaces? The prompt says "length fields are decimal and should match the following strings." It doesn't say they contain spaces; likely no spaces in header/payload. We'll assume they are contiguous strings.
                add_packet(id, hlen, plen, header, payload);
            }
        } else if (strcmp(cmd, "DROP") == 0) {
            drop_packet(id);
        } else if (strcmp(cmd, "HEADER") == 0) {
            print_header(id);
        } else if (strcmp(cmd, "PAYLOAD") == 0) {
            print_payload(id);
        } else if (strcmp(cmd, "STATS") == 0) {
            stats();
        }
    }
    cleanup_packets();
    return 0;
}