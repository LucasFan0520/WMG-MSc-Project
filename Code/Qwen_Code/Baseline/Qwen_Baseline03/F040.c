// F040.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char id[100];
    int hlen;
    int plen;
    char header[1000];
    char payload[1000];
} Pkt;

Pkt pkts[1000];
int pn = 0;

int findPkt(char *id) {
    for (int i = 0; i < pn; i++) {
        if (!strcmp(pkts[i].id, id)) return i;
    }
    return -1;
}

int main() {
    char cmd[20], id[100], header[1000], payload[1000];
    int hlen, plen;
    while (scanf("%s", cmd) == 1 && strcmp(cmd, "END")) {
        if (!strcmp(cmd, "PACKET")) {
            scanf("%s %d %d", id, &hlen, &plen);
            scanf("%s", header);
            fgets(payload, 1000, stdin);
            if (payload[0] == ' ') memmove(payload, payload + 1, strlen(payload));
            payload[strcspn(payload, "\n")] = 0;
            if ((int)strlen(header) == hlen && (int)strlen(payload) == plen) {
                strcpy(pkts[pn].id, id);
                pkts[pn].hlen = hlen;
                pkts[pn].plen = plen;
                strcpy(pkts[pn].header, header);
                strcpy(pkts[pn].payload, payload);
                pn++;
            }
        } else if (!strcmp(cmd, "DROP")) {
            scanf("%s", id);
            int i = findPkt(id);
            if (i != -1) {
                for (int j = i; j < pn - 1; j++) {
                    pkts[j] = pkts[j + 1];
                }
                pn--;
            }
        } else if (!strcmp(cmd, "HEADER")) {
            scanf("%s", id);
            int i = findPkt(id);
            if (i != -1) {
                printf("%s\n", pkts[i].header);
            }
        } else if (!strcmp(cmd, "PAYLOAD")) {
            scanf("%s", id);
            int i = findPkt(id);
            if (i != -1) {
                printf("%s\n", pkts[i].payload);
            }
        } else if (!strcmp(cmd, "STATS")) {
            int total = 0;
            for (int i = 0; i < pn; i++) {
                total += pkts[i].plen;
            }
            printf("%d\n", total);
        }
    }
    return 0;
}