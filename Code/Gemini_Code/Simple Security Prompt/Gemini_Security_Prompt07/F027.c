// F027.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    int priority;
    char *title;
    char *note;
    int insertion_id;
} Reminder;

int compare_reminders(const void *a, const void *b) {
    Reminder *rA = (Reminder *)a;
    Reminder *rB = (Reminder *)b;
    if (rB->priority != rA->priority) {
        return rB->priority - rA->priority;
    }
    return rA->insertion_id - rB->insertion_id;
}

int main() {
    Reminder *reminders = NULL;
    int count = 0;
    int total_inserted = 0;
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\r\n")] = 0;
        char *cmd = strtok(line, " ");
        if (!cmd) continue;
        if (strcmp(cmd, "END") == 0) break;
        if (strcmp(cmd, "ADD") == 0) {
            char *p_str = strtok(NULL, " ");
            char *title = strtok(NULL, " ");
            char *note = strtok(NULL, "");
            if (p_str && title && note) {
                reminders = realloc(reminders, (count + 1) * sizeof(Reminder));
                reminders[count].priority = atoi(p_str);
                reminders[count].title = strdup(title);
                reminders[count].note = strdup(note);
                reminders[count].insertion_id = total_inserted++;
                count++;
            }
        } else if (strcmp(cmd, "BUMP") == 0) {
            char *title = strtok(NULL, " ");
            char *d_str = strtok(NULL, " ");
            if (title && d_str) {
                int delta = atoi(d_str);
                for (int i = 0; i < count; i++) {
                    if (strcmp(reminders[i].title, title) == 0) {
                        reminders[i].priority += delta;
                        break;
                    }
                }
            }
        } else if (strcmp(cmd, "REMOVE") == 0) {
            char *title = strtok(NULL, " ");
            if (title) {
                for (int i = 0; i < count; i++) {
                    if (strcmp(reminders[i].title, title) == 0) {
                        free(reminders[i].title);
                        free(reminders[i].note);
                        for (int j = i; j < count - 1; j++) {
                            reminders[j] = reminders[j + 1];
                        }
                        count--;
                        i--;
                    }
                }
                if (count == 0) {
                    free(reminders);
                    reminders = NULL;
                } else {
                    reminders = realloc(reminders, count * sizeof(Reminder));
                }
            }
        } else if (strcmp(cmd, "TOP") == 0) {
            char *n_str = strtok(NULL, " ");
            if (n_str) {
                int n = atoi(n_str);
                if (count > 0) {
                    Reminder *temp = malloc(count * sizeof(Reminder));
                    memcpy(temp, reminders, count * sizeof(Reminder));
                    qsort(temp, count, sizeof(Reminder), compare_reminders);
                    int limit = (n < count) ? n : count;
                    for (int i = 0; i < limit; i++) {
                        printf("(%d) %s: %s\n", temp[i].priority, temp[i].title, temp[i].note);
                    }
                    free(temp);
                }
            }
        } else if (strcmp(cmd, "LIST") == 0) {
            for (int i = 0; i < count; i++) {
                printf("(%d) %s: %s\n", reminders[i].priority, reminders[i].title, reminders[i].note);
            }
        }
    }
    for (int i = 0; i < count; i++) {
        free(reminders[i].title);
        free(reminders[i].note);
    }
    free(reminders);
    return 0;
}
