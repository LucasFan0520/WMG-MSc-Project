/* F013.c */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

typedef struct Task {
    char *title;
    char *note;
    struct Task *next;
} Task;

Task *todo = NULL;
Task *doing = NULL;
Task *done = NULL;

Task *find_in_list(Task *list, const char *title) {
    while (list) {
        if (strcmp(list->title, title) == 0) return list;
        list = list->next;
    }
    return NULL;
}

void free_task(Task *t) {
    free(t->title);
    free(t->note);
    free(t);
}

void remove_from_list(Task **list, const char *title) {
    Task *prev = NULL, *curr = *list;
    while (curr) {
        if (strcmp(curr->title, title) == 0) {
            if (prev) prev->next = curr->next;
            else *list = curr->next;
            free_task(curr);
            return;
        }
        prev = curr;
        curr = curr->next;
    }
}

void add_to_list(Task **list, const char *title, const char *note) {
    Task *t = malloc(sizeof(Task));
    t->title = strdup(title);
    t->note = strdup(note);
    t->next = *list;
    *list = t;
}

Task *find_task_any(const char *title) {
    Task *t = find_in_list(todo, title);
    if (t) return t;
    t = find_in_list(doing, title);
    if (t) return t;
    return find_in_list(done, title);
}

void todo_add(const char *title, const char *note) {
    if (find_task_any(title)) return;
    add_to_list(&todo, title, note);
}

void start_task(const char *title) {
    Task *t = find_in_list(todo, title);
    if (!t) return;
    remove_from_list(&todo, title);
    add_to_list(&doing, title, t->note);
}

void done_task(const char *title) {
    Task *t = find_in_list(doing, title);
    if (!t) return;
    remove_from_list(&doing, title);
    add_to_list(&done, title, t->note);
}

void remove_task(const char *title) {
    if (find_in_list(todo, title)) remove_from_list(&todo, title);
    else if (find_in_list(doing, title)) remove_from_list(&doing, title);
    else if (find_in_list(done, title)) remove_from_list(&done, title);
}

void find_task_print(const char *title) {
    Task *t = find_task_any(title);
    if (!t) {
        printf("NOT_FOUND\n");
        return;
    }
    const char *listname = "todo";
    if (find_in_list(doing, title)) listname = "doing";
    else if (find_in_list(done, title)) listname = "done";
    printf("%s %s\n", listname, t->note);
}

void report_all(void) {
    printf("TODO:\n");
    Task *t = todo;
    while (t) { printf("%s %s\n", t->title, t->note); t = t->next; }
    printf("DOING:\n");
    t = doing;
    while (t) { printf("%s %s\n", t->title, t->note); t = t->next; }
    printf("DONE:\n");
    t = done;
    while (t) { printf("%s %s\n", t->title, t->note); t = t->next; }
}

void trim_newline(char *s) {
    size_t len = strlen(s);
    if (len > 0 && s[len-1] == '\n') s[len-1] = '\0';
}

int main(void) {
    char line[4096];
    char cmd[32];
    char title[256];
    char note[4096];

    while (fgets(line, sizeof(line), stdin)) {
        trim_newline(line);
        if (strcmp(line, "END") == 0) break;
        if (sscanf(line, "%31s", cmd) != 1) continue;

        if (strcmp(cmd, "TODO") == 0) {
            char *p = line + 4;
            while (*p == ' ') p++;
            if (sscanf(p, "%255s", title) != 1) continue;
            p += strlen(title);
            while (*p == ' ') p++;
            todo_add(title, p);
        } else if (strcmp(cmd, "START") == 0) {
            char *p = line + 5;
            while (*p == ' ') p++;
            if (sscanf(p, "%255s", title) == 1) start_task(title);
        } else if (strcmp(cmd, "DONE") == 0) {
            char *p = line + 4;
            while (*p == ' ') p++;
            if (sscanf(p, "%255s", title) == 1) done_task(title);
        } else if (strcmp(cmd, "REMOVE") == 0) {
            char *p = line + 6;
            while (*p == ' ') p++;
            if (sscanf(p, "%255s", title) == 1) remove_task(title);
        } else if (strcmp(cmd, "FIND") == 0) {
            char *p = line + 4;
            while (*p == ' ') p++;
            if (sscanf(p, "%255s", title) == 1) find_task_print(title);
        } else if (strcmp(cmd, "REPORT") == 0) {
            report_all();
        }
    }

    while (todo) { Task *t = todo; todo = todo->next; free_task(t); }
    while (doing) { Task *t = doing; doing = doing->next; free_task(t); }
    while (done) { Task *t = done; done = done->next; free_task(t); }
    return 0;
}