// F040.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Pkt {
    char *id;
    int h_len, p_len;
    char *header, *payload;
    struct Pkt *next;
} Pkt;

Pkt *head = NULL;

int main() {
    char cmd[20], id[256];
    int h_len, p_len;
    char header[1024], payload[2048];
    while (scanf("%s", cmd) == 1 && strcmp(cmd, "END") != 0) {
        if (strcmp(cmd, "PACKET") == 0) {
            scanf("%s %d %d", id, &h_len, &p_len);
            scanf("%s", header);
            fgets(payload, sizeof(payload), stdin);
            if (payload[0] == ' ') memmove(payload, payload + 1, strlen(payload));
            size_t l = strlen(payload);
            if (l > 0 && payload[l-1] == '\n') payload[l-1] = '\0';
            
            if ((int)strlen(header) == h_len && (int)strlen(payload) == p_len) {
                Pkt *p = malloc(sizeof(Pkt));
                p->id = strdup(id); p->h_len = h_len; p->p_len = p_len;
                p->header = strdup(header); p->payload = strdup(payload); p->next = head; head = p;
            }
        } else if (strcmp(cmd, "DROP") == 0) {
            scanf("%s", id);
            Pkt *prev = NULL, *curr = head;
            while (curr) {
                if (strcmp(curr->id, id) == 0) {
                    if (prev) prev->next = curr->next; else head = curr->next;
                    free(curr->id); free(curr->header); free(curr->payload); free(curr); break;
                }
                prev = curr; curr = curr->next;
            }
        } else if (strcmp(cmd, "HEADER") == 0) {
            scanf("%s", id);
            for (Pkt *p = head; p; p = p->next) if (strcmp(p->id, id) == 0) { printf("%s\n", p->header); break; }
        } else if (strcmp(cmd, "PAYLOAD") == 0) {
            scanf("%s", id);
            for (Pkt *p = head; p; p = p->next) if (strcmp(p->id, id) == 0) { printf("%s\n", p->payload); break; }
        } else if (strcmp(cmd, "STATS") == 0) {
            long long total = 0;
            for (Pkt *p = head; p; p = p->next) total += p->p_len;
            printf("%lld\n", total);
        }
    }
    return 0;
}