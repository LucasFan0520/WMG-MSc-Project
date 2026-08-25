// F040.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Packet {
    char *id;
    int header_len;
    int payload_len;
    char *header;
    char *payload;
    struct Packet *next;
} Packet;

Packet *packets = NULL;

void add_packet(const char *id, int hlen, int plen, const char *header, const char *payload) {
    if (strlen(header) != (size_t)hlen || strlen(payload) != (size_t)plen) return;
    Packet *p = malloc(sizeof(Packet));
    p->id = strdup(id);
    p->header_len = hlen;
    p->payload_len = plen;
    p->header = strdup(header);
    p->payload = strdup(payload);
    p->next = packets;
    packets = p;
}

void drop_packet(const char *id) {
    Packet **pp = &packets;
    while (*pp) {
        if (strcmp((*pp)->id, id) == 0) {
            Packet *tmp = *pp;
            *pp = tmp->next;
            free(tmp->id);
            free(tmp->header);
            free(tmp->payload);
            free(tmp);
            return;
        }
        pp = &(*pp)->next;
    }
}

void print_header(const char *id) {
    for (Packet *p = packets; p; p = p->next) {
        if (strcmp(p->id, id) == 0) {
            printf("%s\n", p->header);
            return;
        }
    }
}

void print_payload(const char *id) {
    for (Packet *p = packets; p; p = p->next) {
        if (strcmp(p->id, id) == 0) {
            printf("%s\n", p->payload);
            return;
        }
    }
}

void print_stats() {
    long long total = 0;
    for (Packet *p = packets; p; p = p->next) {
        total += p->payload_len;
    }
    printf("%lld\n", total);
}

void cleanup() {
    while (packets) {
        Packet *tmp = packets->next;
        free(packets->id);
        free(packets->header);
        free(packets->payload);
        free(packets);
        packets = tmp;
    }
}

int main() {
    char line[8192];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        if (strcmp(line, "END") == 0) break;
        char cmd[32], a1[256], a2[256], a3[256];
        int hlen, plen;
        if (sscanf(line, "%31s", cmd) != 1) continue;
        if (strcmp(cmd, "PACKET") == 0) {
            if (sscanf(line, "%*s %255s %d %d %255s", a1, &hlen, &plen, a2) == 4) {
                char *t = strchr(line, ' ');
                if (t) { t = strchr(t + 1, ' '); if (t) { t = strchr(t + 1, ' '); if (t) { t = strchr(t + 1, ' '); if (t) { t = strchr(t + 1, ' '); if (t) add_packet(a1, hlen, plen, a2, t + 1); } } } }
            }
        } else if (strcmp(cmd, "DROP") == 0) {
            if (sscanf(line, "%*s %255s", a1) == 1) drop_packet(a1);
        } else if (strcmp(cmd, "HEADER") == 0) {
            if (sscanf(line, "%*s %255s", a1) == 1) print_header(a1);
        } else if (strcmp(cmd, "PAYLOAD") == 0) {
            if (sscanf(line, "%*s %255s", a1) == 1) print_payload(a1);
        } else if (strcmp(cmd, "STATS") == 0) {
            print_stats();
        }
    }
    cleanup();
    return 0;
}