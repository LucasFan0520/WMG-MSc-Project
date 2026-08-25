// F040.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Packet {
    char *id;
    int hLen, pLen;
    char *header;
    char *payload;
    struct Packet *next;
} Packet;

Packet *head = NULL;

void free_list() {
    while (head) {
        Packet *p = head; head = head->next;
        free(p->id); free(p->header); free(p->payload); free(p);
    }
}

Packet *find_packet(const char *id) {
    for (Packet *p = head; p; p = p->next)
        if (strcmp(p->id, id) == 0) return p;
    return NULL;
}

int main() {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        char cmd[16], id[256], header[4096] = "", payload[4096] = "";
        int hLen, pLen;
        if (sscanf(line, "%15s", cmd) != 1) continue;
        if (strcmp(cmd, "END") == 0) break;

        if (strcmp(cmd, "PACKET") == 0) {
            if (sscanf(line, "%*s %255s %d %d %4095s %4095s", id, &hLen, &pLen, header, payload) != 5) continue;
            if ((int)strlen(header) != hLen || (int)strlen(payload) != pLen) continue;
            if (find_packet(id)) continue;
            Packet *p = malloc(sizeof(Packet));
            p->id = strdup(id); p->hLen = hLen; p->pLen = pLen;
            p->header = strdup(header); p->payload = strdup(payload);
            p->next = head; head = p;
        } else if (strcmp(cmd, "DROP") == 0) {
            if (sscanf(line, "%*s %255s", id) != 1) continue;
            Packet *prev = NULL, *curr = head;
            while (curr) {
                if (strcmp(curr->id, id) == 0) {
                    Packet *tmp = curr; curr = curr->next;
                    if (prev) prev->next = curr; else head = curr;
                    free(tmp->id); free(tmp->header); free(tmp->payload); free(tmp);
                    break;
                }
                prev = curr; curr = curr->next;
            }
        } else if (strcmp(cmd, "HEADER") == 0) {
            if (sscanf(line, "%*s %255s", id) != 1) continue;
            Packet *p = find_packet(id);
            if (p) printf("%s\n", p->header);
        } else if (strcmp(cmd, "PAYLOAD") == 0) {
            if (sscanf(line, "%*s %255s", id) != 1) continue;
            Packet *p = find_packet(id);
            if (p) printf("%s\n", p->payload);
        } else if (strcmp(cmd, "STATS") == 0) {
            long long total = 0;
            for (Packet *p = head; p; p = p->next) total += p->pLen;
            printf("%lld\n", total);
        }
    }
    free_list();
    return 0;
}