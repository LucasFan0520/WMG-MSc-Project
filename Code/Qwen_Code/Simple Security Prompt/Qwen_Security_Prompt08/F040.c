// F040.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Pkt {
    char *id;
    int hLen;
    int pLen;
    char *header;
    char *payload;
    struct Pkt *next;
} Pkt;

Pkt *head = NULL;

int main() {
    char cmd[20];
    char id[100];
    char header[1000];
    char payload[2000];
    int hLen, pLen;
    while (scanf("%s", cmd) == 1 && strcmp(cmd, "END")) {
        if (!strcmp(cmd, "PACKET")) {
            scanf("%s %d %d %s", id, &hLen, &pLen, header);
            fgets(payload, 2000, stdin);
            payload[strcspn(payload, "\n")] = 0;
            if (payload[0] == ' ') memmove(payload, payload + 1, strlen(payload));
            if ((int)strlen(header) == hLen && (int)strlen(payload) == pLen) {
                Pkt *p = malloc(sizeof(Pkt));
                p->id = strdup(id);
                p->hLen = hLen;
                p->pLen = pLen;
                p->header = strdup(header);
                p->payload = strdup(payload);
                p->next = head;
                head = p;
            }
        } else if (!strcmp(cmd, "DROP")) {
            scanf("%s", id);
            Pkt *p = head;
            Pkt *prev = NULL;
            while (p) {
                if (!strcmp(p->id, id)) {
                    if (prev) prev->next = p->next;
                    else head = p->next;
                    free(p->id);
                    free(p->header);
                    free(p->payload);
                    free(p);
                    break;
                }
                prev = p;
                p = p->next;
            }
        } else if (!strcmp(cmd, "HEADER")) {
            scanf("%s", id);
            for (Pkt *p = head; p; p = p->next) {
                if (!strcmp(p->id, id)) {
                    printf("%s\n", p->header);
                    break;
                }
            }
        } else if (!strcmp(cmd, "PAYLOAD")) {
            scanf("%s", id);
            for (Pkt *p = head; p; p = p->next) {
                if (!strcmp(p->id, id)) {
                    printf("%s\n", p->payload);
                    break;
                }
            }
        } else if (!strcmp(cmd, "STATS")) {
            int total = 0;
            for (Pkt *p = head; p; p = p->next) {
                total += p->pLen;
            }
            printf("%d\n", total);
        }
    }
    return 0;
}