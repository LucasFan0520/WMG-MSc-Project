// F040.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct { char *id; int hLen, pLen; char *header, *payload; } Pkt;
Pkt *pkts = 0; int n = 0, cap = 0;

int find(char *id) {
    for (int i = 0; i < n; i++) if (!strcmp(pkts[i].id, id)) return i;
    return -1;
}

int main() {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        if (!strcmp(line, "END")) break;
        char cmd[20], a[256], b[2048], c[2048]; int hl, pl;
        a[0] = b[0] = c[0] = 0;
        sscanf(line, "%s", cmd);
        if (!strcmp(cmd, "PACKET")) sscanf(line, "%*s %s %d %d %s %s", a, &hl, &pl, b, c);
        else sscanf(line, "%*s %s", a);

        if (!strcmp(cmd, "PACKET")) {
            if ((int)strlen(b) == hl && (int)strlen(c) == pl) {
                int idx = find(a);
                if (idx >= 0) { free(pkts[idx].header); free(pkts[idx].payload); pkts[idx].header = strdup(b); pkts[idx].payload = strdup(c); }
                else {
                    if (n == cap) { cap = cap ? cap * 2 : 4; pkts = realloc(pkts, cap * sizeof(Pkt)); }
                    pkts[n].id = strdup(a); pkts[n].hLen = hl; pkts[n].pLen = pl; pkts[n].header = strdup(b); pkts[n].payload = strdup(c); n++;
                }
            }
        } else if (!strcmp(cmd, "DROP")) {
            int idx = find(a);
            if (idx >= 0) {
                free(pkts[idx].id); free(pkts[idx].header); free(pkts[idx].payload);
                for (int i = idx; i < n - 1; i++) pkts[i] = pkts[i + 1]; n--;
            }
        } else if (!strcmp(cmd, "HEADER")) {
            int idx = find(a);
            if (idx >= 0) puts(pkts[idx].header);
        } else if (!strcmp(cmd, "PAYLOAD")) {
            int idx = find(a);
            if (idx >= 0) puts(pkts[idx].payload);
        } else if (!strcmp(cmd, "STATS")) {
            long long total = 0;
            for (int i = 0; i < n; i++) total += pkts[i].pLen;
            printf("%lld\n", total);
        }
    }
    return 0;
}