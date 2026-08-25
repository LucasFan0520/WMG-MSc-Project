// F007.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *read_line(FILE *fp) {
    size_t size = 1024;
    size_t len = 0;
    char *buf = malloc(size);
    if (!buf) return NULL;
    int c;
    while ((c = fgetc(fp)) != EOF && c != '\n') {
        if (len + 1 >= size) {
            size *= 2;
            char *new_buf = realloc(buf, size);
            if (!new_buf) { free(buf); return NULL; }
            buf = new_buf;
        }
        buf[len++] = c;
    }
    if (len == 0 && c == EOF) { free(buf); return NULL; }
    buf[len] = '\0';
    return buf;
}

typedef struct {
    int time;
    char *label;
    char *note;
} Activity;

int compare_activities(const void *a, const void *b) {
    Activity *actA = (Activity *)a;
    Activity *actB = (Activity *)b;
    if (actA->time != actB->time) {
        return actA->time - actB->time;
    }
    return strcmp(actA->label, actB->label);
}

int main() {
    Activity *log = NULL;
    int count = 0;
    int capacity = 0;
    while (1) {
        char *line = read_line(stdin);
        if (!line) break;
        char cmd[64];
        int n = 0;
        if (sscanf(line, "%63s%n", cmd, &n) < 1) {
            free(line);
            continue;
        }
        if (strcmp(cmd, "END") == 0) {
            free(line);
            break;
        }
        if (strcmp(cmd, "CLEAR") == 0) {
            for (int i = 0; i < count; i++) {
                free(log[i].label);
                free(log[i].note);
            }
            count = 0;
            free(line);
            continue;
        }
        if (strcmp(cmd, "SORT") == 0) {
            Activity *temp = malloc(count * sizeof(Activity));
            memcpy(temp, log, count * sizeof(Activity));
            qsort(temp, count, sizeof(Activity), compare_activities);
            for (int i = 0; i < count; i++) {
                printf("%d %s %s\n", temp[i].time, temp[i].label, temp[i].note);
            }
            free(temp);
            free(line);
            continue;
        }
        char *p = line + n;
        while (*p == ' ' || *p == '\t') p++;
        if (strcmp(cmd, "ADD") == 0) {
            int tm = 0;
            int n2 = 0;
            if (sscanf(p, "%d%n", &tm, &n2) == 1) {
                p += n2;
                while (*p == ' ' || *p == '\t') p++;
                char *lbl_start = p;
                while (*p && *p != ' ' && *p != '\t') p++;
                int lbl_len = p - lbl_start;
                char *label = malloc(lbl_len + 1);
                memcpy(label, lbl_start, lbl_len);
                label[lbl_len] = '\0';
                while (*p == ' ' || *p == '\t') p++;
                char *note = p;
                if (count >= capacity) {
                    capacity = capacity == 0 ? 4 : capacity * 2;
                    log = realloc(log, capacity * sizeof(Activity));
                }
                log[count].time = tm;
                log[count].label = label;
                log[count].note = strdup(note);
                count++;
            }
        } else if (strcmp(cmd, "DELETE") == 0) {
            int tm = 0;
            char lbl[256];
            if (sscanf(p, "%d %255s", &tm, lbl) == 2) {
                int write_idx = 0;
                for (int i = 0; i < count; i++) {
                    if (log[i].time == tm && strcmp(log[i].label, lbl) == 0) {
                        free(log[i].label);
                        free(log[i].note);
                    } else {
                        log[write_idx++] = log[i];
                    }
                }
                count = write_idx;
            }
        } else if (strcmp(cmd, "FIND") == 0) {
            char lbl[256];
            if (sscanf(p, "%255s", lbl) == 1) {
                int earliest_idx = -1;
                for (int i = 0; i < count; i++) {
                    if (strcmp(log[i].label, lbl) == 0) {
                        if (earliest_idx == -1 || log[i].time < log[earliest_idx].time) {
                            earliest_idx = i;
                        }
                    }
                }
                if (earliest_idx != -1) {
                    printf("%d %s %s\n", log[earliest_idx].time, log[earliest_idx].label, log[earliest_idx].note);
                }
            }
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
