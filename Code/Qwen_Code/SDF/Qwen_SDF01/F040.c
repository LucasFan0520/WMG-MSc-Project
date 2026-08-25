// F040.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

typedef struct { char *id; char *header; char *payload; } Pkt;
typedef struct { Pkt *d; size_t len, cap; } PL;

char *read_line(void) {
    char *l = NULL;
    size_t len = 0;
    if (getline(&l, &len, stdin) == -1) {
        free(l);
        return NULL;
    }
    size_t sl = strlen(l);
    if (sl > 0 && l[sl - 1] == '\n') l[sl - 1] = '\0';
    return l;
}

char *next_token(char **s) {
    if (!*s) return NULL;
    while (**s == ' ') (*s)++;
    if (!**s) return NULL;
    char *st = *s;
    while (**s && **s != ' ') (*s)++;
    if (**s) { **s = '\0'; (*s)++; }
    return st;
}

int find_pkt(PL *l, const char *id) {
    for (size_t i = 0; i < l->len; i++) if (strcmp(l->d[i].id, id) == 0) return i;
    return -1;
}

int main(void) {
    PL list = {0};
    char *line;
    while ((line = read_line()) != NULL) {
        if (strcmp(line, "END") == 0) { free(line); break; }
        char *p = line;
        char *cmd = next_token(&p);
        if (!cmd) { free(line); continue; }
        
        if (strcmp(cmd, "PACKET") == 0) {
            char *id = next_token(&p);
            char *hl_s = next_token(&p);
            char *pl_s = next_token(&p);
            char *header = next_token(&p);
            char *payload = p;
            if (id && hl_s && pl_s && header && payload) {
                long hl = strtol(hl_s, NULL, 10);
                long pl = strtol(pl_s, NULL, 10);
                if (hl >= 0 && pl >= 0 && (size_t)hl == strlen(header) && (size_t)pl == strlen(payload)) {
                    if (find_pkt(&list, id) == -1) {
                        if (list.len == list.cap) {
                            size_t nc = list.cap ? list.cap * 2 : 4;
                            Pkt *nd = realloc(list.d, nc * sizeof(Pkt));
                            if (nd) { list.d = nd; list.cap = nc; }
                        }
                        if (list.len < list.cap) {
                            list.d[list.len].id = strdup(id);
                            list.d[list.len].header = strdup(header);
                            list.d[list.len].payload = strdup(payload);
                            list.len++;
                        }
                    }
                }
            }
        } else if (strcmp(cmd, "DROP") == 0) {
            char *id = next_token(&p);
            if (id) {
                int idx = find_pkt(&list, id);
                if (idx != -1) {
                    free(list.d[idx].id);
                    free(list.d[idx].header);
                    free(list.d[idx].payload);
                    for (size_t i = idx; i < list.len - 1; i++) list.d[i] = list.d[i + 1];
                    list.len--;
                }
            }
        } else if (strcmp(cmd, "HEADER") == 0) {
            char *id = next_token(&p);
            if (id) {
                int idx = find_pkt(&list, id);
                if (idx != -1) {
                    printf("%s\n", list.d[idx].header);
                }
            }
        } else if (strcmp(cmd, "PAYLOAD") == 0) {
            char *id = next_token(&p);
            if (id) {
                int idx = find_pkt(&list, id);
                if (idx != -1) {
                    printf("%s\n", list.d[idx].payload);
                }
            }
        } else if (strcmp(cmd, "STATS") == 0) {
            long total = 0;
            for (size_t i = 0; i < list.len; i++) {
                total += strlen(list.d[i].payload);
            }
            printf("%ld\n", total);
        }
        free(line);
    }
    for (size_t i = 0; i < list.len; i++) {
        free(list.d[i].id);
        free(list.d[i].header);
        free(list.d[i].payload);
    }
    free(list.d);
    return 0;
}