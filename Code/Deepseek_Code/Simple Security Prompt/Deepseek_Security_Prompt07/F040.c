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
    p->header_len = hlen;
    p->payload_len = plen;
    p->header = strdup(header);
    p->payload = strdup(payload);
    p->next = packets;
    packets = p;
}

void drop_packet(const char *id) {
    Packet *prev = NULL;
    Packet *p = packets;
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

void print_header(const char *id) {
    Packet *p = find_packet(id);
    if (!p) {
        printf("NOT_FOUND\n");
        return;
    }
    for (char *s = p->header; *s; s++) putchar(*s == ' ' ? '_' : *s);
    putchar('\n');
}

void print_payload(const char *id) {
    Packet *p = find_packet(id);
    if (!p) {
        printf("NOT_FOUND\n");
        return;
    }
    for (char *s = p->payload; *s; s++) putchar(*s == ' ' ? '_' : *s);
    putchar('\n');
}

void print_stats() {
    Packet *p = packets;
    int total = 0;
    while (p) {
        total += p->payload_len;
        p = p->next;
    }
    printf("%d\n", total);
}

void free_all() {
    Packet *p = packets;
    while (p) {
        Packet *next = p->next;
        free(p->id);
        free(p->header);
        free(p->payload);
        free(p);
        p = next;
    }
}

int main() {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        char cmd[32];
        if (sscanf(line, "%31s", cmd) != 1) continue;
        char rest[4096];
        char *p = line + strlen(cmd);
        while (*p == ' ') p++;
        strcpy(rest, p);
        if (strcmp(cmd, "PACKET") == 0) {
            char id[256];
            int hlen, plen;
            char header[4096], payload[4096];
            if (sscanf(rest, "%255s %d %d %[^ ] %[^\n]", id, &hlen, &plen, header, payload) == 5) {
                if ((int)strlen(header) == hlen && (int)strlen(payload) == plen) {
                    add_packet(id, hlen, plen, header, payload);
                }
            }
        } else if (strcmp(cmd, "DROP") == 0) {
            drop_packet(rest);
        } else if (strcmp(cmd, "HEADER") == 0) {
            print_header(rest);
        } else if (strcmp(cmd, "PAYLOAD") == 0) {
            print_payload(rest);
        } else if (strcmp(cmd, "STATS") == 0) {
            print_stats();
        }
    }
    free_all();
    return 0;
}