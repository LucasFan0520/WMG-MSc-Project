// F007.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *read_line(FILE *fp) {
    size_t size = 128;
    size_t len = 0;
    char *buf = malloc(size);
    if (!buf) return NULL;
    int c;
    while ((c = fgetc(fp)) != EOF && c != '\n') {
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
    while (1) {
        char *line = read_line(stdin);
        if (!line) break;
        if (strcmp(line, "END") == 0) {
            free(line);
            break;
        }
        if (strncmp(line, "ADD ", 4) == 0) {
            int t;
            char lbl[128];
            if (sscanf(line + 4, "%d %127s", &t, lbl) == 2) {
                char *p = line + 4;
                while (*p && *p != ' ') p++;
                if (*p == ' ') p++;
                while (*p && *p != ' ') p++;
                if (*p == ' ') p++;
                log = realloc(log, (count + 1) * sizeof(Activity));
                log[count].time = t;
                log[count].label = strdup(lbl);
                log[count].note = strdup(p);
                count++;
            }
        } else if (strncmp(line, "DELETE ", 7) == 0) {
            int t;
            char lbl[128];
            if (sscanf(line + 7, "%d %127s", &t, lbl) == 2) {
                int i = 0;
                while (i < count) {
                    if (log[i].time == t && strcmp(log[i].label, lbl) == 0) {
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
                log = realloc(log, count * sizeof(Activity));
            }
        } else if (strncmp(line, "FIND ", 5) == 0) {
            char *lbl = line + 5;
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
        } else if (strcmp(line, "SORT") == 0) {
            Activity *temp = malloc(count * sizeof(Activity));
            memcpy(temp, log, count * sizeof(Activity));
            qsort(temp, count, sizeof(Activity), compare_activities);
            for (int i = 0; i < count; i++) {
                printf("%d %s %s\n", temp[i].time, temp[i].label, temp[i].note);
            }
            free(temp);
        } else if (strcmp(line, "CLEAR") == 0) {
            for (int i = 0; i < count; i++) {
                free(log[i].label);
                free(log[i].note);
            }
            free(log);
            log = NULL;
            count = 0;
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
