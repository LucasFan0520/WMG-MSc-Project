// F040.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Pkt { char *id; int hlen, plen; char *header; char *payload; struct Pkt *next; } Pkt;
Pkt *pkts = NULL;

void cmd_packet(char *id, int hl, int pl, char *h, char *p) {
    if ((int)strlen(h) != hl || (int)strlen(p) != pl) return;
    Pkt *pk = malloc(sizeof(Pkt)); pk->id = strdup(id); pk->hlen = hl; pk->plen = pl; pk->header = strdup(h); pk->payload = strdup(p); pk->next = pkts; pkts = pk;
}
void cmd_drop(char *id) {
    Pkt *curr = pkts, *prev = NULL;
    while (curr) { if (!strcmp(curr->id, id)) { if (prev) prev->next = curr->next; else pkts = curr->next; free(curr->id); free(curr->header); free(curr->payload); free(curr); return; } prev = curr; curr = curr->next; }
}
void cmd_header(char *id) { for (Pkt *p = pkts; p; p = p->next) if (!strcmp(p->id, id)) { printf("%s\n", p->header); return; } }
void cmd_payload(char *id) { for (Pkt *p = pkts; p; p = p->next) if (!strcmp(p->id, id)) { printf("%s\n", p->payload); return; } }
void cmd_stats() {
    long total = 0; for (Pkt *p = pkts; p; p = p->next) total += p->plen;
    printf("%ld\n", total);
}
void free_all() { Pkt *p = pkts; while (p) { Pkt *np = p->next; free(p->id); free(p->header); free(p->payload); free(p); p = np; } }
int main() {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0; if (!strcmp(line, "END")) break;
        char cmd[32], a1[256], a2[256], a3[256]; int hl, pl;
        if (sscanf(line, "%31s", cmd) != 1) continue;
        if (!strcmp(cmd, "PACKET") && sscanf(line, "%*s %255s %d %d %255s %255s", a1, &hl, &pl, a2, a3) == 5) cmd_packet(a1, hl, pl, a2, a3);
        else if (!strcmp(cmd, "DROP") && sscanf(line, "%*s %255s", a1) == 1) cmd_drop(a1);
        else if (!strcmp(cmd, "HEADER") && sscanf(line, "%*s %255s", a1) == 1) cmd_header(a1);
        else if (!strcmp(cmd, "PAYLOAD") && sscanf(line, "%*s %255s", a1) == 1) cmd_payload(a1);
        else if (!strcmp(cmd, "STATS")) cmd_stats();
    }
    free_all(); return 0;
}