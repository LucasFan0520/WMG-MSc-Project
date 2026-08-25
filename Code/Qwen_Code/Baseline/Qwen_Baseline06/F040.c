// F040.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct { char *id, *header, *payload; int hlen, plen; } Pkt;
Pkt *pkts; int n_pkt;

int find_pkt(char *id) {
    for(int i=0; i<n_pkt; i++) if(!strcmp(pkts[i].id, id)) return i;
    return -1;
}

int main() {
    char cmd[20], id[100], header[1000], payload[1000]; int hlen, plen;
    while(scanf("%s", cmd)==1 && strcmp(cmd, "END")) {
        if(!strcmp(cmd, "PACKET")) {
            scanf("%s %d %d %s %s", id, &hlen, &plen, header, payload);
            if((int)strlen(header) == hlen && (int)strlen(payload) == plen) {
                pkts = realloc(pkts, (n_pkt+1)*sizeof(Pkt));
                pkts[n_pkt].id = strdup(id);
                pkts[n_pkt].header = strdup(header);
                pkts[n_pkt].payload = strdup(payload);
                pkts[n_pkt].hlen = hlen;
                pkts[n_pkt].plen = plen;
                n_pkt++;
            }
        } else if(!strcmp(cmd, "DROP")) {
            scanf("%s", id);
            int idx = find_pkt(id);
            if(idx != -1) {
                free(pkts[idx].id); free(pkts[idx].header); free(pkts[idx].payload);
                for(int k=idx; k<n_pkt-1; k++) pkts[k] = pkts[k+1];
                n_pkt--;
            }
        } else if(!strcmp(cmd, "HEADER")) {
            scanf("%s", id);
            int idx = find_pkt(id);
            if(idx != -1) printf("%s\n", pkts[idx].header);
        } else if(!strcmp(cmd, "PAYLOAD")) {
            scanf("%s", id);
            int idx = find_pkt(id);
            if(idx != -1) printf("%s\n", pkts[idx].payload);
        } else if(!strcmp(cmd, "STATS")) {
            int total = 0;
            for(int i=0; i<n_pkt; i++) total += pkts[i].plen;
            printf("%d\n", total);
        }
    }
    return 0;
}