// F027.c
#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    int priority;
    char *title;
    char *note;
} Reminder;

Reminder *reminders = NULL;
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
                    char *title = space2 + 1;
                    char *space3 = strchr(title, ' ');
                    if (space3) {
                        *space3 = '\0';
                        char *note = space3 + 1;
                        if (count >= capacity) {
                            capacity = capacity == 0 ? 4 : capacity * 2;
                            Reminder *tmp = realloc(reminders, capacity * sizeof(Reminder));
                            if (!tmp) {
                                free(line);
                                return 1;
                            }
                            reminders = tmp;
                        }
                        reminders[count].priority = atoi(arg1);
                        reminders[count].title = strdup(title);
                        reminders[count].note = strdup(note);
                        if (reminders[count].title && reminders[count].note) {
                            count++;
                        }
                    }
                }
            } else if (strcmp(cmd, "BUMP") == 0) {
                char *space2 = strchr(arg1, ' ');
                if (space2) {
                    *space2 = '\0';
                    int delta = atoi(space2 + 1);
                    for (int i = 0; i < count; i++) {
                        if (strcmp(reminders[i].title, arg1) == 0) {
                            reminders[i].priority += delta;
                        }
                    }
                }
            } else if (strcmp(cmd, "REMOVE") == 0) {
                for (int i = 0; i < count; ) {
                    if (strcmp(reminders[i].title, arg1) == 0) {
                        free(reminders[i].title);
                        free(reminders[i].note);
                        for (int j = i; j < count - 1; j++) {
                            reminders[j] = reminders[j + 1];
                        }
                        count--;
                    } else {
                        i++;
                    }
                }
            } else if (strcmp(cmd, "TOP") == 0) {
                int n = atoi(arg1);
                if (n > 0 && count > 0) {
                    Reminder *copy = malloc(count * sizeof(Reminder));
                    if (copy) {
                        for (int i = 0; i < count; i++) {
                            copy[i] = reminders[i];
                        }
                        for (int i = 0; i < count - 1; i++) {
                            for (int j = 0; j < count - i - 1; j++) {
                                if (copy[j].priority < copy[j + 1].priority) {
                                    Reminder tmp = copy[j];
                                    copy[j] = copy[j + 1];
                                    copy[j + 1] = tmp;
                                }
                            }
                        }
                        int limit = n < count ? n : count;
                        for (int i = 0; i < limit; i++) {
                            printf("%d %s %s\n", copy[i].priority, copy[i].title, copy[i].note);
                        }
                        free(copy);
                    }
                }
            }
        } else {
            if (strcmp(cmd, "LIST") == 0) {
                for (int i = 0; i < count; i++) {
                    printf("%d %s %s\n", reminders[i].priority, reminders[i].title, reminders[i].note);
                }
            } else if (strcmp(cmd, "END") == 0) {
                break;
            }
        }
    }
    free(line);
    for (int i = 0; i < count; i++) {
        free(reminders[i].title);
        free(reminders[i].note);
    }
    free(reminders);
    return 0;
}