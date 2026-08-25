// F040.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct { char *id; int hlen; int plen; char *header; char *payload; } Pkt;
Pkt pkts[10000]; int npkts = 0;

int find_pkt(char *id) {
    for (int i = 0; i < npkts; i++) if (!strcmp(pkts[i].id, id)) return i;
    return -1;
}

int main() {
    char cmd[20], id[100], header[10000], payload[10000]; int hlen, plen;
    while (scanf("%s", cmd) == 1 && strcmp(cmd, "END")) {
        if (!strcmp(cmd, "PACKET")) {
            scanf("%s %d %d", id, &hlen, &plen);
            fgets(header, sizeof(header), stdin);
            header[strcspn(header, "\n")] = 0;
            if (header[0] == ' ') memmove(header, header + 1, strlen(header));
            fgets(payload, sizeof(payload), stdin);
            payload[strcspn(payload, "\n")] = 0;
            if (payload[0] == ' ') memmove(payload, payload + 1, strlen(payload));
            if ((int)strlen(header) == hlen && (int)strlen(payload) == plen) {
                pkts[npkts].id = strdup(id);
                pkts[npkts].hlen = hlen;
                pkts[npkts].plen = plen;
                pkts[npkts].header = strdup(header);
                pkts[npkts++].payload = strdup(payload);
            }
        } else if (!strcmp(cmd, "DROP")) {
            scanf("%s", id);
            int idx = find_pkt(id);
            if (idx != -1) {
                free(pkts[idx].id); free(pkts[idx].header); free(pkts[idx].payload);
                for (int i = idx; i < npkts - 1; i++) pkts[i] = pkts[i + 1];
                npkts--;
            }
        } else if (!strcmp(cmd, "HEADER")) {
            scanf("%s", id);
            int idx = find_pkt(id);
            if (idx != -1) printf("%s\n", pkts[idx].header);
        } else if (!strcmp(cmd, "PAYLOAD")) {
            scanf("%s", id);
            int idx = find_pkt(id);
            if (idx != -1) printf("%s\n", pkts[idx].payload);
        } else if (!strcmp(cmd, "STATS")) {
            long total = 0;
            for (int i = 0; i < npkts; i++) total += pkts[i].plen;
            printf("%ld\n", total);
        }
    }
    for (int i = 0; i < npkts; i++) { free(pkts[i].id); free(pkts[i].header); free(pkts[i].payload); }
    return 0;
}