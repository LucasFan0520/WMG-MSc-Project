// F040.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_ID 100
#define MAX_DATA 10000

typedef struct {
    char id[MAX_ID];
    int headerLength;
    int payloadLength;
    char header[MAX_DATA];
    char payload[MAX_DATA];
} Packet;

Packet packets[1000];
int pkt_count = 0;

int find_pkt(const char *id) {
    for (int i = 0; i < pkt_count; i++) if (strcmp(packets[i].id, id) == 0) return i;
    return -1;
}

int main() {
    char cmd[20], id[MAX_ID], header[MAX_DATA], payload[MAX_DATA];
    int hLen, pLen;
    while (scanf("%s", cmd) == 1) {
        if (strcmp(cmd, "END") == 0) break;
        if (strcmp(cmd, "PACKET") == 0) {
            scanf("%s %d %d %s %s", id, &hLen, &pLen, header, payload);
            if ((int)strlen(header) == hLen && (int)strlen(payload) == pLen) {
                int idx = find_pkt(id);
                if (idx == -1) idx = pkt_count++;
                strcpy(packets[idx].id, id);
                packets[idx].headerLength = hLen;
                packets[idx].payloadLength = pLen;
                strcpy(packets[idx].header, header);
                strcpy(packets[idx].payload, payload);
            }
        } else if (strcmp(cmd, "DROP") == 0) {
            scanf("%s", id);
            int idx = find_pkt(id);
            if (idx != -1) {
                for (int i = idx; i < pkt_count - 1; i++) packets[i] = packets[i + 1];
                pkt_count--;
            }
        } else if (strcmp(cmd, "HEADER") == 0) {
            scanf("%s", id);
            int idx = find_pkt(id);
            if (idx != -1) printf("%s\n", packets[idx].header);
        } else if (strcmp(cmd, "PAYLOAD") == 0) {
            scanf("%s", id);
            int idx = find_pkt(id);
            if (idx != -1) printf("%s\n", packets[idx].payload);
        } else if (strcmp(cmd, "STATS") == 0) {
            int total = 0;
            for (int i = 0; i < pkt_count; i++) total += packets[i].payloadLength;
            printf("%d\n", total);
        }
    }
    return 0;
}