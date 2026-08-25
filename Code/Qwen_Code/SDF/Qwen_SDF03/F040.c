// F040.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_LINE 65536

typedef struct Pkt {
    char *id;
    char *header;
    char *payload;
    struct Pkt *next;
} Pkt;

Pkt *head = NULL;

char *get_token(char **s) {
    while (**s == ' ') (*s)++;
    if (!**s) return NULL;
    char *start = *s;
    while (**s && **s != ' ') (*s)++;
    if (**s) { **s = '\0'; (*s)++; }
    return start;
}

Pkt *find_pkt(const char *id) {
    for (Pkt *p = head; p; p = p->next)
        if (strcmp(p->id, id) == 0) return p;
    return NULL;
}

int main() {
    char line[MAX_LINE];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        char *ptr = line;
        char *cmd = get_token(&ptr);
        if (!cmd) continue;
        if (strcmp(cmd, "PACKET") == 0) {
            char *id = get_token(&ptr);
            char *hstr = get_token(&ptr);
            char *pstr = get_token(&ptr);
            if (id && hstr && pstr) {
                long hlen = strtol(hstr, NULL, 10);
                long plen = strtol(pstr, NULL, 10);
                if (hlen >= 0 && plen >= 0 && (size_t)hlen <= strlen(ptr) && (size_t)plen <= strlen(ptr + hlen)) {
                    Pkt *p = malloc(sizeof(Pkt));
                    if (p) {
                        p->id = strdup(id);
                        p->header = malloc(hlen + 1);
                        p->payload = malloc(plen + 1);
                        if (p->header && p->payload) {
                            memcpy(p->header, ptr, hlen);
                            p->header[hlen] = '\0';
                            memcpy(p->payload, ptr + hlen, plen);
                            p->payload[plen] = '\0';
                            p->next = head;
                            head = p;
                        } else {
                            free(p->header);
                            free(p->payload);
                            free(p->id);
                            free(p);
                        }
                    }
                }
            }
        } else if (strcmp(cmd, "DROP") == 0) {
            char *id = get_token(&ptr);
            if (id) {
                Pkt *prev = NULL, *curr = head;
                while (curr) {
                    if (strcmp(curr->id, id) == 0) {
                        if (prev) prev->next = curr->next;
                        else head = curr->next;
                        free(curr->id);
                        free(curr->header);
                        free(curr->payload);
                        free(curr);
                        break;
                    }
                    prev = curr;
                    curr = curr->next;
                }
            }
        } else if (strcmp(cmd, "HEADER") == 0) {
            char *id = get_token(&ptr);
            if (id) {
                Pkt *p = find_pkt(id);
                if (p) printf("%s\n", p->header);
            }
        } else if (strcmp(cmd, "PAYLOAD") == 0) {
            char *id = get_token(&ptr);
            if (id) {
                Pkt *p = find_pkt(id);
                if (p) printf("%s\n", p->payload);
            }
        } else if (strcmp(cmd, "STATS") == 0) {
            size_t total = 0;
            for (Pkt *p = head; p; p = p->next) {
                total += strlen(p->payload);
            }
            printf("%zu\n", total);
        }
    }
    while (head) {
        Pkt *next = head->next;
        free(head->id);
        free(head->header);
        free(head->payload);
        free(head);
        head = next;
    }
    return 0;
}