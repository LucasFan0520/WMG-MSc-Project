// filename: F007.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Activity {
    char *time;
    char *label;
    char *note;
    struct Activity *next;
} Activity;

static Activity *head = NULL;

static void cleanup(void) {
    while (head) {
        Activity *tmp = head;
        head = head->next;
        free(tmp->time);
        free(tmp->label);
        free(tmp->note);
        free(tmp);
    }
}

static void add_activity(const char *time, const char *label, const char *note) {
    Activity *a = malloc(sizeof(Activity));
    if (!a) return;
    a->time = strdup(time);
    a->label = strdup(label);
    a->note = strdup(note);
    if (!a->time || !a->label || !a->note) {
        free(a->time);
        free(a->label);
        free(a->note);
        free(a);
        return;
    }
    a->next = head;
    head = a;
}

static void delete_activity(const char *time, const char *label) {
    Activity *prev = NULL;
    for (Activity *a = head; a; a = a->next) {
        if (strcmp(a->time, time) == 0 && strcmp(a->label, label) == 0) {
            if (prev) prev->next = a->next;
            else head = a->next;
            free(a->time);
            free(a->label);
            free(a->note);
            free(a);
            return;
        }
        prev = a;
    }
}

static void find_earliest(const char *label) {
    Activity *best = NULL;
    for (Activity *a = head; a; a = a->next) {
        if (strcmp(a->label, label) == 0) {
            if (!best || strcmp(a->time, best->time) < 0)
                best = a;
        }
    }
    if (best) {
        printf("%s %s %s\n", best->time, best->label, best->note);
    }
}

static void sort_activities(void) {
    // Simple bubble sort on linked list: convert to array, sort, rebuild
    int count = 0;
    for (Activity *a = head; a; a = a->next) ++count;
    if (count < 2) return;
    Activity **arr = malloc(count * sizeof(Activity *));
    if (!arr) return;
    int i = 0;
    for (Activity *a = head; a; a = a->next) arr[i++] = a;
    for (int j = 0; j < count - 1; ++j) {
        for (int k = 0; k < count - j - 1; ++k) {
            int cmp = strcmp(arr[k]->time, arr[k+1]->time);
            if (cmp > 0 || (cmp == 0 && strcmp(arr[k]->label, arr[k+1]->label) > 0)) {
                Activity *tmp = arr[k];
                arr[k] = arr[k+1];
                arr[k+1] = tmp;
            }
        }
    }
    head = arr[0];
    for (int j = 0; j < count - 1; ++j) arr[j]->next = arr[j+1];
    arr[count-1]->next = NULL;
    free(arr);
}

static void print_activities(void) {
    sort_activities();
    for (Activity *a = head; a; a = a->next)
        printf("%s %s %s\n", a->time, a->label, a->note);
}

static void clear_all(void) {
    cleanup();
    head = NULL;
}

int main(void) {
    char *line = NULL;
    size_t linelen = 0;
    atexit(cleanup);
    while (getline(&line, &linelen, stdin) != -1) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        char *p = line;
        while (*p == ' ') ++p;
        char *cmd = p;
        while (*p && *p != ' ') ++p;
        if (*p) *p++ = '\0';
        while (*p == ' ') ++p;
        char *arg1 = p;
        while (*p && *p != ' ') ++p;
        if (*p) *p++ = '\0';
        while (*p == ' ') ++p;
        char *arg2 = p;
        while (*p && *p != ' ') ++p;
        if (*p) *p++ = '\0';
        while (*p == ' ') ++p;
        char *arg3 = p;
        if (strcmp(cmd, "ADD") == 0) {
            if (*arg1 && *arg2) add_activity(arg1, arg2, arg3);
        } else if (strcmp(cmd, "DELETE") == 0) {
            if (*arg1 && *arg2) delete_activity(arg1, arg2);
        } else if (strcmp(cmd, "FIND") == 0) {
            if (*arg1) find_earliest(arg1);
        } else if (strcmp(cmd, "SORT") == 0) {
            print_activities();
        } else if (strcmp(cmd, "CLEAR") == 0) {
            clear_all();
        }
    }
    free(line);
    return 0;
}