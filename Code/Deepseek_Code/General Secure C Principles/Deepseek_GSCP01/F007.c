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

void add_activity(const char *time, const char *label, const char *note) {
    Activity *a = malloc(sizeof(Activity));
    a->time = strdup(time);
    a->label = strdup(label);
    a->note = strdup(note);
    a->next = head;
    head = a;
}

void delete_activity(const char *time, const char *label) {
    Activity *a = head, *prev = NULL;
    while (a) {
        if (strcmp(a->time, time) == 0 && strcmp(a->label, label) == 0) {
            if (prev) prev->next = a->next;
            else head = a->next;
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

void find_activity(const char *label) {
    Activity *a = head;
    Activity *earliest = NULL;
    while (a) {
        if (strcmp(a->label, label) == 0) {
            if (!earliest || strcmp(a->time, earliest->time) < 0)
                earliest = a;
        }
        a = a->next;
    }
    if (!earliest) { printf("NOT_FOUND\n"); return; }
    printf("%s %s ", earliest->time, earliest->label);
    for (char *c = earliest->note; *c; c++) {
        putchar(*c == ' ' ? '_' : *c);
    }
    putchar('\n');
}

int compare_activities(const void *a, const void *b) {
    Activity **aa = (Activity **)a;
    Activity **bb = (Activity **)b;
    int cmp = strcmp((*aa)->time, (*bb)->time);
    if (cmp != 0) return cmp;
    return strcmp((*aa)->label, (*bb)->label);
}

void sort_activities(void) {
    int count = 0;
    Activity *a = head;
    while (a) { count++; a = a->next; }
    if (count == 0) return;
    Activity **arr = malloc(count * sizeof(Activity *));
    a = head;
    for (int i = 0; i < count; i++) {
        arr[i] = a;
        a = a->next;
    }
    qsort(arr, count, sizeof(Activity *), compare_activities);
    for (int i = 0; i < count; i++) {
        printf("%s %s ", arr[i]->time, arr[i]->label);
        for (char *c = arr[i]->note; *c; c++) {
            putchar(*c == ' ' ? '_' : *c);
        }
        putchar('\n');
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
    head = NULL;
}

void free_all(void) {
    clear_all();
}

int main(void) {
    char *line = NULL;
    size_t len = 0;
    while (getline(&line, &len, stdin) != -1) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        char cmd[16];
        if (sscanf(line, "%15s", cmd) < 1) continue;
        if (strcmp(cmd, "ADD") == 0) {
            char time[256], label[256];
            char *p = line + 4;
            if (sscanf(p, "%255s %255s", time, label) != 2) continue;
            while (*p && *p != ' ') p++;
            while (*p == ' ') p++;
            while (*p && *p != ' ') p++;
            while (*p == ' ') p++;
            add_activity(time, label, p);
        } else if (strcmp(cmd, "DELETE") == 0) {
            char time[256], label[256];
            if (sscanf(line + 7, "%255s %255s", time, label) == 2)
                delete_activity(time, label);
        } else if (strcmp(cmd, "FIND") == 0) {
            char *p = line + 5;
            while (*p == ' ') p++;
            find_activity(p);
        } else if (strcmp(cmd, "SORT") == 0) {
            sort_activities();
        } else if (strcmp(cmd, "CLEAR") == 0) {
            clear_all();
        }
    }
    free_all();
    free(line);
    return 0;
}