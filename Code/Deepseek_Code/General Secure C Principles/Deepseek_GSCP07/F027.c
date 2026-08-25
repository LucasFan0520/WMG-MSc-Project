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
    Reminder *r = head;
    while (r) {
        if (strcmp(r->title, title) == 0) return r;
        r = r->next;
    }
    return NULL;
}

void free_reminder(Reminder *r) {
    free(r->title);
    free(r->note);
    free(r);
}

void handle_add(char *cmd) {
    char *pstr = strtok(NULL, " ");
    if (!pstr) return;
    int prio = atoi(pstr);
    char *title = strtok(NULL, " ");
    if (!title) return;
    if (find_reminder(title)) return;
    char *note = strtok(NULL, "");
    if (!note) note = "";
    Reminder *new = malloc(sizeof(Reminder));
    new->priority = prio;
    new->title = strdup(title);
    new->note = strdup(note);
    new->next = NULL;
    if (!head) head = new;
    else {
        Reminder *last = head;
        while (last->next) last = last->next;
        last->next = new;
    }
}

void handle_bump(char *cmd) {
    char *title = strtok(NULL, " ");
    if (!title) return;
    char *dstr = strtok(NULL, " ");
    if (!dstr) return;
    int delta = atoi(dstr);
    Reminder *r = find_reminder(title);
    if (r) r->priority += delta;
}

void handle_remove(char *cmd) {
    char *title = strtok(NULL, " ");
    if (!title) return;
    Reminder *r = head, *prev = NULL;
    while (r) {
        if (strcmp(r->title, title) == 0) {
            if (prev) prev->next = r->next;
            else head = r->next;
            free_reminder(r);
            return;
        }
        prev = r;
        r = r->next;
    }
}

void handle_top(char *cmd) {
    char *nstr = strtok(NULL, " ");
    if (!nstr) return;
    int n = atoi(nstr);
    if (n <= 0) return;
    // Count reminders
    int count = 0;
    Reminder *r = head;
    while (r) { count++; r = r->next; }
    if (count == 0) return;
    // Copy pointers to array and sort by priority descending
    Reminder **arr = malloc(count * sizeof(Reminder*));
    r = head;
    for (int i = 0; i < count; i++) { arr[i] = r; r = r->next; }
    for (int i = 0; i < count-1; i++) {
        for (int j = i+1; j < count; j++) {
            if (arr[i]->priority < arr[j]->priority) {
                Reminder *tmp = arr[i];
                arr[i] = arr[j];
                arr[j] = tmp;
            }
        }
    }
    int limit = n < count ? n : count;
    for (int i = 0; i < limit; i++) {
        printf("%d %s %s\n", arr[i]->priority, arr[i]->title, arr[i]->note);
    }
    free(arr);
}

void handle_list() {
    Reminder *r = head;
    while (r) {
        printf("%d %s %s\n", r->priority, r->title, r->note);
        r = r->next;
    }
}

int main() {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = '\0';
        char *cmd = strtok(line, " ");
        if (!cmd) continue;
        if (strcmp(cmd, "END") == 0) break;
        if (strcmp(cmd, "ADD") == 0) handle_add(cmd);
        else if (strcmp(cmd, "BUMP") == 0) handle_bump(cmd);
        else if (strcmp(cmd, "REMOVE") == 0) handle_remove(cmd);
        else if (strcmp(cmd, "TOP") == 0) handle_top(cmd);
        else if (strcmp(cmd, "LIST") == 0) handle_list();
    }
    while (head) {
        Reminder *tmp = head;
        head = head->next;
        free_reminder(tmp);
    }
    return 0;
}