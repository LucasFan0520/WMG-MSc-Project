// F040.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Pkt {
    char *id;
    int hlen;
    int plen;
    char *header;
    char *payload;
    struct Pkt *next;
} Pkt;

Pkt *head = NULL;

void add_pkt(const char *id, int hlen, int plen, const char *header, const char *payload) {
    if ((int)strlen(header) != hlen || (int)strlen(payload) != plen) return;
    Pkt *p = malloc(sizeof(Pkt));
    p->id = strdup(id);
    p->hlen = hlen;
    p->plen = plen;
    p->header = strdup(header);
    p->payload = strdup(payload);
    p->next = head;
    head = p;
}

void drop_pkt(const char *id) {
    Pkt *p = head, *prev = NULL;
    while (p) {
        if (strcmp(p->id, id) == 0) {
            if (prev) prev->next = p->next;
            else head = p->next;
            free(p->id); free(p->header); free(p->payload); free(p);
            return;
        }
        prev = p; p = p->next;
    }
}

void print_header(const char *id) {
    for (Pkt *p = head; p; p = p->next) {
        if (strcmp(p->id, id) == 0) {
            printf("%s\n", p->header);
            return;
        }
    }
}

void print_payload(const char *id) {
    for (Pkt *p = head; p; p = p->next) {
        if (strcmp(p->id, id) == 0) {
            printf("%s\n", p->payload);
            return;
        }
    }
}

void stats_pkt() {
    long long total = 0;
    for (Pkt *p = head; p; p = p->next) {
        total += p->plen;
    }
    printf("%lld\n", total);
}

int main() {
    char line[10000];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        if (strcmp(line, "END") == 0) break;
        char cmd[20], id[1000], header[1000], payload[8000];
        int hlen, plen;
        header[0] = payload[0] = 0;
        sscanf(line, "%s", cmd);
        if (strcmp(cmd, "PACKET") == 0) {
            sscanf(line, "%*s %s %d %d %s %[^\n]", id, &hlen, &plen, header, payload);
            add_pkt(id, hlen, plen, header, payload);
        } else if (strcmp(cmd, "DROP") == 0) {
            sscanf(line, "%*s %s", id);
            drop_pkt(id);
        } else if (strcmp(cmd, "HEADER") == 0) {
            sscanf(line, "%*s %s", id);
            print_header(id);
        } else if (strcmp(cmd, "PAYLOAD") == 0) {
            sscanf(line, "%*s %s", id);
            print_payload(id);
        } else if (strcmp(cmd, "STATS") == 0) {
            stats_pkt();
        }
    }
    while (head) {
        Pkt *n = head->next;
        free(head->id); free(head->header); free(head->payload); free(head);
        head = n;
    }
    return 0;
}