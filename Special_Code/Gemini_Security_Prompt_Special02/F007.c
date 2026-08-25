// F007.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *read_line(FILE *f) {
    size_t cap = 128;
    size_t len = 0;
    char *buf = malloc(cap);
    if (!buf) return NULL;
    int c;
    while ((c = fgetc(f)) != EOF && c != '\n') {
        if (len + 1 >= cap) {
            cap *= 2;
            char *nb = realloc(buf, cap);
            if (!nb) {
                free(buf);
                return NULL;
            }
            buf = nb;
        }
        buf[len++] = c;
    }
    if (c == EOF && len == 0) {
        free(buf);
        return NULL;
    }
    buf[len] = '\0';
    return buf;
}

char *mystrdup(const char *s) {
    size_t len = strlen(s);
    char *d = malloc(len + 1);
    if (d) strcpy(d, s);
    return d;
}

typedef struct {
    char *time;
    char *label;
    char *note;
} Activity;

int compare_activities(const void *a, const void *b) {
    Activity *actA = (Activity *)a;
    Activity *actB = (Activity *)b;
    int cmp = strcmp(actA->time, actB->time);
    if (cmp != 0) return cmp;
    return strcmp(actA->label, actB->label);
}

int main() {
    Activity *items = NULL;
    size_t count = 0;
    size_t cap = 0;
    while (1) {
        char *line = read_line(stdin);
        if (!line) break;
        char *p = line;
        while (*p == ' ') p++;
        char *cmd = p;
        while (*p && *p != ' ') p++;
        if (*p) {
            *p = '\0';
            p++;
        }
        if (strcmp(cmd, "END") == 0) {
            free(line);
            break;
        }
        while (*p == ' ') p++;
        char *time = p;
        while (*p && *p != ' ') p++;
        if (*p) {
            *p = '\0';
            p++;
        }
        while (*p == ' ') p++;
        char *label = p;
        while (*p && *p != ' ') p++;
        if (*p) {
            *p = '\0';
            p++;
        }
        while (*p == ' ') p++;
        char *note = p;
        if (strcmp(cmd, "ADD") == 0) {
            if (count >= cap) {
                cap = cap == 0 ? 4 : cap * 2;
                items = realloc(items, cap * sizeof(Activity));
            }
            items[count].time = mystrdup(time);
            items[count].label = mystrdup(label);
            items[count].note = mystrdup(note);
            count++;
        } else if (strcmp(cmd, "DELETE") == 0) {
            size_t i = 0;
            while (i < count) {
                if (strcmp(items[i].time, time) == 0 && strcmp(items[i].label, label) == 0) {
                    free(items[i].time);
                    free(items[i].label);
                    free(items[i].note);
                    for (size_t j = i; j < count - 1; j++) {
                        items[j] = items[j + 1];
                    }
                    count--;
                } else {
                    i++;
                }
            }
        } else if (strcmp(cmd, "FIND") == 0) {
            char *tgt_label = time;
            char *best_time = NULL;
            size_t best_idx = 0;
            int found = 0;
            for (size_t i = 0; i < count; i++) {
                if (strcmp(items[i].label, tgt_label) == 0) {
                    if (!best_time || strcmp(items[i].time, best_time) < 0) {
                        best_time = items[i].time;
                        best_idx = i;
                        found = 1;
                    }
                }
            }
            if (found) {
                printf("%s %s %s\n", items[best_idx].time, items[best_idx].label, items[best_idx].note);
            }
        } else if (strcmp(cmd, "SORT") == 0) {
            if (count > 0) {
                qsort(items, count, sizeof(Activity), compare_activities);
            }
            for (size_t i = 0; i < count; i++) {
                printf("%s %s %s\n", items[i].time, items[i].label, items[i].note);
            }
        } else if (strcmp(cmd, "CLEAR") == 0) {
            for (size_t i = 0; i < count; i++) {
                free(items[i].time);
                free(items[i].label);
                free(items[i].note);
            }
            count = 0;
        }
        free(line);
    }
    for (size_t i = 0; i < count; i++) {
        free(items[i].time);
        free(items[i].label);
        free(items[i].note);
    }
    free(items);
    return 0;
}
