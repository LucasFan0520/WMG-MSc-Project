// F007.c
#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    int time;
    char *label;
    char *note;
} Activity;

Activity *log_entries = NULL;
int count = 0;
int capacity = 0;

int main(void) {
    char *line = NULL;
    size_t len = 0;
    ssize_t nread;
    while ((nread = getline(&line, &len, stdin)) != -1) {
        size_t l = strlen(line);
        if (l > 0 && line[l - 1] == '\n') {
            line[l - 1] = '\0';
        }
        char *cmd = line;
        char *space1 = strchr(cmd, ' ');
        if (space1) {
            *space1 = '\0';
            char *arg1 = space1 + 1;
            if (strcmp(cmd, "ADD") == 0) {
                char *space2 = strchr(arg1, ' ');
                if (space2) {
                    *space2 = '\0';
                    char *label = space2 + 1;
                    char *space3 = strchr(label, ' ');
                    if (space3) {
                        *space3 = '\0';
                        char *note = space3 + 1;
                        if (count >= capacity) {
                            capacity = capacity == 0 ? 4 : capacity * 2;
                            Activity *tmp = realloc(log_entries, capacity * sizeof(Activity));
                            if (!tmp) {
                                free(line);
                                return 1;
                            }
                            log_entries = tmp;
                        }
                        log_entries[count].time = atoi(arg1);
                        log_entries[count].label = strdup(label);
                        log_entries[count].note = strdup(note);
                        if (log_entries[count].label && log_entries[count].note) {
                            count++;
                        }
                    }
                }
            } else if (strcmp(cmd, "DELETE") == 0) {
                char *space2 = strchr(arg1, ' ');
                if (space2) {
                    *space2 = '\0';
                    int t = atoi(arg1);
                    char *label = space2 + 1;
                    for (int i = 0; i < count; ) {
                        if (log_entries[i].time == t && strcmp(log_entries[i].label, label) == 0) {
                            free(log_entries[i].label);
                            free(log_entries[i].note);
                            for (int j = i; j < count - 1; j++) {
                                log_entries[j] = log_entries[j + 1];
                            }
                            count--;
                        } else {
                            i++;
                        }
                    }
                }
            } else if (strcmp(cmd, "FIND") == 0) {
                int earliest_idx = -1;
                for (int i = 0; i < count; i++) {
                    if (strcmp(log_entries[i].label, arg1) == 0) {
                        if (earliest_idx == -1 || log_entries[i].time < log_entries[earliest_idx].time) {
                            earliest_idx = i;
                        }
                    }
                }
                if (earliest_idx != -1) {
                    printf("%d %s %s\n", log_entries[earliest_idx].time, log_entries[earliest_idx].label, log_entries[earliest_idx].note);
                }
            }
        } else {
            if (strcmp(cmd, "SORT") == 0) {
                for (int i = 0; i < count - 1; i++) {
                    for (int j = 0; j < count - i - 1; j++) {
                        int cmp = 0;
                        if (log_entries[j].time > log_entries[j + 1].time) {
                            cmp = 1;
                        } else if (log_entries[j].time == log_entries[j + 1].time) {
                            cmp = strcmp(log_entries[j].label, log_entries[j + 1].label);
                        }
                        if (cmp > 0) {
                            Activity tmp = log_entries[j];
                            log_entries[j] = log_entries[j + 1];
                            log_entries[j + 1] = tmp;
                        }
                    }
                }
                for (int i = 0; i < count; i++) {
                    printf("%d %s %s\n", log_entries[i].time, log_entries[i].label, log_entries[i].note);
                }
            } else if (strcmp(cmd, "CLEAR") == 0) {
                for (int i = 0; i < count; i++) {
                    free(log_entries[i].label);
                    free(log_entries[i].note);
                }
                count = 0;
            } else if (strcmp(cmd, "END") == 0) {
                break;
            }
        }
    }
    free(line);
    for (int i = 0; i < count; i++) {
        free(log_entries[i].label);
        free(log_entries[i].note);
    }
    free(log_entries);
    return 0;
}