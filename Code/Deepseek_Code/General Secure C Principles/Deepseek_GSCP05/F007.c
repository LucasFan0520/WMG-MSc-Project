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

Activity *log_head = NULL;

void add_activity(const char *time, const char *label, const char *note) {
    Activity *a = malloc(sizeof(Activity));
    if (!a) return;
    a->time = strdup(time);
    a->label = strdup(label);
    a->note = strdup(note);
    a->next = log_head;
    log_head = a;
}

void delete_activity(const char *time, const char *label) {
    Activity *prev = NULL, *cur = log_head;
    while (cur) {
        if (strcmp(cur->time, time) == 0 && strcmp(cur->label, label) == 0) {
            if (prev) prev->next = cur->next;
            else log_head = cur->next;
            free(cur->time);
            free(cur->label);
            free(cur->note);
            free(cur);
            return;
        }
        prev = cur;
        cur = cur->next;
    }
}

void find_activity(const char *label) {
    Activity *best = NULL;
    Activity *cur = log_head;
    while (cur) {
        if (strcmp(cur->label, label) == 0) {
            if (!best || strcmp(cur->time, best->time) < 0) best = cur;
        }
        cur = cur->next;
    }
    if (best) {
        printf("%s %s: ", best->time, best->label);
        char *s = best->note;
        for (; *s; s++) putchar(*s == ' ' ? '_' : *s);
        putchar('\n');
    }
}

int compare_activity(const void *a, const void *b) {
    Activity *aa = *(Activity **)a;
    Activity *bb = *(Activity **)b;
    int cmp = strcmp(aa->time, bb->time);
    if (cmp != 0) return cmp;
    return strcmp(aa->label, bb->label);
}

void sort_activities(void) {
    int count = 0;
    Activity *cur = log_head;
    while (cur) { count++; cur = cur->next; }
    if (count == 0) return;
    Activity **arr = malloc(count * sizeof(Activity *));
    if (!arr) return;
    cur = log_head;
    for (int i = 0; i < count; i++) {
        arr[i] = cur;
        cur = cur->next;
    }
    qsort(arr, count, sizeof(Activity *), compare_activity);
    for (int i = 0; i < count; i++) {
        Activity *a = arr[i];
        printf("%s %s: ", a->time, a->label);
        char *s = a->note;
        for (; *s; s++) putchar(*s == ' ' ? '_' : *s);
        putchar('\n');
    }
    free(arr);
}

void clear_log(void) {
    Activity *cur = log_head;
    while (cur) {
        Activity *next = cur->next;
        free(cur->time);
        free(cur->label);
        free(cur->note);
        free(cur);
        cur = next;
    }
    log_head = NULL;
}

void free_all(void) {
    clear_log();
}

int main(void) {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        char cmd[16];
        if (sscanf(line, "%15s", cmd) != 1) continue;
        if (strcmp(cmd, "ADD") == 0) {
            char *p = line + 4;
            while (*p == ' ') p++;
            char *time = p;
            char *space = strchr(time, ' ');
            if (!space) continue;
            *space = '\0';
            char *label = space + 1;
            while (*label == ' ') label++;
            space = strchr(label, ' ');
            if (!space) continue;
            *space = '\0';
            char *note = space + 1;
            while (*note == ' ') note++;
            add_activity(time, label, note);
        } else if (strcmp(cmd, "DELETE") == 0) {
            char *p = line + 7;
            while (*p == ' ') p++;
            char *time = p;
            char *space = strchr(time, ' ');
            if (!space) continue;
            *space = '\0';
            char *label = space + 1;
            while (*label == ' ') label++;
            delete_activity(time, label);
        } else if (strcmp(cmd, "FIND") == 0) {
            char *p = line + 5;
            while (*p == ' ') p++;
            char *label = p;
            char *space = strchr(label, ' ');
            if (space) *space = '\0';
            find_activity(label);
        } else if (strcmp(cmd, "SORT") == 0) {
            sort_activities();
        } else if (strcmp(cmd, "CLEAR") == 0) {
            clear_log();
        }
    }
    free_all();
    return 0;
}