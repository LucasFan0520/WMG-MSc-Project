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

Packet packets[1000];
int pc = 0;

int find_id(char *id) {
    for (int i = 0; i < pc; i++) if (strcmp(packets[i].id, id) == 0) return i;
    return -1;
}

int main() {
    char line[10000];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        if (strcmp(line, "END") == 0) break;
        char cmd[20];
        sscanf(line, "%s", cmd);
        char *rest = line + strlen(cmd);
        while (*rest == ' ') rest++;
        
        if (strcmp(cmd, "PACKET") == 0) {
            char id[1000];
            int hl, pl;
            sscanf(rest, "%s %d %d", id, &hl, &pl);
            char *p = rest;
            while (*p && *p != ' ') p++;
            while (*p == ' ') p++;
            while (*p && *p != ' ') p++;
            while (*p == ' ') p++;
            while (*p && *p != ' ') p++;
            while (*p == ' ') p++;
            if (find_id(id) == -1 && (int)strlen(p) >= hl + pl) {
                packets[pc].id = strdup(id);
                packets[pc].hlen = hl;
                packets[pc].plen = pl;
                packets[pc].header = malloc(hl + 1);
                strncpy(packets[pc].header, p, hl);
                packets[pc].header[hl] = 0;
                packets[pc].payload = malloc(pl + 1);
                strncpy(packets[pc].payload, p + hl, pl);
                packets[pc].payload[pl] = 0;
                pc++;
            }
        } else if (strcmp(cmd, "DROP") == 0) {
            int idx = find_id(rest);
            if (idx != -1) {
                free(packets[idx].id);
                free(packets[idx].header);
                free(packets[idx].payload);
                for (int i = idx; i < pc - 1; i++) packets[i] = packets[i+1];
                pc--;
            }
        } else if (strcmp(cmd, "HEADER") == 0) {
            int idx = find_id(rest);
            if (idx != -1) {
                printf("%s\n", packets[idx].header);
            }
        } else if (strcmp(cmd, "PAYLOAD") == 0) {
            int idx = find_id(rest);
            if (idx != -1) {
                printf("%s\n", packets[idx].payload);
            }
        } else if (strcmp(cmd, "STATS") == 0) {
            int total = 0;
            for (int i = 0; i < pc; i++) {
                total += packets[i].plen;
            }
            printf("%d\n", total);
        }
    }
    for (int i = 0; i < pc; i++) {
        free(packets[i].id);
        free(packets[i].header);
        free(packets[i].payload);
    }
    return 0;
}