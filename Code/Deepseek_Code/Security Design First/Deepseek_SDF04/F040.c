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

Packet *head = NULL;

Packet *find_packet(const char *id) {
    Packet *p = head;
    while (p) {
        if (strcmp(p->id, id) == 0) return p;
        p = p->next;
    }
    return NULL;
}

void add_packet(const char *id, int hlen, int plen, const char *header, const char *payload) {
    if (find_packet(id)) return;
    if (hlen < 0 || plen < 0) return;
    // verify lengths match strings
    if ((int)strlen(header) != hlen || (int)strlen(payload) != plen) return;
    Packet *p = malloc(sizeof(Packet));
    if (!p) return;
    p->id = strdup(id);
    p->headerLen = hlen;
    p->payloadLen = plen;
    p->header = strdup(header);
    p->payload = strdup(payload);
    p->next = NULL;
    if (!p->id || !p->header || !p->payload) {
        free(p->id); free(p->header); free(p->payload); free(p);
        return;
    }
    if (!head) head = p;
    else {
        Packet *last = head;
        while (last->next) last = last->next;
        last->next = p;
    }
}

void drop_packet(const char *id) {
    Packet *p = head, *prev = NULL;
    while (p) {
        if (strcmp(p->id, id) == 0) {
            if (prev) prev->next = p->next;
            else head = p->next;
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
    Packet *p = head;
    while (p) {
        total += p->payloadLen;
        p = p->next;
    }
    printf("%d\n", total);
}

void free_all(void) {
    Packet *p = head;
    while (p) {
        Packet *next = p->next;
        free(p->id);
        free(p->header);
        free(p->payload);
        free(p);
        p = next;
    }
}

int main(void) {
    char *line = NULL;
    size_t len = 0;
    while (getline(&line, &len, stdin) != -1) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        char cmd[64], id[256], header[4096], payload[4096];
        int hlen, plen;
        if (sscanf(line, "%63s", cmd) != 1) continue;
        if (strcmp(cmd, "PACKET") == 0) {
            if (sscanf(line, "%*s %255s %d %d %s %s", id, &hlen, &plen, header, payload) == 5) {
                // but header/payload may contain spaces? spec says header and payload are strings, but no spaces mentioned? They are probably strings but may contain spaces? The prompt doesn't say they can contain spaces, but to be safe we use %s which stops at spaces. However the command says "header payload" at end, likely they are strings without spaces. We'll use %s.
                add_packet(id, hlen, plen, header, payload);
            }
        } else if (strcmp(cmd, "DROP") == 0) {
            if (sscanf(line, "%*s %255s", id) == 1)
                drop_packet(id);
        } else if (strcmp(cmd, "HEADER") == 0) {
            if (sscanf(line, "%*s %255s", id) == 1)
                header_packet(id);
        } else if (strcmp(cmd, "PAYLOAD") == 0) {
            if (sscanf(line, "%*s %255s", id) == 1)
                payload_packet(id);
        } else if (strcmp(cmd, "STATS") == 0) {
            stats_packets();
        }
    }
    free(line);
    free_all();
    return 0;
}