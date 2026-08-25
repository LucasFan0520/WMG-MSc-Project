// F040.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct packet {
    char *id;
    int header_len;
    int payload_len;
    char *header;
    char *payload;
    struct packet *next;
};

struct packet *packets = NULL;

struct packet *find_packet(char *id) {
    struct packet *p = packets;
    while (p) {
        if (strcmp(p->id, id) == 0) return p;
        p = p->next;
    }
    return NULL;
}

void add_packet(char *id, int hlen, int plen, char *header, char *payload) {
    if (find_packet(id)) return;
    struct packet *p = malloc(sizeof(struct packet));
    p->id = malloc(strlen(id) + 1);
    strcpy(p->id, id);
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

void drop_packet(char *id) {
    struct packet *p = packets, *prev = NULL;
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

void print_header(char *id) {
    struct packet *p = find_packet(id);
    if (p) printf("%s\n", p->header);
}

void print_payload(char *id) {
    struct packet *p = find_packet(id);
    if (p) printf("%s\n", p->payload);
}

void print_stats() {
    int total = 0;
    struct packet *p = packets;
    while (p) {
        total += p->payload_len;
        p = p->next;
    }
    printf("%d\n", total);
}

int main() {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        if (strcmp(line, "END") == 0) break;
        char cmd[32];
        char id[256];
        int hlen, plen;
        char header[4096], payload[4096];
        int n = sscanf(line, "%31s %255s %d %d %4095s %4095s", cmd, id, &hlen, &plen, header, payload);
        if (strcmp(cmd, "PACKET") == 0 && n >= 6) {
            add_packet(id, hlen, plen, header, payload);
        } else if (strcmp(cmd, "DROP") == 0 && n >= 2) {
            drop_packet(id);
        } else if (strcmp(cmd, "HEADER") == 0 && n >= 2) {
            print_header(id);
        } else if (strcmp(cmd, "PAYLOAD") == 0 && n >= 2) {
            print_payload(id);
        } else if (strcmp(cmd, "STATS") == 0) {
            print_stats();
        }
    }
    return 0;
}