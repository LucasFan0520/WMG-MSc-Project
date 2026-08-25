// F040.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_PKT 10000
#define MAX_LEN 10000

typedef struct {
    char id[100];
    int hlen;
    int plen;
    char header[MAX_LEN];
    char payload[MAX_LEN];
} Pkt;

Pkt pkts[MAX_PKT];
int pcount = 0;

int find_pkt(const char *id) {
    for (int i = 0; i < pcount; i++) if (strcmp(pkts[i].id, id) == 0) return i;
    return -1;
}

int main() {
    char cmd[20], id[100], header[MAX_LEN], payload[MAX_LEN];
    int hlen, plen;
    while (scanf("%s", cmd) == 1 && strcmp(cmd, "END") != 0) {
        if (strcmp(cmd, "PACKET") == 0) {
            scanf("%s %d %d", id, &hlen, &plen);
            scanf("%s", header);
            scanf("%s", payload);
            if (hlen == (int)strlen(header) && plen == (int)strlen(payload)) {
                if (find_pkt(id) == -1 && pcount < MAX_PKT) {
                    strcpy(pkts[pcount].id, id);
                    pkts[pcount].hlen = hlen;
                    pkts[pcount].plen = plen;
                    strcpy(pkts[pcount].header, header);
                    strcpy(pkts[pcount].payload, payload);
                    pcount++;
                }
            }
        } else if (strcmp(cmd, "DROP") == 0) {
            scanf("%s", id);
            int idx = find_pkt(id);
            if (idx != -1) {
                for (int i = idx; i < pcount - 1; i++) pkts[i] = pkts[i + 1];
                pcount--;
            }
        } else if (strcmp(cmd, "HEADER") == 0) {
            scanf("%s", id);
            int idx = find_pkt(id);
            if (idx != -1) printf("%s\n", pkts[idx].header);
        } else if (strcmp(cmd, "PAYLOAD") == 0) {
            scanf("%s", id);
            int idx = find_pkt(id);
            if (idx != -1) printf("%s\n", pkts[idx].payload);
        } else if (strcmp(cmd, "STATS") == 0) {
            long long total = 0;
            for (int i = 0; i < pcount; i++) {
                total += pkts[i].plen;
            }
            printf("%lld\n", total);
        }
    }
    return 0;
}