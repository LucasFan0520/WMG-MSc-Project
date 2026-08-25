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
Activity *tail = NULL;

Activity *find_earliest(const char *label) {
    Activity *best = NULL;
    Activity *a = head;
    while (a) {
        if (strcmp(a->label, label) == 0) {
            if (!best || strcmp(a->time, best->time) < 0 ||
                (strcmp(a->time, best->time) == 0 && strcmp(a->label, best->label) < 0))
                best = a;
        }
        a = a->next;
    }
    return best;
}

void add_activity(const char *time, const char *label, const char *note) {
    Activity *a = malloc(sizeof(Activity));
    a->time = malloc(strlen(time)+1);
    strcpy(a->time, time);
    a->label = malloc(strlen(label)+1);
    strcpy(a->label, label);
    a->note = malloc(strlen(note)+1);
    strcpy(a->note, note);
    a->next = NULL;
    if (tail) tail->next = a;
    else head = a;
    tail = a;
}

void delete_activity(const char *time, const char *label) {
    Activity *prev = NULL;
    Activity *a = head;
    while (a) {
        if (strcmp(a->time, time) == 0 && strcmp(a->label, label) == 0) {
            if (prev) prev->next = a->next;
            else head = a->next;
            if (a == tail) tail = prev;
            free(a->time);
            free(a->label);
            free(a->note);
            free(a);
            a = prev ? prev->next : head;
        } else {
            prev = a;
            a = a->next;
        }
    }
}

void find_label(const char *label) {
    Activity *a = find_earliest(label);
    if (a) {
        printf("%s %s %s\n", a->time, a->label, a->note);
    }
}

int compare_activity(const void *p1, const void *p2) {
    Activity *a1 = *(Activity**)p1;
    Activity *a2 = *(Activity**)p2;
    int cmp = strcmp(a1->time, a2->time);
    if (cmp != 0) return cmp;
    return strcmp(a1->label, a2->label);
}

void sort_activities(void) {
    if (!head) return;
    int count = 0;
    Activity *a = head;
    while (a) { count++; a = a->next; }
    Activity **arr = malloc(count * sizeof(Activity*));
    a = head;
    for (int i=0; i<count; i++) { arr[i] = a; a = a->next; }
    qsort(arr, count, sizeof(Activity*), compare_activity);
    for (int i=0; i<count; i++) {
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
    char line[1024];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        if (strcmp(line, "END") == 0) break;
        char *cmd = strtok(line, " ");
        if (!cmd) continue;
        char *rest = strtok(NULL, "");
        if (!rest) rest = "";
        if (strcmp(cmd, "ADD") == 0) {
            char *time = strtok(rest, " ");
            if (!time) continue;
            char *label = strtok(NULL, " ");
            if (!label) continue;
            char *note = strtok(NULL, "");
            if (!note) note = "";
            add_activity(time, label, note);
        } else if (strcmp(cmd, "DELETE") == 0) {
            char *time = strtok(rest, " ");
            if (!time) continue;
            char *label = strtok(NULL, " ");
            if (label) delete_activity(time, label);
        } else if (strcmp(cmd, "FIND") == 0) {
            char *label = strtok(rest, " ");
            if (label) find_label(label);
        } else if (strcmp(cmd, "SORT") == 0) {
            sort_activities();
        } else if (strcmp(cmd, "CLEAR") == 0) {
            clear_all();
        }
    }
    free_all();
    return 0;
}