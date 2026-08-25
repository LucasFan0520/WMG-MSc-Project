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
int p_count = 0;

int find_id(const char *id) {
    for (int i = 0; i < p_count; i++) {
        if (strcmp(packets[i].id, id) == 0) {
            return i;
        }
    }
    return -1;
}

int main() {
    char cmd[32];
    char id[1024];
    int hlen;
    int plen;
    char header[102400];
    char payload[102400];

    while (scanf("%s", cmd) == 1 && strcmp(cmd, "END") != 0) {
        if (strcmp(cmd, "PACKET") == 0) {
            scanf("%s %d %d", id, &hlen, &plen);
            if (hlen < 0 || plen < 0) {
                continue;
            }
            scanf("%s", header);
            scanf("%s", payload);
            if ((int)strlen(header) != hlen || (int)strlen(payload) != plen) {
                continue;
            }
            int idx = find_id(id);
            if (idx != -1) {
                free(packets[idx].header);
                free(packets[idx].payload);
                packets[idx].header_len = hlen;
                packets[idx].payload_len = plen;
                packets[idx].header = strdup(header);
                packets[idx].payload = strdup(payload);
            } else {
                packets = realloc(packets, sizeof(Packet) * (p_count + 1));
                packets[p_count].id = strdup(id);
                packets[p_count].header_len = hlen;
                packets[p_count].payload_len = plen;
                packets[p_count].header = strdup(header);
                packets[p_count].payload = strdup(payload);
                p_count++;
            }
        } else if (strcmp(cmd, "DROP") == 0) {
            scanf("%s", id);
            int idx = find_id(id);
            if (idx != -1) {
                free(packets[idx].id);
                free(packets[idx].header);
                free(packets[idx].payload);
                for (int i = idx; i < p_count - 1; i++) {
                    packets[i] = packets[i + 1];
                }
                p_count--;
            }
        } else if (strcmp(cmd, "HEADER") == 0) {
            scanf("%s", id);
            int idx = find_id(id);
            if (idx != -1) {
                printf("%s\n", packets[idx].header);
            }
        } else if (strcmp(cmd, "PAYLOAD") == 0) {
            scanf("%s", id);
            int idx = find_id(id);
            if (idx != -1) {
                printf("%s\n", packets[idx].payload);
            }
        } else if (strcmp(cmd, "STATS") == 0) {
            long long total = 0;
            for (int i = 0; i < p_count; i++) {
                total += packets[i].payload_len;
            }
            printf("%lld\n", total);
        }
    }

    for (int i = 0; i < p_count; i++) {
        free(packets[i].id);
        free(packets[i].header);
        free(packets[i].payload);
    }
    free(packets);

    return 0;
}