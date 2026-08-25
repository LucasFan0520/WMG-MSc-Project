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

Packet *p_head = NULL;

Packet *find_packet(const char *id) {
    Packet *p = p_head;
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
    if (!p) return;
    p->id = strdup(id);
    p->headerLen = hlen;
    p->payloadLen = plen;
    p->header = strdup(header);
    p->payload = strdup(payload);
    p->next = p_head;
    p_head = p;
}

void drop_packet(const char *id) {
    Packet *prev = NULL;
    Packet *p = p_head;
    while (p) {
        if (strcmp(p->id, id) == 0) {
            if (prev) prev->next = p->next;
            else p_head = p->next;
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
    if (!p) return;
    printf("%s\n", p->header);
}

void print_payload(const char *id) {
    Packet *p = find_packet(id);
    if (!p) return;
    printf("%s\n", p->payload);
}

void stats(void) {
    int total = 0;
    Packet *p = p_head;
    while (p) {
        total += p->payloadLen;
        p = p->next;
    }
    printf("%d\n", total);
}

void free_all(void) {
    while (p_head) {
        Packet *next = p_head->next;
        free(p_head->id);
        free(p_head->header);
        free(p_head->payload);
        free(p_head);
        p_head = next;
    }
}

int main(void) {
    char *line = NULL;
    size_t len = 0;
    ssize_t nread;

    while ((nread = getline(&line, &len, stdin)) != -1) {
        if (line[nread-1] == '\n') line[nread-1] = '\0';
        char cmd[32];
        if (sscanf(line, "%31s", cmd) != 1) continue;
        if (strcmp(cmd, "END") == 0) break;

        if (strcmp(cmd, "PACKET") == 0) {
            char id[256], header[1024], payload[1024];
            int hlen, plen;
            if (sscanf(line, "%*s %255s %d %d %1023[^ ] %1023[^\n]", id, &hlen, &plen, header, payload) == 5) {
                add_packet(id, hlen, plen, header, payload);
            }
        } else if (strcmp(cmd, "DROP") == 0) {
            char id[256];
            if (sscanf(line, "%*s %255s", id) == 1) {
                drop_packet(id);
            }
        } else if (strcmp(cmd, "HEADER") == 0) {
            char id[256];
            if (sscanf(line, "%*s %255s", id) == 1) {
                print_header(id);
            }
        } else if (strcmp(cmd, "PAYLOAD") == 0) {
            char id[256];
            if (sscanf(line, "%*s %255s", id) == 1) {
                print_payload(id);
            }
        } else if (strcmp(cmd, "STATS") == 0) {
            stats();
        }
    }

    free(line);
    free_all();
    return 0;
}