// F007.c
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
        buf[len - 1] = '\0';
    } else {
        buf[len] = '\0';
    }
    return buf;
}

char *safe_dup(const char *s) {
    size_t l = strlen(s);
    char *d = malloc(l + 1);
    if (d) {
        strcpy(d, s);
    }
    return d;
}

typedef struct {
    long long time;
    char *label;
    char *note;
} Activity;

int compare_activities(const void *a, const void *b) {
    const Activity *aa = (const Activity *)a;
    const Activity *bb = (const Activity *)b;
    if (aa->time < bb->time) return -1;
    if (aa->time > bb->time) return 1;
    return strcmp(aa->label, bb->label);
}

int main(void) {
    Activity *log = NULL;
    size_t count = 0;
    size_t cap = 0;
    while (1) {
        char *line = read_line(stdin);
        if (!line) break;
        if (strcmp(line, "END") == 0) {
            free(line);
            break;
        }
        if (strncmp(line, "ADD ", 4) == 0) {
            char *p = line + 4;
            char *time_str = p;
            while (*p && *p != ' ') p++;
            if (*p == ' ') {
                *p = '\0';
                p++;
            }
            char *label = p;
            while (*p && *p != ' ') p++;
            char *note = "";
            if (*p == ' ') {
                *p = '\0';
                note = p + 1;
            }
            long long t_val = strtoll(time_str, NULL, 10);
            if (count >= cap) {
                cap = cap == 0 ? 4 : cap * 2;
                Activity *nb = realloc(log, cap * sizeof(Activity));
                if (!nb) {
                    free(line);
                    break;
                }
                log = nb;
            }
            log[count].time = t_val;
            log[count].label = safe_dup(label);
            log[count].note = safe_dup(note);
            count++;
        } else if (strncmp(line, "DELETE ", 7) == 0) {
            char *p = line + 7;
            char *time_str = p;
            while (*p && *p != ' ') p++;
            char *label = "";
            if (*p == ' ') {
                *p = '\0';
                label = p + 1;
            }
            long long t_val = strtoll(time_str, NULL, 10);
            size_t i = 0;
            while (i < count) {
                if (log[i].time == t_val && strcmp(log[i].label, label) == 0) {
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
        } else if (strncmp(line, "FIND ", 5) == 0) {
            char *label = line + 5;
            size_t best = (size_t)-1;
            for (size_t i = 0; i < count; i++) {
                if (strcmp(log[i].label, label) == 0) {
                    if (best == (size_t)-1 || log[i].time < log[best].time) {
                        best = i;
                    }
                }
            }
            if (best != (size_t)-1) {
                printf("%lld %s %s\n", log[best].time, log[best].label, log[best].note);
            }
        } else if (strcmp(line, "SORT") == 0) {
            qsort(log, count, sizeof(Activity), compare_activities);
            for (size_t i = 0; i < count; i++) {
                printf("%lld %s %s\n", log[i].time, log[i].label, log[i].note);
            }
        } else if (strcmp(line, "CLEAR") == 0) {
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
