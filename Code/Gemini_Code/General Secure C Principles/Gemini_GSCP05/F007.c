// F007.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *read_line(FILE *stream) {
    size_t capacity = 16;
    size_t len = 0;
    char *str = malloc(capacity);
    if (!str) return NULL;
    int c;
    while ((c = fgetc(stream)) != EOF && c != '\n') {
        if (len + 1 >= capacity) {
            capacity *= 2;
            char *new_str = realloc(str, capacity);
            if (!new_str) {
                free(str);
                return NULL;
            }
            str = new_str;
        }
        str[len++] = (char)c;
    }
    if (len == 0 && c == EOF) {
        free(str);
        return NULL;
    }
    if (len > 0 && str[len - 1] == '\n') {
        len--;
    }
    str[len] = '\0';
    return str;
}

typedef struct {
    long long time;
    char *label;
    char *note;
} Activity;

int compare_activity(const void *a, const void *b) {
    const Activity *actA = (const Activity *)a;
    const Activity *actB = (const Activity *)b;
    if (actA->time < actB->time) return -1;
    if (actA->time > actB->time) return 1;
    return strcmp(actA->label, actB->label);
}

int main(void) {
    Activity *log = NULL;
    int count = 0;
    int capacity = 0;
    while (1) {
        char *line = read_line(stdin);
        if (!line) break;
        if (strcmp(line, "END") == 0) {
            free(line);
            break;
        }
        if (strncmp(line, "ADD ", 4) == 0) {
            char *p = line + 4;
            while (*p == ' ') p++;
            char *endptr;
            long long t_val = strtoll(p, &endptr, 10);
            if (endptr != p) {
                p = endptr;
                while (*p == ' ') p++;
                char *label_start = p;
                while (*p && *p != ' ') p++;
                if (*p == ' ') {
                    *p = '\0';
                    char *note_start = p + 1;
                    while (*note_start == ' ') note_start++;
                    if (count >= capacity) {
                        capacity = capacity == 0 ? 4 : capacity * 2;
                        Activity *new_log = realloc(log, capacity * sizeof(Activity));
                        if (new_log) log = new_log;
                    }
                    if (count < capacity) {
                        log[count].time = t_val;
                        log[count].label = strdup(label_start);
                        log[count].note = strdup(note_start);
                        count++;
                    }
                }
            }
        } else if (strncmp(line, "DELETE ", 7) == 0) {
            char *p = line + 7;
            while (*p == ' ') p++;
            char *endptr;
            long long t_val = strtoll(p, &endptr, 10);
            if (endptr != p) {
                p = endptr;
                while (*p == ' ') p++;
                char *label_start = p;
                int i = 0;
                while (i < count) {
                    if (log[i].time == t_val && strcmp(log[i].label, label_start) == 0) {
                        free(log[i].label);
                        free(log[i].note);
                        for (int j = i; j < count - 1; j++) {
                            log[j] = log[j + 1];
                        }
                        count--;
                    } else {
                        i++;
                    }
                }
            }
        } else if (strncmp(line, "FIND ", 5) == 0) {
            char *label_start = line + 5;
            while (*label_start == ' ') label_start++;
            int found_idx = -1;
            for (int i = 0; i < count; i++) {
                if (strcmp(log[i].label, label_start) == 0) {
                    if (found_idx == -1 || log[i].time < log[found_idx].time) {
                        found_idx = i;
                    }
                }
            }
            if (found_idx != -1) {
                printf("%lld %s %s\n", log[found_idx].time, log[found_idx].label, log[found_idx].note);
            }
        } else if (strcmp(line, "SORT") == 0) {
            if (count > 0) {
                qsort(log, count, sizeof(Activity), compare_activity);
            }
            for (int i = 0; i < count; i++) {
                printf("%lld %s %s\n", log[i].time, log[i].label, log[i].note);
            }
        } else if (strcmp(line, "CLEAR") == 0) {
            for (int i = 0; i < count; i++) {
                free(log[i].label);
                free(log[i].note);
            }
            free(log);
            log = NULL;
            count = 0;
            capacity = 0;
        }
        free(line);
    }
    for (int i = 0; i < count; i++) {
        free(log[i].label);
        free(log[i].note);
    }
    free(log);
    return 0;
}
