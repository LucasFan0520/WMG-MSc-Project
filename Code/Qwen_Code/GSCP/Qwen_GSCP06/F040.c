// F040.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_LINE 8192

char* get_word(char** str) {
    while (**str && isspace((unsigned char)**str)) (*str)++;
    if (!**str) return NULL;
    char* start = *str;
    while (**str && !isspace((unsigned char)**str)) (*str)++;
    if (**str) { **str = '\0'; (*str)++; }
    return start;
}

typedef struct Pkt {
    char* id;
    int hlen;
    int plen;
    char* header;
    char* payload;
    struct Pkt* next;
} Pkt;

Pkt* head = NULL;

Pkt* find_pkt(const char* id) {
    for (Pkt* p = head; p; p = p->next)
        if (strcmp(p->id, id) == 0) return p;
    return NULL;
}

int main() {
    char line[MAX_LINE];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        if (strcmp(line, "END") == 0) break;
        char* p = line;
        char* cmd = get_word(&p);
        if (!cmd) continue;

        if (strcmp(cmd, "PACKET") == 0) {
            char* id = get_word(&p);
            char* hl_str = get_word(&p);
            char* pl_str = get_word(&p);
            char* header = get_word(&p);
            if (id && hl_str && pl_str && header) {
                int hl = atoi(hl_str);
                int pl = atoi(pl_str);
                char* payload = p;
                if ((int)strlen(header) == hl && (int)strlen(payload) == pl) {
                    Pkt* n = malloc(sizeof(Pkt));
                    n->id = strdup(id);
                    n->hlen = hl;
                    n->plen = pl;
                    n->header = strdup(header);
                    n->payload = strdup(payload);
                    n->next = head;
                    head = n;
                }
            }
        } else if (strcmp(cmd, "DROP") == 0) {
            char* id = get_word(&p);
            Pkt* prev = NULL;
            for (Pkt* pkt = head; pkt; prev = pkt, pkt = pkt->next) {
                if (strcmp(pkt->id, id) == 0) {
                    if (prev) prev->next = pkt->next;
                    else head = pkt->next;
                    free(pkt->id);
                    free(pkt->header);
                    free(pkt->payload);
                    free(pkt);
                    break;
                }
            }
        } else if (strcmp(cmd, "HEADER") == 0) {
            char* id = get_word(&p);
            Pkt* pkt = find_pkt(id);
            if (pkt) printf("%s\n", pkt->header);
        } else if (strcmp(cmd, "PAYLOAD") == 0) {
            char* id = get_word(&p);
            Pkt* pkt = find_pkt(id);
            if (pkt) printf("%s\n", pkt->payload);
        } else if (strcmp(cmd, "STATS") == 0) {
            long total = 0;
            for (Pkt* pkt = head; pkt; pkt = pkt->next) {
                total += pkt->plen;
            }
            printf("%ld\n", total);
        }
    }
    return 0;
}