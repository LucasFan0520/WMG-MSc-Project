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

void free_act(Activity *a) {
    free(a->time);
    free(a->label);
    free(a->note);
    free(a);
}

void add_act(const char *time, const char *label, const char *note) {
    Activity *new = malloc(sizeof(Activity));
    new->time = strdup(time);
    new->label = strdup(label);
    new->note = strdup(note);
    new->next = NULL;
    if (!head) head = new;
    else {
        Activity *last = head;
        while (last->next) last = last->next;
        last->next = new;
    }
}

void delete_act(const char *time, const char *label) {
    Activity *a = head, *prev = NULL;
    while (a) {
        if (strcmp(a->time, time) == 0 && strcmp(a->label, label) == 0) {
            if (prev) prev->next = a->next;
            else head = a->next;
            free_act(a);
            return;
        }
        prev = a;
        a = a->next;
    }
}

void find_earliest(const char *label) {
    Activity *best = NULL;
    Activity *a = head;
    while (a) {
        if (strcmp(a->label, label) == 0) {
            if (!best || strcmp(a->time, best->time) < 0) best = a;
        }
        a = a->next;
    }
    if (best) {
        printf("%s %s %s\n", best->time, best->label, best->note);
    }
}

void sort_print() {
    // Simple insertion sort on linked list? We'll just collect and sort.
    // Since we need to print in order, we can use a simple array of pointers and qsort.
    int count = 0;
    Activity *a = head;
    while (a) { count++; a = a->next; }
    if (count == 0) return;
    Activity **arr = malloc(count * sizeof(Activity*));
    a = head;
    for (int i = 0; i < count; i++) { arr[i] = a; a = a->next; }
    // Sort by time then label using strcmp
    for (int i = 0; i < count-1; i++) {
        for (int j = i+1; j < count; j++) {
            if (strcmp(arr[i]->time, arr[j]->time) > 0 ||
                (strcmp(arr[i]->time, arr[j]->time) == 0 && strcmp(arr[i]->label, arr[j]->label) > 0)) {
                Activity *tmp = arr[i];
                arr[i] = arr[j];
                arr[j] = tmp;
            }
        }
    }
    for (int i = 0; i < count; i++) {
        printf("%s %s %s\n", arr[i]->time, arr[i]->label, arr[i]->note);
    }
    free(arr);
}

void clear_all() {
    while (head) {
        Activity *tmp = head;
        head = head->next;
        free_act(tmp);
    }
}

int main() {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = '\0';
        char *cmd = strtok(line, " ");
        if (!cmd) continue;
        if (strcmp(cmd, "END") == 0) break;
        if (strcmp(cmd, "ADD") == 0) {
            char *time = strtok(NULL, " ");
            char *label = strtok(NULL, " ");
            char *note = strtok(NULL, "");
            if (time && label) {
                if (!note) note = "";
                add_act(time, label, note);
            }
        } else if (strcmp(cmd, "DELETE") == 0) {
            char *time = strtok(NULL, " ");
            char *label = strtok(NULL, " ");
            if (time && label) delete_act(time, label);
        } else if (strcmp(cmd, "FIND") == 0) {
            char *label = strtok(NULL, " ");
            if (label) find_earliest(label);
        } else if (strcmp(cmd, "SORT") == 0) {
            sort_print();
        } else if (strcmp(cmd, "CLEAR") == 0) {
            clear_all();
        }
    }
    clear_all();
    return 0;
}