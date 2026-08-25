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
int count = 0;

void add_activity(const char *time, const char *label, const char *note) {
    Activity *a = malloc(sizeof(Activity));
    a->time = strdup(time);
    a->label = strdup(label);
    a->note = strdup(note);
    a->next = NULL;
    if (tail) {
        tail->next = a;
        tail = a;
    } else {
        head = tail = a;
    }
    count++;
}

void delete_activity(const char *time, const char *label) {
    Activity *prev = NULL;
    Activity *a = head;
    while (a) {
        if (strcmp(a->time, time) == 0 && strcmp(a->label, label) == 0) {
            if (prev) prev->next = a->next;
            else head = a->next;
            if (!a->next) tail = prev;
            free(a->time);
            free(a->label);
            free(a->note);
            free(a);
            count--;
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
            if (!best || strcmp(a->time, best->time) < 0 ||
                (strcmp(a->time, best->time) == 0 && strcmp(a->label, best->label) < 0)) {
                best = a;
            }
        }
        a = a->next;
    }
    if (!best) {
        printf("NOT_FOUND\n");
    } else {
        printf("%s %s:", best->time, best->label);
        for (char *s = best->note; *s; s++) putchar(*s == ' ' ? '_' : *s);
        putchar('\n');
    }
}

int compare_activity(const void *a, const void *b) {
    Activity *aa = *(Activity **)a;
    Activity *bb = *(Activity **)b;
    int c = strcmp(aa->time, bb->time);
    if (c != 0) return c;
    return strcmp(aa->label, bb->label);
}

void sort_and_print() {
    if (!head) return;
    Activity **arr = malloc(count * sizeof(Activity *));
    Activity *a = head;
    int i = 0;
    while (a) {
        arr[i++] = a;
        a = a->next;
    }
    qsort(arr, count, sizeof(Activity *), compare_activity);
    for (i = 0; i < count; i++) {
        printf("%s %s:", arr[i]->time, arr[i]->label);
        for (char *s = arr[i]->note; *s; s++) putchar(*s == ' ' ? '_' : *s);
        putchar('\n');
    }
    free(arr);
}

void clear_all() {
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
    count = 0;
}

void free_all() {
    clear_all();
}

int main() {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        char cmd[32];
        if (sscanf(line, "%31s", cmd) != 1) continue;
        char rest[4096];
        char *p = line + strlen(cmd);
        while (*p == ' ') p++;
        strcpy(rest, p);
        if (strcmp(cmd, "ADD") == 0) {
            char time[256], label[256], note[4096];
            if (sscanf(rest, "%255s %255s %[^\n]", time, label, note) == 3) {
                add_activity(time, label, note);
            }
        } else if (strcmp(cmd, "DELETE") == 0) {
            char time[256], label[256];
            if (sscanf(rest, "%255s %255s", time, label) == 2) {
                delete_activity(time, label);
            }
        } else if (strcmp(cmd, "FIND") == 0) {
            char label[256];
            if (sscanf(rest, "%255s", label) == 1) {
                find_earliest(label);
            }
        } else if (strcmp(cmd, "SORT") == 0) {
            sort_and_print();
        } else if (strcmp(cmd, "CLEAR") == 0) {
            clear_all();
        }
    }
    free_all();
    return 0;
}