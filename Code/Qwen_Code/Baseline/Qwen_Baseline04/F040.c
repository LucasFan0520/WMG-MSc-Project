// F040.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *id;
    int hlen;
    int plen;
    char *header;
    char *payload;
} Packet;

Packet *pkts = NULL;
int pcount = 0;

int find_pkt(const char *id) {
    for (int i = 0; i < pcount; i++) {
        if (strcmp(pkts[i].id, id) == 0) return i;
    }
    return -1;
}

int main() {
    char cmd[20];
    while (scanf("%s", cmd) == 1) {
        if (strcmp(cmd, "END") == 0) break;
        if (strcmp(cmd, "PACKET") == 0) {
            char id[100];
            int hlen, plen;
            char header[1000], payload[1000];
            scanf("%s %d %d", id, &hlen, &plen);
            scanf("%s %s", header, payload);
            if ((int)strlen(header) == hlen && (int)strlen(payload) == plen) {
                if (find_pkt(id) == -1) {
                    pkts = realloc(pkts, sizeof(Packet) * (pcount + 1));
                    pkts[pcount].id = strdup(id);
                    pkts[pcount].hlen = hlen;
                    pkts[pcount].plen = plen;
                    pkts[pcount].header = strdup(header);
                    pkts[pcount].payload = strdup(payload);
                    pcount++;
                }
            }
        } else if (strcmp(cmd, "DROP") == 0) {
            char id[100];
            scanf("%s", id);
            int idx = find_pkt(id);
            if (idx != -1) {
                free(pkts[idx].id);
                free(pkts[idx].header);
                free(pkts[idx].payload);
                for (int i = idx; i < pcount - 1; i++) pkts[i] = pkts[i+1];
                pcount--;
            }
        } else if (strcmp(cmd, "HEADER") == 0) {
            char id[100];
            scanf("%s", id);
            int idx = find_pkt(id);
            if (idx != -1) {
                printf("%s\n", pkts[idx].header);
            }
        } else if (strcmp(cmd, "PAYLOAD") == 0) {
            char id[100];
            scanf("%s", id);
            int idx = find_pkt(id);
            if (idx != -1) {
                printf("%s\n", pkts[idx].payload);
            }
        } else if (strcmp(cmd, "STATS") == 0) {
            int total = 0;
            for (int i = 0; i < pcount; i++) {
                total += pkts[i].plen;
            }
            printf("%d\n", total);
        }
    }
    for (int i = 0; i < pcount; i++) {
        free(pkts[i].id);
        free(pkts[i].header);
        free(pkts[i].payload);
    }
    free(pkts);
    return 0;
}