// F040.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *id;
    int header_len;
    int payload_len;
    char *header;
    char *payload;
} Packet;

Packet *packets = NULL;
int pc = 0;

int find_packet(const char *id) {
    for (int i = 0; i < pc; i++) {
        if (strcmp(packets[i].id, id) == 0) return i;
    }
    return -1;
}

int main(void) {
    char line[16384];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        char cmd[32];
        char a1[256];
        a1[0] = '\0';
        int n = sscanf(line, "%31s %255s", cmd, a1);
        if (n < 1) continue;

        if (strcmp(cmd, "PACKET") == 0) {
            char id[256];
            int hlen, plen;
            char header[4096];
            char payload[4096];
            header[0] = '\0';
            payload[0] = '\0';
            n = sscanf(line, "%31s %255s %d %d %4095s %4095[^\n]", cmd, id, &hlen, &plen, header, payload);
            if (n < 6) continue;
            if (hlen < 0 || plen < 0) continue;
            if ((int)strlen(header) != hlen) continue;
            if ((int)strlen(payload) != plen) continue;
            packets = realloc(packets, (pc + 1) * sizeof(Packet));
            if (!packets) return 1;
            packets[pc].id = strdup(id);
            packets[pc].header_len = hlen;
            packets[pc].payload_len = plen;
            packets[pc].header = strdup(header);
            packets[pc].payload = strdup(payload);
            pc++;
        } else if (strcmp(cmd, "DROP") == 0 && n >= 2) {
            int pi = find_packet(a1);
            if (pi >= 0) {
                free(packets[pi].id);
                free(packets[pi].header);
                free(packets[pi].payload);
                for (int k = pi; k < pc - 1; k++) {
                    packets[k] = packets[k + 1];
                }
                pc--;
            }
        } else if (strcmp(cmd, "HEADER") == 0 && n >= 2) {
            int pi = find_packet(a1);
            if (pi >= 0) {
                printf("%s\n", packets[pi].header);
            }
        } else if (strcmp(cmd, "PAYLOAD") == 0 && n >= 2) {
            int pi = find_packet(a1);
            if (pi >= 0) {
                printf("%s\n", packets[pi].payload);
            }
        } else if (strcmp(cmd, "STATS") == 0) {
            int total = 0;
            for (int i = 0; i < pc; i++) {
                total += packets[i].payload_len;
            }
            printf("%d\n", total);
        }
    }
    for (int i = 0; i < pc; i++) {
        free(packets[i].id);
        free(packets[i].header);
        free(packets[i].payload);
    }
    free(packets);
    return 0;
}