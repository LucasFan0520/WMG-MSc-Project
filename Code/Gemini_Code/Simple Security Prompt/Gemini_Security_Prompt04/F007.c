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
    buf[len] = '\0';
    if (len > 0 && buf[len-1] == '\r') {
        buf[len-1] = '\0';
    }
    return buf;
}

typedef struct {
    long time;
    char *label;
    char *note;
} Activity;

int main(void) {
    Activity *activities = NULL;
    int count = 0;
    int cap = 0;
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
            long t = strtol(p, &endptr, 10);
            p = endptr;
            while (*p == ' ') p++;
            char *label = p;
            while (*p && *p != ' ') p++;
            if (*p == ' ') {
                *p = '\0';
                char *note = p + 1;
                while (*note == ' ') note++;
                if (count >= cap) {
                    cap = cap == 0 ? 16 : cap * 2;
                    activities = realloc(activities, cap * sizeof(Activity));
                }
                activities[count].time = t;
                activities[count].label = strdup(label);
                activities[count].note = strdup(note);
                count++;
            }
        } else if (strncmp(line, "DELETE ", 7) == 0) {
            char *p = line + 7;
            while (*p == ' ') p++;
            char *endptr;
            long t = strtol(p, &endptr, 10);
            p = endptr;
            while (*p == ' ') p++;
            char *label = p;
            int i = 0;
            while (i < count) {
                if (activities[i].time == t && strcmp(activities[i].label, label) == 0) {
                    free(activities[i].label);
                    free(activities[i].note);
                    for (int j = i; j < count - 1; j++) {
                        activities[j] = activities[j + 1];
                    }
                    count--;
                } else {
                    i++;
                }
            }
        } else if (strncmp(line, "FIND ", 5) == 0) {
            char *label = line + 5;
            while (*label == ' ') label++;
            int min_idx = -1;
            for (int i = 0; i < count; i++) {
                if (strcmp(activities[i].label, label) == 0) {
                    if (min_idx == -1 || activities[i].time < activities[min_idx].time) {
                        min_idx = i;
                    }
                }
            }
            if (min_idx != -1) {
                printf("%ld %s %s\n", activities[min_idx].time, activities[min_idx].label, activities[min_idx].note);
            }
        } else if (strcmp(line, "SORT") == 0) {
            for (int i = 0; i < count - 1; i++) {
                for (int j = i + 1; j < count; j++) {
                    int swap = 0;
                    if (activities[i].time > activities[j].time) {
                        swap = 1;
                    } else if (activities[i].time == activities[j].time) {
                        if (strcmp(activities[i].label, activities[j].label) > 0) {
                            swap = 1;
                        }
                    }
                    if (swap) {
                        Activity tmp = activities[i];
                        activities[i] = activities[j];
                        activities[j] = tmp;
                    }
                }
            }
            for (int i = 0; i < count; i++) {
                printf("%ld %s %s\n", activities[i].time, activities[i].label, activities[i].note);
            }
        } else if (strcmp(line, "CLEAR") == 0) {
            for (int i = 0; i < count; i++) {
                free(activities[i].label);
                free(activities[i].note);
            }
            count = 0;
        }
        free(line);
    }
    for (int i = 0; i < count; i++) {
        free(activities[i].label);
        free(activities[i].note);
    }
    free(activities);
    return 0;
}
