// F027.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Reminder {
    char *title;
    char *note;
    int priority;
    struct Reminder *next;
} Reminder;

Reminder *head = NULL;
Reminder *tail = NULL;

Reminder *find_reminder(const char *title) {
    Reminder *r = head;
    while (r) {
        if (strcmp(r->title, title) == 0) return r;
        r = r->next;
    }
    return NULL;
}

void add_reminder(int priority, const char *title, const char *note) {
    if (find_reminder(title)) return;
    Reminder *r = malloc(sizeof(Reminder));
    r->title = malloc(strlen(title) + 1);
    strcpy(r->title, title);
    r->note = malloc(strlen(note) + 1);
    strcpy(r->note, note);
    r->priority = priority;
    r->next = NULL;
    if (tail) tail->next = r;
    else head = r;
    tail = r;
}

void bump_reminder(const char *title, int delta) {
    Reminder *r = find_reminder(title);
    if (r) r->priority += delta;
}

void remove_reminder(const char *title) {
    Reminder *r = head, *prev = NULL;
    while (r) {
        if (strcmp(r->title, title) == 0) {
            if (prev) prev->next = r->next;
            else head = r->next;
            if (!head) tail = NULL;
            if (tail == r) tail = prev;
            free(r->title);
            free(r->note);
            free(r);
            return;
        }
        prev = r;
        r = r->next;
    }
}

void top_n(int n) {
    int count = 0;
    Reminder *r = head;
    while (r) { count++; r = r->next; }
    if (count == 0) return;
    if (n > count) n = count;
    Reminder **arr = malloc(count * sizeof(Reminder *));
    r = head;
    for (int i = 0; i < count; i++) { arr[i] = r; r = r->next; }
    for (int i = 0; i < count - 1; i++) {
        for (int j = i + 1; j < count; j++) {
            if (arr[i]->priority < arr[j]->priority) {
                Reminder *tmp = arr[i];
                arr[i] = arr[j];
                arr[j] = tmp;
            }
        }
    }
    for (int i = 0; i < n; i++) {
        printf("%s %s %d\n", arr[i]->title, arr[i]->note, arr[i]->priority);
    }
    free(arr);
}

void list_all(void) {
    Reminder *r = head;
    while (r) {
        printf("%s %s %d\n", r->title, r->note, r->priority);
        r = r->next;
    }
}

void free_all(void) {
    Reminder *r = head;
    while (r) {
        Reminder *tmp = r->next;
        free(r->title);
        free(r->note);
        free(r);
        r = tmp;
    }
}

int main() {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        char cmd[32];
        char title[256], note[4096];
        int priority, delta, n;
        if (sscanf(line, "%31s", cmd) < 1) continue;
        const char *rest = line + strlen(cmd) + 1;
        while (*rest == ' ') rest++;
        if (strcmp(cmd, "ADD") == 0) {
            if (sscanf(rest, "%d %255s", &priority, title) == 2) {
                const char *n = rest;
                while (*n && *n != ' ') n++;
                while (*n == ' ') n++;
                while (*n && *n != ' ') n++;
                while (*n == ' ') n++;
                if (*n) add_reminder(priority, title, n);
                else add_reminder(priority, title, "");
            }
        } else if (strcmp(cmd, "BUMP") == 0) {
            if (sscanf(rest, "%255s %d", title, &delta) == 2) bump_reminder(title, delta);
        } else if (strcmp(cmd, "REMOVE") == 0) {
            if (sscanf(rest, "%255s", title) == 1) remove_reminder(title);
        } else if (strcmp(cmd, "TOP") == 0) {
            if (sscanf(rest, "%d", &n) == 1) top_n(n);
        } else if (strcmp(cmd, "LIST") == 0) {
            list_all();
        }
    }
    free_all();
    return 0;
}