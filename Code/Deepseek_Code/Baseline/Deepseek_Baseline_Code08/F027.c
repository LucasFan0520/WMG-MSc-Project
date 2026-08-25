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

Reminder *head = NULL;

Reminder* find_reminder(const char *title) {
    Reminder *cur = head;
    while (cur) {
        if (strcmp(cur->title, title) == 0) return cur;
        cur = cur->next;
    }
    return NULL;
}

void add_reminder(int priority, const char *title, const char *note) {
    if (find_reminder(title)) return;
    Reminder *new = malloc(sizeof(Reminder));
    new->priority = priority;
    new->title = strdup(title);
    new->note = strdup(note);
    new->next = NULL;
    if (!head) head = new;
    else {
        Reminder *cur = head;
        while (cur->next) cur = cur->next;
        cur->next = new;
    }
}

void bump_priority(const char *title, int delta) {
    Reminder *r = find_reminder(title);
    if (!r) return;
    r->priority += delta;
}

void remove_reminder(const char *title) {
    Reminder *prev = NULL, *cur = head;
    while (cur) {
        if (strcmp(cur->title, title) == 0) {
            if (prev) prev->next = cur->next;
            else head = cur->next;
            free(cur->title);
            free(cur->note);
            free(cur);
            return;
        }
        prev = cur;
        cur = cur->next;
    }
}

void top_n(int n) {
    // Collect all into array to sort by priority descending
    int count = 0;
    Reminder *cur = head;
    while (cur) { count++; cur = cur->next; }
    if (count == 0) return;
    Reminder **arr = malloc(count * sizeof(Reminder*));
    cur = head;
    for (int i = 0; i < count; i++) {
        arr[i] = cur;
        cur = cur->next;
    }
    // Simple bubble sort descending by priority
    for (int i = 0; i < count - 1; i++) {
        for (int j = 0; j < count - i - 1; j++) {
            if (arr[j]->priority < arr[j+1]->priority) {
                Reminder *tmp = arr[j];
                arr[j] = arr[j+1];
                arr[j+1] = tmp;
            }
        }
    }
    int limit = (n < count) ? n : count;
    for (int i = 0; i < limit; i++) {
        printf("%d %s %s\n", arr[i]->priority, arr[i]->title, arr[i]->note);
    }
    free(arr);
}

void list_all() {
    Reminder *cur = head;
    while (cur) {
        printf("%d %s %s\n", cur->priority, cur->title, cur->note);
        cur = cur->next;
    }
}

void free_all() {
    Reminder *cur = head;
    while (cur) {
        Reminder *tmp = cur;
        cur = cur->next;
        free(tmp->title);
        free(tmp->note);
        free(tmp);
    }
}

int main() {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        if (strcmp(line, "END") == 0) break;
        char *cmd = strtok(line, " ");
        if (!cmd) continue;
        if (strcmp(cmd, "ADD") == 0) {
            char *prio_str = strtok(NULL, " ");
            if (!prio_str) continue;
            int priority = atoi(prio_str);
            char *title = strtok(NULL, " ");
            if (!title) continue;
            char *note = strtok(NULL, "");
            if (!note) note = "";
            add_reminder(priority, title, note);
        } else if (strcmp(cmd, "BUMP") == 0) {
            char *title = strtok(NULL, " ");
            if (!title) continue;
            char *delta_str = strtok(NULL, " ");
            if (!delta_str) continue;
            int delta = atoi(delta_str);
            bump_priority(title, delta);
        } else if (strcmp(cmd, "REMOVE") == 0) {
            char *title = strtok(NULL, " ");
            if (title) remove_reminder(title);
        } else if (strcmp(cmd, "TOP") == 0) {
            char *n_str = strtok(NULL, " ");
            if (!n_str) continue;
            int n = atoi(n_str);
            top_n(n);
        } else if (strcmp(cmd, "LIST") == 0) {
            list_all();
        }
    }
    free_all();
    return 0;
}