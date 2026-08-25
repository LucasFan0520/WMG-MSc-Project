/* F027.c */
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
Reminder *tail = NULL;

Reminder *find_reminder(char *title) {
    for (Reminder *r = head; r; r = r->next)
        if (strcmp(r->title, title) == 0) return r;
    return NULL;
}

void add_reminder(int priority, char *title, char *note) {
    if (find_reminder(title)) return;
    Reminder *r = malloc(sizeof(Reminder));
    r->priority = priority;
    r->title = strdup(title);
    r->note = strdup(note);
    r->next = NULL;
    if (!tail) head = tail = r;
    else { tail->next = r; tail = r; }
}

void bump_priority(char *title, int delta) {
    Reminder *r = find_reminder(title);
    if (r) r->priority += delta;
}

void remove_reminder(char *title) {
    Reminder *prev = NULL;
    for (Reminder *r = head; r; r = r->next) {
        if (strcmp(r->title, title) == 0) {
            if (prev) prev->next = r->next;
            else head = r->next;
            if (r == tail) tail = prev;
            free(r->title);
            free(r->note);
            free(r);
            return;
        }
        prev = r;
    }
}

void top_n(int n) {
    // copy to array and sort by priority descending
    int count = 0;
    for (Reminder *r = head; r; r = r->next) count++;
    if (count == 0) return;
    Reminder **arr = malloc(count * sizeof(Reminder*));
    int i = 0;
    for (Reminder *r = head; r; r = r->next) arr[i++] = r;
    for (int j = 0; j < count-1; j++) {
        for (int k = j+1; k < count; k++) {
            if (arr[k]->priority > arr[j]->priority) {
                Reminder *tmp = arr[j];
                arr[j] = arr[k];
                arr[k] = tmp;
            }
        }
    }
    int limit = n < count ? n : count;
    for (int j = 0; j < limit; j++) {
        printf("%d %s %s\n", arr[j]->priority, arr[j]->title, arr[j]->note);
    }
    free(arr);
}

void list_all(void) {
    for (Reminder *r = head; r; r = r->next)
        printf("%d %s %s\n", r->priority, r->title, r->note);
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

int main() {
    char *line = NULL;
    size_t len = 0;
    while (getline(&line, &len, stdin) != -1) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        char cmd[16], arg1[256], arg2[256], arg3[1024];
        int n = sscanf(line, "%15s %255s %255s %[^\n]", cmd, arg1, arg2, arg3);
        if (n < 1) continue;
        if (strcmp(cmd, "ADD") == 0 && n == 4) {
            add_reminder(atoi(arg1), arg2, arg3);
        } else if (strcmp(cmd, "BUMP") == 0 && n == 3) {
            bump_priority(arg1, atoi(arg2));
        } else if (strcmp(cmd, "REMOVE") == 0 && n == 2) {
            remove_reminder(arg1);
        } else if (strcmp(cmd, "TOP") == 0 && n == 2) {
            top_n(atoi(arg1));
        } else if (strcmp(cmd, "LIST") == 0 && n == 1) {
            list_all();
        }
    }
    free(line);
    free_all();
    return 0;
}