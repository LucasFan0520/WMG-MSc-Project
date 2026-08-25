// F040.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct { char *id; int hlen; int plen; char *header; char *payload; } Pkt;
Pkt *pkts = NULL;
int np = 0;

int main() {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        char cmd[20], a[256];
        int hl, pl;
        if (sscanf(line, "%s", cmd) != 1) continue;
        if (strcmp(cmd, "END") == 0) break;
        
        if (strcmp(cmd, "PACKET") == 0) {
            sscanf(line, "%*s %s %d %d", a, &hl, &pl);
            char *t = strchr(line, ' '); t++; t = strchr(t, ' '); t++; t = strchr(t, ' '); t++; t = strchr(t, ' '); t++;
            char *payload_start = t + hl;
            if (*payload_start == ' ') payload_start++;
            if ((int)strlen(t) >= hl && (int)strlen(payload_start) == pl) {
                pkts = realloc(pkts, (np + 1) * sizeof(Pkt));
                pkts[np].id = strdup(a);
                pkts[np].hlen = hl;
                pkts[np].plen = pl;
                pkts[np].header = malloc(hl + 1);
                strncpy(pkts[np].header, t, hl);
                pkts[np].header[hl] = 0;
                pkts[np].payload = strdup(payload_start);
                np++;
            }
        } else if (strcmp(cmd, "DROP") == 0) {
            sscanf(line, "%*s %s", a);
            for (int i = 0; i < np; i++) {
                if (strcmp(pkts[i].id, a) == 0) {
                    free(pkts[i].id); free(pkts[i].header); free(pkts[i].payload);
                    for (int j = i; j < np - 1; j++) pkts[j] = pkts[j + 1];
                    np--;
                    break;
                }
            }
        } else if (strcmp(cmd, "HEADER") == 0) {
            sscanf(line, "%*s %s", a);
            for (int i = 0; i < np; i++) {
                if (strcmp(pkts[i].id, a) == 0) {
                    printf("%s\n", pkts[i].header);
                    break;
                }
            }
        } else if (strcmp(cmd, "PAYLOAD") == 0) {
            sscanf(line, "%*s %s", a);
            for (int i = 0; i < np; i++) {
                if (strcmp(pkts[i].id, a) == 0) {
                    printf("%s\n", pkts[i].payload);
                    break;
                }
            }
        } else if (strcmp(cmd, "STATS") == 0) {
            long long total = 0;
            for (int i = 0; i < np; i++) total += pkts[i].plen;
            printf("%lld\n", total);
        }
    }
    for (int i = 0; i < np; i++) { free(pkts[i].id); free(pkts[i].header); free(pkts[i].payload); }
    free(pkts);
    return 0;
}