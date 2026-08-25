// F027.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Reminder {
    int priority;
    char *title;
    char *note;
    struct Reminder *next;
} Reminder;

Reminder *reminders = NULL;

void add_reminder(int priority, const char *title, const char *note) {
    Reminder *r = malloc(sizeof(Reminder));
    if (!r) exit(EXIT_FAILURE);
    r->priority = priority;
    r->title = strdup(title);
    r->note = strdup(note);
    r->next = reminders;
    reminders = r;
}

void bump_reminder(const char *title, int delta) {
    Reminder *curr = reminders;
    while (curr) {
        if (strcmp(curr->title, title) == 0) {
            curr->priority += delta;
            return;
        }
        curr = curr->next;
    }
}

void remove_reminder(const char *title) {
    Reminder *curr = reminders;
    Reminder *prev = NULL;
    while (curr) {
        if (strcmp(curr->title, title) == 0) {
            if (prev) prev->next = curr->next;
            else reminders = curr->next;
            free(curr->title);
            free(curr->note);
            free(curr);
            return;
        }
        prev = curr;
        curr = curr->next;
    }
}

int cmp_reminders(const void *a, const void *b) {
    const Reminder *ra = *(const Reminder **)a;
    const Reminder *rb = *(const Reminder **)b;
    if (ra->priority != rb->priority) return (rb->priority - ra->priority);
    return strcmp(ra->title, rb->title);
}

void top_reminders(int n) {
    int count = 0;
    Reminder *curr = reminders;
    while (curr) {
        count++;
        curr = curr->next;
    }
    if (count == 0) return;
    
    Reminder **arr = malloc(count * sizeof(Reminder *));
    if (!arr) exit(EXIT_FAILURE);
    
    curr = reminders;
    for (int i = 0; i < count; i++) {
        arr[i] = curr;
        curr = curr->next;
    }
    
    qsort(arr, count, sizeof(Reminder *), cmp_reminders);
    
    int limit = n < count ? n : count;
    for (int i = 0; i < limit; i++) {
        printf("%d %s %s\n", arr[i]->priority, arr[i]->title, arr[i]->note);
    }
    free(arr);
}

void list_reminders() {
    Reminder *curr = reminders;
    while (curr) {
        printf("%d %s %s\n", curr->priority, curr->title, curr->note);
        curr = curr->next;
    }
}

void free_all() {
    Reminder *curr = reminders;
    while (curr) {
        Reminder *next = curr->next;
        free(curr->title);
        free(curr->note);
        free(curr);
        curr = next;
    }
}

int main() {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        if (strcmp(line, "END") == 0) break;
        
        char cmd[32] = {0};
        char arg1[256] = {0};
        char arg2[256] = {0};
        char arg3[3500] = {0};
        int val = 0;
        
        int matched = sscanf(line, "%31s %255s %255s %[^\n]", cmd, arg1, arg2, arg3);
        if (matched >= 2) {
            sscanf(arg2, "%d", &val);
        }
        
        if (strcmp(cmd, "ADD") == 0 && matched >= 3) {
            int priority = 0;
            sscanf(arg1, "%d", &priority);
            add_reminder(priority, arg2, arg3);
        } else if (strcmp(cmd, "BUMP") == 0 && matched >= 2) {
            bump_reminder(arg1, val);
        } else if (strcmp(cmd, "REMOVE") == 0) {
            remove_reminder(arg1);
        } else if (strcmp(cmd, "TOP") == 0) {
            top_reminders(val);
        } else if (strcmp(cmd, "LIST") == 0) {
            list_reminders();
        }
    }
    free_all();
    return 0;
}