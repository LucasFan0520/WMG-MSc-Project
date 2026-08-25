/* F013.c */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Task {
    char *title;
    char *note;
    struct Task *next;
} Task;

Task *todo = NULL, *doing = NULL, *done = NULL;

Task *find_task(const char *title, Task **list) {
    Task *t = *list;
    while (t) {
        if (strcmp(t->title, title) == 0) return t;
        t = t->next;
    }
    return NULL;
}

void remove_from_list(Task **list, const char *title) {
    Task *t = *list, *prev = NULL;
    while (t && strcmp(t->title, title) != 0) {
        prev = t;
        t = t->next;
    }
    if (!t) return;
    if (prev) prev->next = t->next;
    else *list = t->next;
    free(t->title);
    free(t->note);
    free(t);
}

void add_to_list(Task **list, const char *title, const char *note) {
    Task *t = malloc(sizeof(Task));
    t->title = strdup(title);
    t->note = strdup(note);
    t->next = *list;
    *list = t;
}

void todo_task(const char *title, const char *note) {
    if (find_task(title, &todo) || find_task(title, &doing) || find_task(title, &done))
        return;
    add_to_list(&todo, title, note);
}

void start_task(const char *title) {
    Task *t = find_task(title, &todo);
    if (!t) return;
    remove_from_list(&todo, title);
    add_to_list(&doing, title, t->note);
}

void done_task(const char *title) {
    Task *t = find_task(title, &doing);
    if (!t) {
        t = find_task(title, &todo);
        if (!t) return;
        remove_from_list(&todo, title);
    } else {
        remove_from_list(&doing, title);
    }
    add_to_list(&done, title, t->note);
}

void remove_task(const char *title) {
    if (find_task(title, &todo)) remove_from_list(&todo, title);
    else if (find_task(title, &doing)) remove_from_list(&doing, title);
    else if (find_task(title, &done)) remove_from_list(&done, title);
}

void find_task_print(const char *title) {
    Task *t = find_task(title, &todo);
    char *listname = "todo";
    if (!t) { t = find_task(title, &doing); listname = "doing"; }
    if (!t) { t = find_task(title, &done); listname = "done"; }
    if (!t) { printf("NOT_FOUND\n"); return; }
    printf("%s ", listname);
    for (char *c = t->note; *c; c++) {
        putchar(*c == ' ' ? '_' : *c);
    }
    putchar('\n');
}

void print_list(Task *list, const char *name) {
    printf("%s\n", name);
    Task *t = list;
    while (t) {
        printf("%s ", t->title);
        for (char *c = t->note; *c; c++) {
            putchar(*c == ' ' ? '_' : *c);
        }
        putchar('\n');
        t = t->next;
    }
}

void report_all(void) {
    print_list(todo, "todo");
    print_list(doing, "doing");
    print_list(done, "done");
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

void free_all(void) {
    free_list(todo);
    free_list(doing);
    free_list(done);
}

int main(void) {
    char *line = NULL;
    size_t len = 0;
    while (getline(&line, &len, stdin) != -1) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        char cmd[16];
        if (sscanf(line, "%15s", cmd) < 1) continue;
        if (strcmp(cmd, "TODO") == 0) {
            char title[256];
            char *p = line + 5;
            if (sscanf(p, "%255s", title) != 1) continue;
            while (*p && *p != ' ') p++;
            while (*p == ' ') p++;
            todo_task(title, p);
        } else if (strcmp(cmd, "START") == 0) {
            char *p = line + 6;
            while (*p == ' ') p++;
            start_task(p);
        } else if (strcmp(cmd, "DONE") == 0) {
            char *p = line + 5;
            while (*p == ' ') p++;
            done_task(p);
        } else if (strcmp(cmd, "REMOVE") == 0) {
            char *p = line + 7;
            while (*p == ' ') p++;
            remove_task(p);
        } else if (strcmp(cmd, "FIND") == 0) {
            char *p = line + 5;
            while (*p == ' ') p++;
            find_task_print(p);
        } else if (strcmp(cmd, "REPORT") == 0) {
            report_all();
        }
    }
    free_all();
    free(line);
    return 0;
}