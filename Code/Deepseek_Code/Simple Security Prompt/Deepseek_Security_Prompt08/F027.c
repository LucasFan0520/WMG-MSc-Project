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

Reminder *head = NULL, *tail = NULL;

Reminder *find_reminder(const char *title) {
    Reminder *r = head;
    while (r) {
        if (strcmp(r->title, title) == 0) return r;
        r = r->next;
    }
    return NULL;
}

void add_reminder(int priority, const char *title, const char *note) {
    Reminder *r = malloc(sizeof(Reminder));
    r->priority = priority;
    r->title = malloc(strlen(title) + 1);
    strcpy(r->title, title);
    r->note = malloc(strlen(note) + 1);
    strcpy(r->note, note);
    r->next = NULL;
    if (tail) {
        tail->next = r;
        tail = r;
    } else {
        head = tail = r;
    }
}

void bump_reminder(const char *title, int delta) {
    Reminder *r = find_reminder(title);
    if (!r) return;
    r->priority += delta;
}

void remove_reminder(const char *title) {
    Reminder *prev = NULL, *curr = head;
    while (curr) {
        if (strcmp(curr->title, title) == 0) {
            if (prev) prev->next = curr->next;
            else head = curr->next;
            if (curr == tail) tail = prev;
            free(curr->title);
            free(curr->note);
            free(curr);
            return;
        }
        prev = curr;
        curr = curr->next;
    }
}

void top_reminders(int n) {
    int count = 0;
    Reminder *r = head;
    while (r) { count++; r = r->next; }
    if (count == 0) return;
    Reminder **arr = malloc(count * sizeof(Reminder *));
    int i = 0;
    r = head;
    while (r) { arr[i++] = r; r = r->next; }
    for (int i = 0; i < count - 1; i++) {
        for (int j = i + 1; j < count; j++) {
            if (arr[i]->priority < arr[j]->priority) {
                Reminder *tmp = arr[i];
                arr[i] = arr[j];
                arr[j] = tmp;
            }
        }
    }
    for (int i = 0; i < n && i < count; i++) {
        printf("%d %s %s\n", arr[i]->priority, arr[i]->title, arr[i]->note);
    }
    free(arr);
}

void list_reminders(void) {
    Reminder *r = head;
    while (r) {
        printf("%d %s %s\n", r->priority, r->title, r->note);
        r = r->next;
    }
}

void free_all(void) {
    Reminder *r = head;
    while (r) {
        Reminder *next = r->next;
        free(r->title);
        free(r->note);
        free(r);
        r = next;
    }
}

int main(void) {
    char *line = NULL;
    size_t len = 0;
    ssize_t read;
    while ((read = getline(&line, &len, stdin)) != -1) {
        if (line[read - 1] == '\n') line[read - 1] = '\0';
        if (strcmp(line, "END") == 0) break;
        char *cmd = strtok(line, " ");
        if (!cmd) continue;
        if (strcmp(cmd, "ADD") == 0) {
            char *prio = strtok(NULL, " ");
            char *title = strtok(NULL, " ");
            char *note = strtok(NULL, "");
            if (prio && title && note) add_reminder(atoi(prio), title, note);
        } else if (strcmp(cmd, "BUMP") == 0) {
            char *title = strtok(NULL, " ");
            char *delta = strtok(NULL, " ");
            if (title && delta) bump_reminder(title, atoi(delta));
        } else if (strcmp(cmd, "REMOVE") == 0) {
            char *title = strtok(NULL, " ");
            if (title) remove_reminder(title);
        } else if (strcmp(cmd, "TOP") == 0) {
            char *n = strtok(NULL, " ");
            if (n) top_reminders(atoi(n));
        } else if (strcmp(cmd, "LIST") == 0) {
            list_reminders();
        }
    }
    free(line);
    free_all();
    return 0;
}