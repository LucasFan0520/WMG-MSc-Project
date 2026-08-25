// F007.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *read_line(FILE *stream) {
    size_t size = 16;
    size_t len = 0;
    char *buf = malloc(size);
    if (!buf) return NULL;
    int c;
    while ((c = fgetc(stream)) != EOF && c != '\n') {
        if (len + 1 >= size) {
            size *= 2;
            char *new_buf = realloc(buf, size);
            if (!new_buf) {
                free(buf);
                return NULL;
            }
            buf = new_buf;
        }
        buf[len++] = c;
    }
    if (len == 0 && c == EOF) {
        free(buf);
        return NULL;
    }
    buf[len] = '\0';
    if (len > 0 && buf[len - 1] == '\r') {
        buf[len - 1] = '\0';
    }
    return buf;
}

typedef struct {
    char *time;
    char *label;
    char *note;
} Activity;

int compare_activity(const void *a, const void *b) {
    const Activity *actA = (const Activity *)a;
    const Activity *actB = (const Activity *)b;
    int cmp_time = strcmp(actA->time, actB->time);
    if (cmp_time != 0) return cmp_time;
    return strcmp(actA->label, actB->label);
}

int main(void) {
    Activity *log = NULL;
    size_t count = 0;
    size_t capacity = 0;
    while (1) {
        char *line = read_line(stdin);
        if (!line) break;
        char *p = line;
        while (*p == ' ' || *p == '\t') p++;
        if (*p == '\0') {
            free(line);
            continue;
        }
        char *cmd = p;
        while (*p && *p != ' ' && *p != '\t') p++;
        if (*p) {
            *p = '\0';
            p++;
        }
        if (strcmp(cmd, "END") == 0) {
            free(line);
            break;
        }
        if (strcmp(cmd, "ADD") == 0) {
            while (*p == ' ' || *p == '\t') p++;
            char *tm = p;
            while (*p && *p != ' ' && *p != '\t') p++;
            if (*p) {
                *p = '\0';
                p++;
            }
            while (*p == ' ' || *p == '\t') p++;
            char *lbl = p;
            while (*p && *p != ' ' && *p != '\t') p++;
            if (*p) {
                *p = '\0';
                p++;
            }
            while (*p == ' ' || *p == '\t') p++;
            char *nt = p;
            if (*tm != '\0' && *lbl != '\0') {
                if (count >= capacity) {
                    capacity = capacity == 0 ? 4 : capacity * 2;
                    Activity *new_log = realloc(log, capacity * sizeof(Activity));
                    if (new_log) log = new_log;
                }
                if (count < capacity) {
                    log[count].time = strdup(tm);
                    log[count].label = strdup(lbl);
                    log[count].note = strdup(nt);
                    count++;
                }
            }
        } else if (strcmp(cmd, "DELETE") == 0) {
            while (*p == ' ' || *p == '\t') p++;
            char *tm = p;
            while (*p && *p != ' ' && *p != '\t') p++;
            if (*p) {
                *p = '\0';
                p++;
            }
            while (*p == ' ' || *p == '\t') p++;
            char *lbl = p;
            while (*p && *p != ' ' && *p != '\t') p++;
            if (*p) *p = '\0';
            size_t idx = 0;
            while (idx < count) {
                if (strcmp(log[idx].time, tm) == 0 && strcmp(log[idx].label, lbl) == 0) {
                    free(log[idx].time);
                    free(log[idx].label);
                    free(log[idx].note);
                    for (size_t j = idx; j < count - 1; j++) {
                        log[j] = log[j + 1];
                    }
                    count--;
                } else {
                    idx++;
                }
            }
        } else if (strcmp(cmd, "FIND") == 0) {
            while (*p == ' ' || *p == '\t') p++;
            char *lbl = p;
            while (*p && *p != ' ' && *p != '\t') p++;
            if (*p) *p = '\0';
            long long best_idx = -1;
            for (size_t i = 0; i < count; i++) {
                if (strcmp(log[i].label, lbl) == 0) {
                    if (best_idx == -1 || compare_activity(&log[i], &log[best_idx]) < 0) {
                        best_idx = (long long)i;
                    }
                }
            }
            if (best_idx != -1) {
                printf("%s %s %s\n", log[best_idx].time, log[best_idx].label, log[best_idx].note);
            }
        } else if (strcmp(cmd, "SORT") == 0) {
            if (count > 0) {
                qsort(log, count, sizeof(Activity), compare_activity);
            }
            for (size_t i = 0; i < count; i++) {
                printf("%s %s %s\n", log[i].time, log[i].label, log[i].note);
            }
        } else if (strcmp(cmd, "CLEAR") == 0) {
            for (size_t i = 0; i < count; i++) {
                free(log[i].time);
                free(log[i].label);
                free(log[i].note);
            }
            count = 0;
        }
        free(line);
    }
    for (size_t i = 0; i < count; i++) {
        free(log[i].time);
        free(log[i].label);
        free(log[i].note);
    }
    free(log);
    return 0;
}
