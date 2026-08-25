/* F040.c */
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

void add_packet(const char *id, int header_len, int payload_len, const char *header, const char *payload) {
    if (find_packet(id)) return;
    if (header_len < 0 || payload_len < 0) return;
    if ((int)strlen(header) != header_len || (int)strlen(payload) != payload_len) return;
    Packet *p = malloc(sizeof(Packet));
    p->id = strdup(id);
    p->header_len = header_len;
    p->payload_len = payload_len;
    p->header = strdup(header);
    p->payload = strdup(payload);
    p->next = packets;
    packets = p;
}

void drop_packet(const char *id) {
    Packet *p = packets, *prev = NULL;
    while (p && strcmp(p->id, id) != 0) {
        prev = p;
        p = p->next;
    }
    if (!p) return;
    if (prev) prev->next = p->next;
    else packets = p->next;
    free(p->id);
    free(p->header);
    free(p->payload);
    free(p);
}

void header_packet(const char *id) {
    Packet *p = find_packet(id);
    if (!p) { printf("NOT_FOUND\n"); return; }
    printf("%s\n", p->header);
}

void payload_packet(const char *id) {
    Packet *p = find_packet(id);
    if (!p) { printf("NOT_FOUND\n"); return; }
    printf("%s\n", p->payload);
}

void stats(void) {
    int total = 0;
    Packet *p = packets;
    while (p) {
        total += p->payload_len;
        p = p->next;
    }
    printf("%d\n", total);
}

void free_all(void) {
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

int main(void) {
    char *line = NULL;
    size_t len = 0;
    while (getline(&line, &len, stdin) != -1) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        char cmd[16];
        if (sscanf(line, "%15s", cmd) < 1) continue;
        if (strcmp(cmd, "PACKET") == 0) {
            char id[256];
            int hlen, plen;
            char *p = line + 7;
            if (sscanf(p, "%255s %d %d", id, &hlen, &plen) != 3) continue;
            while (*p && *p != ' ') p++;
            while (*p == ' ') p++;
            while (*p && *p != ' ') p++;
            while (*p == ' ') p++;
            while (*p && *p != ' ') p++;
            while (*p == ' ') p++;
            char *header_start = p;
            for (int i = 0; i < hlen; i++) {
                if (!*p) { p = NULL; break; }
                p++;
            }
            if (!p) continue;
            char *payload_start = p;
            for (int i = 0; i < plen; i++) {
                if (!*p) { p = NULL; break; }
                p++;
            }
            if (!p) continue;
            char header[hlen + 1];
            char payload[plen + 1];
            strncpy(header, header_start, hlen);
            header[hlen] = '\0';
            strncpy(payload, payload_start, plen);
            payload[plen] = '\0';
            add_packet(id, hlen, plen, header, payload);
        } else if (strcmp(cmd, "DROP") == 0) {
            char *p = line + 5;
            while (*p == ' ') p++;
            drop_packet(p);
        } else if (strcmp(cmd, "HEADER") == 0) {
            char *p = line + 7;
            while (*p == ' ') p++;
            header_packet(p);
        } else if (strcmp(cmd, "PAYLOAD") == 0) {
            char *p = line + 8;
            while (*p == ' ') p++;
            payload_packet(p);
        } else if (strcmp(cmd, "STATS") == 0) {
            stats();
        }
    }
    free_all();
    free(line);
    return 0;
}