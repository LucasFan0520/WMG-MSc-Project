// F040.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Packet {
    char *id;
    char *header;
    char *payload;
    int header_len;
    int payload_len;
    struct Packet *next;
} Packet;

Packet *head = NULL;
long long total_payload_bytes = 0;

Packet *find_packet(char *id) {
    for (Packet *p = head; p; p = p->next) {
        if (strcmp(p->id, id) == 0) return p;
    }
    return NULL;
}

void add_packet(char *id, int hlen, int plen, char *header, char *payload) {
    if (find_packet(id)) return;
    if (hlen != (int)strlen(header) || plen != (int)strlen(payload)) return;
    Packet *p = malloc(sizeof(Packet));
    p->id = strdup(id);
    p->header = strdup(header);
    p->payload = strdup(payload);
    p->header_len = hlen;
    p->payload_len = plen;
    p->next = head;
    head = p;
    total_payload_bytes += plen;
}

void drop_packet(char *id) {
    Packet *prev = NULL;
    Packet *curr = head;
    while (curr) {
        if (strcmp(curr->id, id) == 0) {
            if (prev) prev->next = curr->next;
            else head = curr->next;
            total_payload_bytes -= curr->payload_len;
            free(curr->id);
            free(curr->header);
            free(curr->payload);
            free(curr);
            return;
        }
        prev = curr;
        curr = curr->next;
    }
}

void print_header(char *id) {
    Packet *p = find_packet(id);
    if (p) printf("%s\n", p->header);
}

void print_payload(char *id) {
    Packet *p = find_packet(id);
    if (p) printf("%s\n", p->payload);
}

void print_stats() {
    printf("%lld\n", total_payload_bytes);
}

int main() {
    char cmd[20], a[100], header[1000], payload[2000];
    int hlen, plen;
    while (scanf("%19s", cmd) == 1 && strcmp(cmd, "END") != 0) {
        if (strcmp(cmd, "PACKET") == 0) {
            scanf("%99s%d%d", a, &hlen, &plen);
            scanf("%999s", header);
            scanf("%1999s", payload);
            add_packet(a, hlen, plen, header, payload);
        } else if (strcmp(cmd, "DROP") == 0) {
            scanf("%99s", a);
            drop_packet(a);
        } else if (strcmp(cmd, "HEADER") == 0) {
            scanf("%99s", a);
            print_header(a);
        } else if (strcmp(cmd, "PAYLOAD") == 0) {
            scanf("%99s", a);
            print_payload(a);
        } else if (strcmp(cmd, "STATS") == 0) {
            print_stats();
        }
    }
    return 0;
}