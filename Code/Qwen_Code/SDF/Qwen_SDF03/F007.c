// F007.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct { long time; char *label, *note; } Act;
Act *acts = NULL;
size_t count = 0, cap = 0;

char *get_token(char **s) {
    while (**s == ' ') (*s)++;
    if (!**s) return NULL;
    char *start = *s;
    while (**s && **s != ' ') (*s)++;
    if (**s) { **s = '\0'; (*s)++; }
    return start;
}

int cmp(const void *a, const void *b) {
    const Act *x = a, *y = b;
    if (x->time != y->time) return (x->time > y->time) - (x->time < y->time);
    return strcmp(x->label, y->label);
}

int main() {
    char line[65536];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        char *ptr = line;
        char *cmd = get_token(&ptr);
        if (!cmd) continue;
        if (strcmp(cmd, "ADD") == 0) {
            char *t_str = get_token(&ptr);
            char *label = get_token(&ptr);
            if (t_str && label) {
                if (count == cap) {
                    size_t new_cap = cap ? cap * 2 : 4;
                    Act *tmp = realloc(acts, new_cap * sizeof(Act));
                    if (!tmp) continue;
                    acts = tmp; cap = new_cap;
                }
                acts[count].time = strtol(t_str, NULL, 10);
                acts[count].label = strdup(label);
                acts[count].note = strdup(ptr);
                count++;
            }
        } else if (strcmp(cmd, "DELETE") == 0) {
            char *t_str = get_token(&ptr);
            char *label = get_token(&ptr);
            if (t_str && label) {
                long t = strtol(t_str, NULL, 10);
                for (size_t i = 0; i < count; ) {
                    if (acts[i].time == t && strcmp(acts[i].label, label) == 0) {
                        free(acts[i].label); free(acts[i].note);
                        acts[i] = acts[--count];
                    } else i++;
                }
            }
        } else if (strcmp(cmd, "FIND") == 0) {
            char *label = get_token(&ptr);
            if (label) {
                long min_t = -1;
                for (size_t i = 0; i < count; i++) {
                    if (strcmp(acts[i].label, label) == 0) {
                        if (min_t == -1 || acts[i].time < min_t) min_t = acts[i].time;
                    }
                }
                if (min_t != -1) printf("%ld\n", min_t);
            }
        } else if (strcmp(cmd, "SORT") == 0) {
            if (count > 0) qsort(acts, count, sizeof(Act), cmp);
            for (size_t i = 0; i < count; i++) printf("%ld %s %s\n", acts[i].time, acts[i].label, acts[i].note);
        } else if (strcmp(cmd, "CLEAR") == 0) {
            for (size_t i = 0; i < count; i++) { free(acts[i].label); free(acts[i].note); }
            count = 0;
        }
    }
    for (size_t i = 0; i < count; i++) { free(acts[i].label); free(acts[i].note); }
    free(acts);
    return 0;
}