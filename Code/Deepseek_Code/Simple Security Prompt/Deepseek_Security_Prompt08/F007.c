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

Activity *head = NULL, *tail = NULL;

void add_activity(const char *time, const char *label, const char *note) {
    Activity *a = malloc(sizeof(Activity));
    a->time = malloc(strlen(time) + 1);
    strcpy(a->time, time);
    a->label = malloc(strlen(label) + 1);
    strcpy(a->label, label);
    a->note = malloc(strlen(note) + 1);
    strcpy(a->note, note);
    a->next = NULL;
    if (tail) {
        tail->next = a;
        tail = a;
    } else {
        head = tail = a;
    }
}

void delete_activity(const char *time, const char *label) {
    Activity *prev = NULL, *curr = head;
    while (curr) {
        if (strcmp(curr->time, time) == 0 && strcmp(curr->label, label) == 0) {
            if (prev) prev->next = curr->next;
            else head = curr->next;
            if (curr == tail) tail = prev;
            Activity *next = curr->next;
            free(curr->time);
            free(curr->label);
            free(curr->note);
            free(curr);
            curr = next;
        } else {
            prev = curr;
            curr = curr->next;
        }
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

void sort_print(void) {
    int count = 0;
    Activity *a = head;
    while (a) { count++; a = a->next; }
    if (count == 0) return;
    Activity **arr = malloc(count * sizeof(Activity *));
    int i = 0;
    a = head;
    while (a) { arr[i++] = a; a = a->next; }
    for (int i = 0; i < count - 1; i++) {
        for (int j = i + 1; j < count; j++) {
            int cmp = strcmp(arr[i]->time, arr[j]->time);
            if (cmp > 0 || (cmp == 0 && strcmp(arr[i]->label, arr[j]->label) > 0)) {
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
            char *time = strtok(NULL, " ");
            char *label = strtok(NULL, " ");
            char *note = strtok(NULL, "");
            if (time && label && note) add_activity(time, label, note);
        } else if (strcmp(cmd, "DELETE") == 0) {
            char *time = strtok(NULL, " ");
            char *label = strtok(NULL, " ");
            if (time && label) delete_activity(time, label);
        } else if (strcmp(cmd, "FIND") == 0) {
            char *label = strtok(NULL, " ");
            if (label) find_earliest(label);
        } else if (strcmp(cmd, "SORT") == 0) {
            sort_print();
        } else if (strcmp(cmd, "CLEAR") == 0) {
            clear_all();
        }
    }
    free(line);
    free_all();
    return 0;
}