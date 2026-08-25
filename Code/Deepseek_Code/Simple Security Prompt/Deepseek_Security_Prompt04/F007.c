/* F007.c */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Activity {
    char *time;
    char *label;
    char *note;
    struct Activity *next;
} Activity;

Activity *head = NULL;
Activity *tail = NULL;

void add_activity(char *time, char *label, char *note) {
    Activity *a = malloc(sizeof(Activity));
    a->time = strdup(time);
    a->label = strdup(label);
    a->note = strdup(note);
    a->next = NULL;
    if (!tail) head = tail = a;
    else { tail->next = a; tail = a; }
}

void delete_activities(char *time, char *label) {
    Activity *prev = NULL;
    for (Activity *a = head; a; ) {
        if (strcmp(a->time, time) == 0 && strcmp(a->label, label) == 0) {
            Activity *next = a->next;
            if (prev) prev->next = next;
            else head = next;
            if (a == tail) tail = prev;
            free(a->time);
            free(a->label);
            free(a->note);
            free(a);
            a = next;
        } else {
            prev = a;
            a = a->next;
        }
    }
}

void find_earliest(char *label) {
    Activity *earliest = NULL;
    for (Activity *a = head; a; a = a->next) {
        if (strcmp(a->label, label) == 0) {
            if (!earliest || strcmp(a->time, earliest->time) < 0)
                earliest = a;
        }
    }
    if (earliest) {
        printf("%s %s %s\n", earliest->time, earliest->label, earliest->note);
    }
}

void sort_activities(void) {
    if (!head) return;
    // Simple insertion sort on linked list
    Activity *sorted = NULL;
    Activity *cur = head;
    while (cur) {
        Activity *next = cur->next;
        // Insert cur into sorted list
        if (!sorted || strcmp(cur->time, sorted->time) < 0 ||
            (strcmp(cur->time, sorted->time) == 0 && strcmp(cur->label, sorted->label) < 0)) {
            cur->next = sorted;
            sorted = cur;
        } else {
            Activity *s = sorted;
            while (s->next && (strcmp(s->next->time, cur->time) < 0 ||
                (strcmp(s->next->time, cur->time) == 0 && strcmp(s->next->label, cur->label) < 0)))
                s = s->next;
            cur->next = s->next;
            s->next = cur;
        }
        cur = next;
    }
    head = sorted;
    tail = NULL;
    for (Activity *a = head; a; a = a->next) tail = a;
}

void print_all_sorted(void) {
    sort_activities();
    for (Activity *a = head; a; a = a->next)
        printf("%s %s %s\n", a->time, a->label, a->note);
}

void clear_all(void) {
    Activity *a = head;
    while (a) {
        Activity *next = a->next;
        free(a->time);
        free(a->label);
        free(a->note);
        free(a);
        a = next;
    }
    head = tail = NULL;
}

void free_all(void) {
    clear_all();
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
            add_activity(arg1, arg2, arg3);
        } else if (strcmp(cmd, "DELETE") == 0 && n == 3) {
            delete_activities(arg1, arg2);
        } else if (strcmp(cmd, "FIND") == 0 && n == 2) {
            find_earliest(arg1);
        } else if (strcmp(cmd, "SORT") == 0 && n == 1) {
            print_all_sorted();
        } else if (strcmp(cmd, "CLEAR") == 0 && n == 1) {
            clear_all();
        }
    }
    free(line);
    free_all();
    return 0;
}