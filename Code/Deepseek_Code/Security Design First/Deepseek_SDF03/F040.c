// F040.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_LINE 4096
#define MAX_ID 64
#define MAX_HEADER 4096
#define MAX_PAYLOAD 4096

typedef struct Packet {
    char id[MAX_ID];
    size_t header_len;
    size_t payload_len;
    char header[MAX_HEADER];
    char payload[MAX_PAYLOAD];
    struct Packet *next;
} Packet;

Packet *head = NULL;

Packet *find_packet(const char *id) {
    Packet *p = head;
    while (p) {
        if (strcmp(p->id, id) == 0) return p;
        p = p->next;
    }
    return NULL;
}

void free_all() {
    Packet *p = head;
    while (p) {
        Packet *tmp = p;
        p = p->next;
        free(tmp);
    }
    head = NULL;
}

void handle_packet(char *line) {
    char id[MAX_ID];
    int hlen, plen;
    char hbuf[MAX_HEADER], pbuf[MAX_PAYLOAD];
    int n = sscanf(line, "PACKET %63s %d %d %4095s %4095s", id, &hlen, &plen, hbuf, pbuf);
    if (n < 5) return;
    if (hlen < 0 || plen < 0) return;
    if ((size_t)hlen != strlen(hbuf) || (size_t)plen != strlen(pbuf)) return;
    if (find_packet(id)) return;
    Packet *p = malloc(sizeof(Packet));
    if (!p) return;
    strcpy(p->id, id);
    p->header_len = hlen;
    p->payload_len = plen;
    strcpy(p->header, hbuf);
    strcpy(p->payload, pbuf);
    p->next = head;
    head = p;
}

void handle_drop(char *line) {
    char id[MAX_ID];
    if (sscanf(line, "DROP %63s", id) != 1) return;
    Packet *p = head, *prev = NULL;
    while (p) {
        if (strcmp(p->id, id) == 0) {
            if (prev) prev->next = p->next;
            else head = p->next;
            free(p);
            return;
        }
        prev = p;
        p = p->next;
    }
}

void handle_header(char *line) {
    char id[MAX_ID];
    if (sscanf(line, "HEADER %63s", id) != 1) return;
    Packet *p = find_packet(id);
    if (!p) return;
    printf("%s\n", p->header);
}

void handle_payload(char *line) {
    char id[MAX_ID];
    if (sscanf(line, "PAYLOAD %63s", id) != 1) return;
    Packet *p = find_packet(id);
    if (!p) return;
    printf("%s\n", p->payload);
}

void handle_stats() {
    size_t total = 0;
    Packet *p = head;
    while (p) {
        total += p->payload_len;
        p = p->next;
    }
    printf("%zu\n", total);
}

int main() {
    char line[MAX_LINE];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        if (strncmp(line, "PACKET ", 7) == 0) handle_packet(line);
        else if (strncmp(line, "DROP ", 5) == 0) handle_drop(line);
        else if (strncmp(line, "HEADER ", 7) == 0) handle_header(line);
        else if (strncmp(line, "PAYLOAD ", 8) == 0) handle_payload(line);
        else if (strcmp(line, "STATS") == 0) handle_stats();
    }
    free_all();
    return 0;
}