/* F007.c */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *read_line(FILE *fp) {
    size_t cap = 16;
    size_t len = 0;
    char *buf = malloc(cap);
    if (!buf) return NULL;
    int c;
    while ((c = fgetc(fp)) != EOF && c != '\n') {
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
    if (len == 0 && c == EOF) {
        free(buf);
        return NULL;
    }
    if (len > 0 && buf[len - 1] == '\n') {
        len--;
    }
    buf[len] = '\0';
    return buf;
}

char *mystrdup(const char *s) {
    size_t l = strlen(s);
    char *d = malloc(l + 1);
    if (d) strcpy(d, s);
    return d;
}

typedef struct {
    long long time;
    char *label;
    char *note;
    size_t id;
} Activity;

int compare_activities(const void *a, const void *b) {
    const Activity *ra = (const Activity *)a;
    const Activity *rb = (const Activity *)b;
    if (ra->time < rb->time) return -1;
    if (ra->time > rb->time) return 1;
    int lc = strcmp(ra->label, rb->label);
    if (lc != 0) return lc;
    return (ra->id < rb->id) ? -1 : 1;
}

int main(void) {
    Activity *log = NULL;
    size_t count = 0;
    size_t cap = 0;
    size_t seq = 0;
    while (1) {
        char *line = read_line(stdin);
        if (!line) break;
        char *p = line;
        while (*p == ' ') p++;
        if (*p == '\0') {
            free(line);
            continue;
        }
        char *cmd = p;
        while (*p && *p != ' ') p++;
        if (*p == ' ') {
            *p = '\0';
            p++;
        }
        if (strcmp(cmd, "END") == 0) {
            free(line);
            break;
        }
        if (strcmp(cmd, "ADD") == 0) {
            while (*p == ' ') p++;
            char *time_str = p;
            while (*p && *p != ' ') p++;
            if (*p == ' ') {
                *p = '\0';
                p++;
            }
            while (*p == ' ') p++;
            char *label = p;
            while (*p && *p != ' ') p++;
            if (*p == ' ') {
                *p = '\0';
                p++;
            }
            while (*p == ' ') p++;
            char *note = p;
            long long tval = strtoll(time_str, NULL, 10);
            if (count >= cap) {
                cap = cap == 0 ? 4 : cap * 2;
                Activity *nl = realloc(log, cap * sizeof(Activity));
                if (nl) log = nl;
            }
            char *nlab = mystrdup(label);
            char *nnote = mystrdup(note);
            if (nlab && nnote) {
                log[count].time = tval;
                log[count].label = nlab;
                log[count].note = nnote;
                log[count].id = seq++;
                count++;
            } else {
                free(nlab);
                free(nnote);
            }
        } else if (strcmp(cmd, "DELETE") == 0) {
            while (*p == ' ') p++;
            char *time_str = p;
            while (*p && *p != ' ') p++;
            if (*p == ' ') {
                *p = '\0';
                p++;
            }
            while (*p == ' ') p++;
            char *label = p;
            while (*p && *p != ' ') p++;
            if (*p == ' ') {
                *p = '\0';
                p++;
            }
            long long tval = strtoll(time_str, NULL, 10);
            size_t i = 0;
            while (i < count) {
                if (log[i].time == tval && strcmp(log[i].label, label) == 0) {
                    free(log[i].label);
                    free(log[i].note);
                    for (size_t j = i; j < count - 1; j++) {
                        log[j] = log[j + 1];
                    }
                    count--;
                } else {
                    i++;
                }
            }
        } else if (strcmp(cmd, "FIND") == 0) {
            while (*p == ' ') p++;
            char *label = p;
            while (*p && *p != ' ') p++;
            if (*p == ' ') {
                *p = '\0';
                p++;
            }
            int best_idx = -1;
            for (size_t i = 0; i < count; i++) {
                if (strcmp(log[i].label, label) == 0) {
                    if (best_idx == -1 || log[i].time < log[best_idx].time) {
                        best_idx = (int)i;
                    }
                }
            }
            if (best_idx != -1) {
                printf("%lld %s %s\n", log[best_idx].time, log[best_idx].label, log[best_idx].note);
            }
        } else if (strcmp(cmd, "SORT") == 0) {
            if (count > 0) {
                Activity *copy = malloc(count * sizeof(Activity));
                if (copy) {
                    memcpy(copy, log, count * sizeof(Activity));
                    qsort(copy, count, sizeof(Activity), compare_activities);
                    for (size_t i = 0; i < count; i++) {
                        printf("%lld %s %s\n", copy[i].time, copy[i].label, copy[i].note);
                    }
                    free(copy);
                }
            }
        } else if (strcmp(cmd, "CLEAR") == 0) {
            for (size_t i = 0; i < count; i++) {
                free(log[i].label);
                free(log[i].note);
            }
            count = 0;
        }
        free(line);
    }
    for (size_t i = 0; i < count; i++) {
        free(log[i].label);
        free(log[i].note);
    }
    free(log);
    return 0;
}
