// F027.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    int priority;
    char *title;
    char *note;
    int order;
} Reminder;

Reminder *reminders = NULL;
int num_reminders = 0;
int global_order = 0;

int find_reminder(const char *title) {
    for (int i = 0; i < num_reminders; i++) {
        if (strcmp(reminders[i].title, title) == 0) return i;
    }
    return -1;
}

int cmp_priority(const void *a, const void *b) {
    Reminder *ra = (Reminder*)a;
    Reminder *rb = (Reminder*)b;
    if (rb->priority != ra->priority) return rb->priority - ra->priority;
    return ra->order - rb->order;
}

int main() {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        if (strcmp(line, "END") == 0) break;
        
        char cmd[64];
        sscanf(line, "%s", cmd);
        
        if (strcmp(cmd, "ADD") == 0) {
            int prio;
            char title[256];
            sscanf(line, "%*s %d %s", &prio, title);
            char *note = strstr(line, title);
            if (note) {
                note += strlen(title);
                if (*note == ' ') note++;
            } else {
                note = "";
            }
            reminders = realloc(reminders, (num_reminders + 1) * sizeof(Reminder));
            reminders[num_reminders].priority = prio;
            reminders[num_reminders].title = strdup(title);
            reminders[num_reminders].note = strdup(note);
            reminders[num_reminders].order = global_order++;
            num_reminders++;
        } else if (strcmp(cmd, "BUMP") == 0) {
            char title[256];
            int delta;
            sscanf(line, "%*s %s %d", title, &delta);
            int idx = find_reminder(title);
            if (idx >= 0) {
                reminders[idx].priority += delta;
            }
        } else if (strcmp(cmd, "REMOVE") == 0) {
            char title[256];
            sscanf(line, "%*s %s", title);
            int idx = find_reminder(title);
            if (idx >= 0) {
                free(reminders[idx].title);
                free(reminders[idx].note);
                for (int i = idx; i < num_reminders - 1; i++) {
                    reminders[i] = reminders[i + 1];
                }
                num_reminders--;
            }
        } else if (strcmp(cmd, "TOP") == 0) {
            int n;
            sscanf(line, "%*s %d", &n);
            Reminder *sorted = malloc(num_reminders * sizeof(Reminder));
            memcpy(sorted, reminders, num_reminders * sizeof(Reminder));
            qsort(sorted, num_reminders, sizeof(Reminder), cmp_priority);
            int limit = n < num_reminders ? n : num_reminders;
            for (int i = 0; i < limit; i++) {
                printf("%d %s %s\n", sorted[i].priority, sorted[i].title, sorted[i].note);
            }
            free(sorted);
        } else if (strcmp(cmd, "LIST") == 0) {
            for (int i = 0; i < num_reminders; i++) {
                printf("%d %s %s\n", reminders[i].priority, reminders[i].title, reminders[i].note);
            }
        }
    }
    
    for (int i = 0; i < num_reminders; i++) {
        free(reminders[i].title);
        free(reminders[i].note);
    }
    free(reminders);
    
    return 0;
}