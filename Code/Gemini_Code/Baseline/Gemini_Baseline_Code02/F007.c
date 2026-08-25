// F007.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *read_line(FILE *f) {
    size_t size = 16;
    size_t len = 0;
    char *str = malloc(size);
    if (!str) return NULL;
    int c;
    while ((c = fgetc(f)) != EOF && c != '\n') {
        if (len + 1 >= size) {
            size *= 2;
            char *new_str = realloc(str, size);
            if (!new_str) { free(str); return NULL; }
            str = new_str;
        }
        str[len++] = c;
    }
    if (len == 0 && c == EOF) { free(str); return NULL; }
    str[len] = '\0';
    return str;
}

typedef struct {
    long time;
    char *label;
    char *note;
} Act;

int compare_act(const void *a, const void *b) {
    Act *actA = (Act *)a;
    Act *actB = (Act *)b;
    if (actA->time != actB->time) {
        return (actA->time > actB->time) - (actA->time < actB->time);
    }
    return strcmp(actA->label, actB->label);
}

int main() {
    Act *acts = NULL;
    int count = 0;
    while (1) {
        char *line = read_line(stdin);
        if (!line) break;
        if (strcmp(line, "END") == 0) { free(line); break; }
        char *cmd = line;
        char *p1 = strchr(cmd, ' ');
        if (!p1) {
            if (strcmp(cmd, "SORT") == 0) {
                if (count > 0) {
                    Act *sorted = malloc(sizeof(Act) * count);
                    memcpy(sorted, acts, sizeof(Act) * count);
                    qsort(sorted, count, sizeof(Act), compare_act);
                    for (int i = 0; i < count; i++) {
                        printf("%ld %s %s\n", sorted[i].time, sorted[i].label, sorted[i].note);
                    }
                    free(sorted);
                }
            } else if (strcmp(cmd, "CLEAR") == 0) {
                for (int i = 0; i < count; i++) {
                    free(acts[i].label);
                    free(acts[i].note);
                }
                free(acts);
                acts = NULL;
                count = 0;
            }
            free(line);
            continue;
        }
        *p1 = '\0';
        char *args = p1 + 1;
        if (strcmp(cmd, "ADD") == 0) {
            char *p2 = strchr(args, ' ');
            if (p2) {
                *p2 = '\0';
                long tm = atol(args);
                char *label = p2 + 1;
                char *p3 = strchr(label, ' ');
                if (p3) {
                    *p3 = '\0';
                    char *note = p3 + 1;
                    acts = realloc(acts, sizeof(Act) * (count + 1));
                    acts[count].time = tm;
                    acts[count].label = strdup(label);
                    acts[count].note = strdup(note);
                    count++;
                }
            }
        } else if (strcmp(cmd, "DELETE") == 0) {
            char *p2 = strchr(args, ' ');
            if (p2) {
                *p2 = '\0';
                long tm = atol(args);
                char *label = p2 + 1;
                int i = 0;
                while (i < count) {
                    if (acts[i].time == tm && strcmp(acts[i].label, label) == 0) {
                        free(acts[i].label);
                        free(acts[i].note);
                        for (int j = i; j < count - 1; j++) {
                            acts[j] = acts[j + 1];
                        }
                        count--;
                    } else {
                        i++;
                    }
                }
                if (count == 0) { free(acts); acts = NULL; }
                else { acts = realloc(acts, sizeof(Act) * count); }
            }
        } else if (strcmp(cmd, "FIND") == 0) {
            char *label = args;
            int best_idx = -1;
            for (int i = 0; i < count; i++) {
                if (strcmp(acts[i].label, label) == 0) {
                    if (best_idx == -1 || acts[i].time < acts[best_idx].time) {
                        best_idx = i;
                    }
                }
            }
            if (best_idx != -1) {
                printf("%ld %s %s\n", acts[best_idx].time, acts[best_idx].label, acts[best_idx].note);
            }
        }
        free(line);
    }
    for (int i = 0; i < count; i++) {
        free(acts[i].label);
        free(acts[i].note);
    }
    free(acts);
    return 0;
}
