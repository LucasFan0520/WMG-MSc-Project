// F027.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    int priority;
    char *title;
    char *note;
} Reminder;

Reminder *reminders = NULL;
int r_count = 0;

int find_title(const char *title) {
    for (int i = 0; i < r_count; i++) {
        if (strcmp(reminders[i].title, title) == 0) {
            return i;
        }
    }
    return -1;
}

int cmp_priority(const void *a, const void *b) {
    return ((Reminder *)b)->priority - ((Reminder *)a)->priority;
}

int main() {
    char cmd[32];
    char title[1024];
    char note[10240];
    int priority;
    int delta;
    int n;

    while (scanf("%s", cmd) == 1 && strcmp(cmd, "END") != 0) {
        if (strcmp(cmd, "ADD") == 0) {
            scanf("%d %s", &priority, title);
            fgets(note, sizeof(note), stdin);
            if (note[0] == ' ') {
                memmove(note, note + 1, strlen(note));
            }
            note[strcspn(note, "\n")] = '\0';
            reminders = realloc(reminders, sizeof(Reminder) * (r_count + 1));
            reminders[r_count].priority = priority;
            reminders[r_count].title = strdup(title);
            reminders[r_count].note = strdup(note);
            r_count++;
        } else if (strcmp(cmd, "BUMP") == 0) {
            scanf("%s %d", title, &delta);
            int idx = find_title(title);
            if (idx != -1) {
                reminders[idx].priority += delta;
            }
        } else if (strcmp(cmd, "REMOVE") == 0) {
            scanf("%s", title);
            int idx = find_title(title);
            if (idx != -1) {
                free(reminders[idx].title);
                free(reminders[idx].note);
                for (int i = idx; i < r_count - 1; i++) {
                    reminders[i] = reminders[i + 1];
                }
                r_count--;
            }
        } else if (strcmp(cmd, "TOP") == 0) {
            scanf("%d", &n);
            Reminder *sorted = malloc(sizeof(Reminder) * r_count);
            for (int i = 0; i < r_count; i++) {
                sorted[i] = reminders[i];
            }
            qsort(sorted, r_count, sizeof(Reminder), cmp_priority);
            int limit = n < r_count ? n : r_count;
            for (int i = 0; i < limit; i++) {
                printf("%d %s %s\n", sorted[i].priority, sorted[i].title, sorted[i].note);
            }
            free(sorted);
        } else if (strcmp(cmd, "LIST") == 0) {
            for (int i = 0; i < r_count; i++) {
                printf("%d %s %s\n", reminders[i].priority, reminders[i].title, reminders[i].note);
            }
        }
    }

    for (int i = 0; i < r_count; i++) {
        free(reminders[i].title);
        free(reminders[i].note);
    }
    free(reminders);

    return 0;
}