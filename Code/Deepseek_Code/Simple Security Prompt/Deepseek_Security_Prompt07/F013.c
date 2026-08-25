// F013.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Task {
    char *title;
    char *note;
    struct Task *next;
} Task;

Task *todo = NULL;
Task *doing = NULL;
Task *done = NULL;

Task *find_task_in_list(Task *list, const char *title) {
    Task *t = list;
    while (t) {
        if (strcmp(t->title, title) == 0) return t;
        t = t->next;
    }
    return NULL;
}

void remove_task_from_list(Task **list, const char *title) {
    Task *prev = NULL;
    Task *t = *list;
    while (t) {
        if (strcmp(t->title, title) == 0) {
            if (prev) prev->next = t->next;
            else *list = t->next;
            free(t->title);
            free(t->note);
            free(t);
            return;
        }
        prev = t;
        t = t->next;
    }
}

void add_task_to_list(Task **list, const char *title, const char *note) {
    Task *t = malloc(sizeof(Task));
    t->title = strdup(title);
    t->note = strdup(note);
    t->next = *list;
    *list = t;
}

void todo_task(const char *title, const char *note) {
    if (find_task_in_list(todo, title) || find_task_in_list(doing, title) || find_task_in_list(done, title)) return;
    add_task_to_list(&todo, title, note);
}

void start_task(const char *title) {
    Task *t = find_task_in_list(todo, title);
    if (!t) return;
    char *note = strdup(t->note);
    remove_task_from_list(&todo, title);
    add_task_to_list(&doing, title, note);
    free(note);
}

void done_task(const char *title) {
    Task *t = find_task_in_list(doing, title);
    if (t) {
        char *note = strdup(t->note);
        remove_task_from_list(&doing, title);
        add_task_to_list(&done, title, note);
        free(note);
        return;
    }
    t = find_task_in_list(todo, title);
    if (t) {
        char *note = strdup(t->note);
        remove_task_from_list(&todo, title);
        add_task_to_list(&done, title, note);
        free(note);
    }
}

void remove_task(const char *title) {
    if (find_task_in_list(todo, title)) {
        remove_task_from_list(&todo, title);
    } else if (find_task_in_list(doing, title)) {
        remove_task_from_list(&doing, title);
    } else if (find_task_in_list(done, title)) {
        remove_task_from_list(&done, title);
    }
}

void find_task(const char *title) {
    Task *t = find_task_in_list(todo, title);
    if (t) { printf("TODO:"); for (char *s = t->note; *s; s++) putchar(*s == ' ' ? '_' : *s); putchar('\n'); return; }
    t = find_task_in_list(doing, title);
    if (t) { printf("DOING:"); for (char *s = t->note; *s; s++) putchar(*s == ' ' ? '_' : *s); putchar('\n'); return; }
    t = find_task_in_list(done, title);
    if (t) { printf("DONE:"); for (char *s = t->note; *s; s++) putchar(*s == ' ' ? '_' : *s); putchar('\n'); return; }
    printf("NOT_FOUND\n");
}

void print_list(Task *list, const char *label) {
    Task *t = list;
    while (t) {
        printf("%s %s:", label, t->title);
        for (char *s = t->note; *s; s++) putchar(*s == ' ' ? '_' : *s);
        putchar('\n');
        t = t->next;
    }
}

void report_all() {
    print_list(todo, "TODO");
    print_list(doing, "DOING");
    print_list(done, "DONE");
}

void free_list(Task *list) {
    Task *t = list;
    while (t) {
        Task *next = t->next;
        free(t->title);
        free(t->note);
        free(t);
        t = next;
    }
}

void free_all() {
    free_list(todo);
    free_list(doing);
    free_list(done);
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
        if (strcmp(cmd, "TODO") == 0) {
            char title[256], note[4096];
            if (sscanf(rest, "%255s %[^\n]", title, note) == 2) {
                todo_task(title, note);
            }
        } else if (strcmp(cmd, "START") == 0) {
            char title[256];
            if (sscanf(rest, "%255s", title) == 1) {
                start_task(title);
            }
        } else if (strcmp(cmd, "DONE") == 0) {
            char title[256];
            if (sscanf(rest, "%255s", title) == 1) {
                done_task(title);
            }
        } else if (strcmp(cmd, "REMOVE") == 0) {
            char title[256];
            if (sscanf(rest, "%255s", title) == 1) {
                remove_task(title);
            }
        } else if (strcmp(cmd, "FIND") == 0) {
            char title[256];
            if (sscanf(rest, "%255s", title) == 1) {
                find_task(title);
            }
        } else if (strcmp(cmd, "REPORT") == 0) {
            report_all();
        }
    }
    free_all();
    return 0;
}