// F007.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct activity {
    char *time;
    char *label;
    char *note;
    struct activity *next;
};

struct activity *log_head = NULL;
struct activity *log_tail = NULL;

void add_activity(char *time, char *label, char *note) {
    struct activity *a = malloc(sizeof(struct activity));
    a->time = malloc(strlen(time) + 1);
    strcpy(a->time, time);
    a->label = malloc(strlen(label) + 1);
    strcpy(a->label, label);
    a->note = malloc(strlen(note) + 1);
    strcpy(a->note, note);
    a->next = NULL;
    if (log_tail) {
        log_tail->next = a;
        log_tail = a;
    } else {
        log_head = log_tail = a;
    }
}

void delete_activity(char *time, char *label) {
    struct activity *a = log_head, *prev = NULL;
    while (a) {
        if (strcmp(a->time, time) == 0 && strcmp(a->label, label) == 0) {
            if (prev) prev->next = a->next;
            else log_head = a->next;
            if (log_tail == a) log_tail = prev;
            free(a->time);
            free(a->label);
            free(a->note);
            free(a);
            return;
        }
        prev = a;
        a = a->next;
    }
}

void find_label(char *label) {
    struct activity *a = log_head;
    struct activity *earliest = NULL;
    while (a) {
        if (strcmp(a->label, label) == 0) {
            if (!earliest || strcmp(a->time, earliest->time) < 0) {
                earliest = a;
            }
        }
        a = a->next;
    }
    if (earliest) {
        printf("%s %s %s\n", earliest->time, earliest->label, earliest->note);
    }
}

int compare_activity(const void *x, const void *y) {
    struct activity *a = *(struct activity **)x;
    struct activity *b = *(struct activity **)y;
    int cmp = strcmp(a->time, b->time);
    if (cmp != 0) return cmp;
    return strcmp(a->label, b->label);
}

void sort_activities() {
    int count = 0;
    struct activity *a = log_head;
    while (a) { count++; a = a->next; }
    if (count == 0) return;
    struct activity **arr = malloc(count * sizeof(struct activity *));
    a = log_head;
    for (int i = 0; i < count; i++) {
        arr[i] = a;
        a = a->next;
    }
    qsort(arr, count, sizeof(struct activity *), compare_activity);
    for (int i = 0; i < count; i++) {
        printf("%s %s %s\n", arr[i]->time, arr[i]->label, arr[i]->note);
    }
    free(arr);
}

void clear_log() {
    while (log_head) {
        struct activity *a = log_head;
        log_head = log_head->next;
        free(a->time);
        free(a->label);
        free(a->note);
        free(a);
    }
    log_tail = NULL;
}

int main() {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        if (strcmp(line, "END") == 0) break;
        char cmd[32];
        char time[256], label[256], note[4096];
        int n = sscanf(line, "%31s %255s %255s", cmd, time, label);
        if (strcmp(cmd, "ADD") == 0 && n >= 3) {
            char *rest = line + strlen(cmd) + 1 + strlen(time) + 1 + strlen(label) + 1;
            add_activity(time, label, rest);
        } else if (strcmp(cmd, "DELETE") == 0 && n >= 3) {
            delete_activity(time, label);
        } else if (strcmp(cmd, "FIND") == 0 && n >= 2) {
            find_label(label);
        } else if (strcmp(cmd, "SORT") == 0) {
            sort_activities();
        } else if (strcmp(cmd, "CLEAR") == 0) {
            clear_log();
        }
    }
    return 0;
}