// F007.c
#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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
    int count = 0;
    int capacity = 0;
    char *line = NULL;
    size_t len = 0;
    ssize_t read;

    while ((read = getline(&line, &len, stdin)) != -1) {
        if (read > 0 && line[read - 1] == '\n') {
            line[read - 1] = '\0';
        }
        if (strcmp(line, "END") == 0) {
            break;
        }
        if (strncmp(line, "ADD ", 4) == 0) {
            char *p = line + 4;
            while (*p == ' ') p++;
            char *endptr;
            long long t = strtoll(p, &endptr, 10);
            while (*endptr == ' ') endptr++;
            char *label = endptr;
            char *space = strchr(label, ' ');
            char *note = "";
            if (space) {
                *space = '\0';
                note = space + 1;
                while (*note == ' ') note++;
            }
            if (count >= capacity) {
                int new_cap = capacity == 0 ? 4 : capacity * 2;
                Activity *tmp = realloc(log, new_cap * sizeof(Activity));
                if (!tmp) {
                    free(line);
                    for(int i=0; i<count; i++) { free(log[i].label); free(log[i].note); }
                    free(log);
                    return 1;
                }
                log = tmp;
                capacity = new_cap;
            }
            char *nl = strdup(label);
            char *nn = strdup(note);
            if (!nl || !nn) {
                free(nl); free(nn); free(line);
                for(int i=0; i<count; i++) { free(log[i].label); free(log[i].note); }
                free(log);
                return 1;
            }
            log[count].time = t;
            log[count].label = nl;
            log[count].note = nn;
            count++;
        } else if (strncmp(line, "DELETE ", 7) == 0) {
            char *p = line + 7;
            while (*p == ' ') p++;
            char *endptr;
            long long t = strtoll(p, &endptr, 10);
            while (*endptr == ' ') endptr++;
            char *label = endptr;
            int w = 0;
            for (int i = 0; i < count; i++) {
                if (log[i].time == t && strcmp(log[i].label, label) == 0) {
                    free(log[i].label);
                    free(log[i].note);
                } else {
                    log[w++] = log[i];
                }
            }
            count = w;
        } else if (strncmp(line, "FIND ", 5) == 0) {
            char *label = line + 5;
            while (*label == ' ') label++;
            int earliest = -1;
            for (int i = 0; i < count; i++) {
                if (strcmp(log[i].label, label) == 0) {
                    if (earliest == -1 || log[i].time < log[earliest].time) {
                        earliest = i;
                    }
                }
            }
            if (earliest != -1) {
                printf("%lld %s %s\n", log[earliest].time, log[earliest].label, log[earliest].note);
            } else {
                printf("NOT_FOUND\n");
            }
        } else if (strcmp(line, "SORT") == 0) {
            if (count > 0) {
                qsort(log, count, sizeof(Activity), compare_activities);
            }
            for (int i = 0; i < count; i++) {
                printf("%lld %s %s\n", log[i].time, log[i].label, log[i].note);
            }
        } else if (strcmp(line, "CLEAR") == 0) {
            for (int i = 0; i < count; i++) {
                free(log[i].label);
                free(log[i].note);
            }
            count = 0;
        }
    }
    free(line);
    for (int i = 0; i < count; i++) {
        free(log[i].label);
        free(log[i].note);
    }
    log = NULL;
    free(log);
    return 0;
}
