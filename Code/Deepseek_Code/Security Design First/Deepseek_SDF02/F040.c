/* F040.c */
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
} packet_t;

packet_t *packets = NULL;
packet_t *tail = NULL;

packet_t *find_packet(const char *id) {
    packet_t *p = packets;
    while (p) {
        if (strcmp(p->id, id) == 0) return p;
        p = p->next;
    }
    return NULL;
}

void packet_cmd(const char *id, int hlen, int plen, const char *header, const char *payload) {
    if (find_packet(id)) return;
    if (hlen < 0 || plen < 0) return;
    if (strlen(header) != (size_t)hlen || strlen(payload) != (size_t)plen) return;
    packet_t *p = malloc(sizeof(packet_t));
    if (!p) return;
    p->id = strdup(id);
    p->header_len = hlen;
    p->payload_len = plen;
    p->header = strdup(header);
    p->payload = strdup(payload);
    p->next = NULL;
    if (!p->id || !p->header || !p->payload) {
        free(p->id); free(p->header); free(p->payload); free(p);
        return;
    }
    if (tail) tail->next = p;
    else packets = p;
    tail = p;
}

void drop_packet(const char *id) {
    packet_t *prev = NULL, *p = packets;
    while (p) {
        if (strcmp(p->id, id) == 0) {
            if (prev) prev->next = p->next;
            else packets = p->next;
            if (p == tail) tail = prev;
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

void header_cmd(const char *id) {
    packet_t *p = find_packet(id);
    if (!p) {
        printf("NOT_FOUND\n");
        return;
    }
    printf("%s\n", p->header);
}

void payload_cmd(const char *id) {
    packet_t *p = find_packet(id);
    if (!p) {
        printf("NOT_FOUND\n");
        return;
    }
    printf("%s\n", p->payload);
}

void stats_cmd(void) {
    int total = 0;
    packet_t *p = packets;
    while (p) {
        total += p->payload_len;
        p = p->next;
    }
    printf("%d\n", total);
}

void free_all(void) {
    packet_t *p = packets;
    while (p) {
        packet_t *nxt = p->next;
        free(p->id);
        free(p->header);
        free(p->payload);
        free(p);
        p = nxt;
    }
}

int main(void) {
    char *line = NULL;
    size_t len = 0;
    while (getline(&line, &len, stdin) != -1) {
        line[strcspn(line, "\n")] = 0;
        if (strcmp(line, "END") == 0) break;
        char cmd[10], id[256];
        int hlen, plen;
        if (sscanf(line, "%9s %255s %d %d", cmd, id, &hlen, &plen) >= 2) {
            const char *rest = line + strlen(cmd) + 1 + strlen(id) + 1;
            if (strcmp(cmd, "PACKET") == 0) {
                if (sscanf(line, "%*s %255s %d %d", id, &hlen, &plen) == 3) {
                    const char *header = rest;
                    const char *payload = header + hlen + 1; // skip space
                    while (*payload == ' ') payload++;
                    packet_cmd(id, hlen, plen, header, payload);
                }
            } else if (strcmp(cmd, "DROP") == 0) {
                drop_packet(id);
            } else if (strcmp(cmd, "HEADER") == 0) {
                header_cmd(id);
            } else if (strcmp(cmd, "PAYLOAD") == 0) {
                payload_cmd(id);
            } else if (strcmp(cmd, "STATS") == 0) {
                stats_cmd();
            }
        }
    }
    free(line);
    free_all();
    return 0;
}