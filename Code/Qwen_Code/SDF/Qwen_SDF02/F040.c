// F040.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_LINE 4096

typedef struct Packet {
    char *id;
    char *header;
    char *payload;
    int header_len;
    int payload_len;
    struct Packet *next;
} Packet;

Packet *packets = NULL;

Packet* find_packet(const char *id) {
    for (Packet *p = packets; p; p = p->next)
        if (strcmp(p->id, id) == 0) return p;
    return NULL;
}

int main() {
    char line[MAX_LINE];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        if (strcmp(line, "END") == 0) break;
        
        char cmd[32];
        if (sscanf(line, "%31s", cmd) != 1) continue;
        char *rest = line + strlen(cmd);
        while (*rest == ' ') rest++;
        
        if (strcmp(cmd, "PACKET") == 0) {
            char id[256];
            int h_len, p_len;
            int n = 0;
            if (sscanf(rest, "%255s %d %d%n", id, &h_len, &p_len, &n) != 3) continue;
            char *data = rest + n;
            while (*data == ' ') data++;
            if ((int)strlen(data) < h_len + p_len) continue;
            char *header = malloc(h_len + 1);
            char *payload = malloc(p_len + 1);
            if (!header || !payload) { free(header); free(payload); continue; }
            memcpy(header, data, h_len);
            header[h_len] = '\0';
            memcpy(payload, data + h_len, p_len);
            payload[p_len] = '\0';
            Packet *p = malloc(sizeof(Packet));
            if (!p) { free(header); free(payload); continue; }
            p->id = strdup(id);
            if (!p->id) { free(header); free(payload); free(p); continue; }
            p->header = header;
            p->payload = payload;
            p->header_len = h_len;
            p->payload_len = p_len;
            p->next = packets;
            packets = p;
        } else if (strcmp(cmd, "DROP") == 0) {
            char id[256];
            if (sscanf(rest, "%255s", id) != 1) continue;
            Packet *prev = NULL;
            for (Packet *p = packets; p; prev = p, p = p->next) {
                if (strcmp(p->id, id) == 0) {
                    if (prev) prev->next = p->next;
                    else packets = p->next;
                    free(p->id); free(p->header); free(p->payload); free(p);
                    break;
                }
            }
        } else if (strcmp(cmd, "HEADER") == 0) {
            char id[256];
            if (sscanf(rest, "%255s", id) != 1) continue;
            Packet *p = find_packet(id);
            if (p) printf("%s\n", p->header);
        } else if (strcmp(cmd, "PAYLOAD") == 0) {
            char id[256];
            if (sscanf(rest, "%255s", id) != 1) continue;
            Packet *p = find_packet(id);
            if (p) printf("%s\n", p->payload);
        } else if (strcmp(cmd, "STATS") == 0) {
            long long total = 0;
            for (Packet *p = packets; p; p = p->next) {
                total += p->payload_len;
            }
            printf("%lld\n", total);
        }
    }
    while (packets) {
        Packet *p = packets;
        packets = packets->next;
        free(p->id); free(p->header); free(p->payload); free(p);
    }
    return 0;
}