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

Task *find_task(Task *list, const char *title) {
    Task *t = list;
    while (t) {
        if (strcmp(t->title, title) == 0) return t;
        t = t->next;
    }
    return NULL;
}

Task *find_in_all(const char *title, Task **which) {
    Task *t = find_task(todo, title);
    if (t) { *which = todo; return t; }
    t = find_task(doing, title);
    if (t) { *which = doing; return t; }
    t = find_task(done, title);
    if (t) { *which = done; return t; }
    *which = NULL;
    return NULL;
}

void remove_from_list(Task **list, const char *title) {
    Task *prev = NULL, *curr = *list;
    while (curr) {
        if (strcmp(curr->title, title) == 0) {
            if (prev) prev->next = curr->next;
            else *list = curr->next;
            free(curr->title);
            free(curr->note);
            free(curr);
            return;
        }
        prev = curr;
        curr = curr->next;
    }
}

void add_to_list(Task **list, const char *title, const char *note) {
    if (find_task(*list, title)) return;
    Task *t = malloc(sizeof(Task));
    if (!t) return;
    t->title = strdup(title);
    t->note = strdup(note);
    t->next = *list;
    *list = t;
}

void todo_task(const char *title, const char *note) {
    Task *which = NULL;
    Task *t = find_in_all(title, &which);
    if (t) {
        return;
    }
    add_to_list(&todo, title, note);
}

void start_task(const char *title) {
    Task *which = NULL;
    Task *t = find_in_all(title, &which);
    if (!t || which == doing) return;
    remove_from_list(&which, title);
    add_to_list(&doing, t->title, t->note);
    char *note = strdup(t->note);
    add_to_list(&doing, title, note);
    free(note);
}

void done_task(const char *title) {
    Task *which = NULL;
    Task *t = find_in_all(title, &which);
    if (!t || which == done) return;
    char *note = strdup(t->note);
    remove_from_list(&which, title);
    add_to_list(&done, title, note);
    free(note);
}

void remove_task(const char *title) {
    Task *which = NULL;
    if (find_in_all(title, &which)) {
        remove_from_list(&which, title);
    }
}

void find_task_cmd(const char *title) {
    Task *which = NULL;
    Task *t = find_in_all(title, &which);
    if (!t) {
        printf("NOT_FOUND\n");
        return;
    }
    const char *listname;
    if (which == todo) listname = "todo";
    else if (which == doing) listname = "doing";
    else listname = "done";
    printf("%s %s\n", listname, t->note);
}

void report_all(void) {
    Task *t = todo;
    while (t) {
        printf("todo %s %s\n", t->title, t->note);
        t = t->next;
    }
    t = doing;
    while (t) {
        printf("doing %s %s\n", t->title, t->note);
        t = t->next;
    }
    t = done;
    while (t) {
        printf("done %s %s\n", t->title, t->note);
        t = t->next;
    }
}

void free_list(Task *list) {
    Task *t = list;
    while (t) {
        Task *tmp = t;
        t = t->next;
        free(tmp->title);
        free(tmp->note);
        free(tmp);
    }
}

void free_all(void) {
    free_list(todo);
    free_list(doing);
    free_list(done);
}

int main(void) {
    char line[1024];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        char cmd[16];
        if (sscanf(line, "%15s", cmd) < 1) continue;
        if (strcmp(cmd, "TODO") == 0) {
            char title[256];
            char *p = line + 5;
            while (*p == ' ') p++;
            if (sscanf(p, "%255s", title) == 1) {
                char *note = p;
                while (*note && *note != ' ') note++;
                while (*note && *note == ' ') note++;
                if (*note) {
                    todo_task(title, note);
                }
            }
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
            find_task_cmd(p);
        } else if (strcmp(cmd, "REPORT") == 0) {
            report_all();
        }
    }
    free_all();
    return 0;
}