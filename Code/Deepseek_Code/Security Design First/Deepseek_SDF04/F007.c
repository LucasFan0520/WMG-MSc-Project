// F007.c
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

void add_activity(const char *time, const char *label, const char *note) {
    Activity *a = malloc(sizeof(Activity));
    if (!a) return;
    a->time = strdup(time);
    a->label = strdup(label);
    a->note = strdup(note);
    a->next = NULL;
    if (!a->time || !a->label || !a->note) {
        free(a->time); free(a->label); free(a->note); free(a);
        return;
    }
    if (!head) head = a;
    else {
        Activity *last = head;
        while (last->next) last = last->next;
        last->next = a;
    }
}

void delete_activity(const char *time, const char *label) {
    Activity *a = head, *prev = NULL;
    while (a) {
        if (strcmp(a->time, time) == 0 && strcmp(a->label, label) == 0) {
            if (prev) prev->next = a->next;
            else head = a->next;
            free(a->time); free(a->label); free(a->note); free(a);
            a = prev ? prev->next : head;
        } else {
            prev = a;
            a = a->next;
        }
    }
}

void find_activity(const char *label) {
    Activity *a = head;
    while (a) {
        if (strcmp(a->label, label) == 0) {
            printf("%s %s %s\n", a->time, a->label, a->note);
            return;
        }
        a = a->next;
    }
}

int compare_activities(const Activity *a, const Activity *b) {
    int c = strcmp(a->time, b->time);
    if (c != 0) return c;
    return strcmp(a->label, b->label);
}

void sort_activities(void) {
    if (!head || !head->next) return;
    Activity *sorted = NULL;
    Activity *a = head;
    while (a) {
        Activity *next = a->next;
        if (!sorted || compare_activities(a, sorted) < 0) {
            a->next = sorted;
            sorted = a;
        } else {
            Activity *cur = sorted;
            while (cur->next && compare_activities(a, cur->next) >= 0)
                cur = cur->next;
            a->next = cur->next;
            cur->next = a;
        }
        a = next;
    }
    head = sorted;
}

void print_sorted(void) {
    sort_activities();
    Activity *a = head;
    while (a) {
        printf("%s %s %s\n", a->time, a->label, a->note);
        a = a->next;
    }
}

void clear_activities(void) {
    Activity *a = head;
    while (a) {
        Activity *next = a->next;
        free(a->time); free(a->label); free(a->note); free(a);
        a = next;
    }
    head = NULL;
}

void free_all(void) {
    clear_activities();
}

int main(void) {
    char *line = NULL;
    size_t len = 0;
    while (getline(&line, &len, stdin) != -1) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        char cmd[64], time[256], label[256], note[4096];
        if (sscanf(line, "%63s", cmd) != 1) continue;
        if (strcmp(cmd, "ADD") == 0) {
            if (sscanf(line, "%*s %255s %255s %4095[^\n]", time, label, note) == 3)
                add_activity(time, label, note);
        } else if (strcmp(cmd, "DELETE") == 0) {
            if (sscanf(line, "%*s %255s %255s", time, label) == 2)
                delete_activity(time, label);
        } else if (strcmp(cmd, "FIND") == 0) {
            if (sscanf(line, "%*s %255s", label) == 1)
                find_activity(label);
        } else if (strcmp(cmd, "SORT") == 0) {
            print_sorted();
        } else if (strcmp(cmd, "CLEAR") == 0) {
            clear_activities();
        }
    }
    free(line);
    free_all();
    return 0;
}