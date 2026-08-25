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
    Task *cur;
    Task **lists[] = {&todo, &doing, &done};
    for (int i = 0; i < 3; i++) {
        cur = *lists[i];
        while (cur) {
            if (strcmp(cur->title, title) == 0) {
                *list = *lists[i];
                return cur;
            }
            cur = cur->next;
        }
    }
    return NULL;
}

void remove_from_list(Task **list, const char *title) {
    Task *prev = NULL, *cur = *list;
    while (cur) {
        if (strcmp(cur->title, title) == 0) {
            if (prev) prev->next = cur->next;
            else *list = cur->next;
            return;
        }
        prev = cur;
        cur = cur->next;
    }
}

void add_task(Task **list, const char *title, const char *note) {
    Task *t = malloc(sizeof(Task));
    t->title = strdup(title);
    t->note = strdup(note);
    t->next = *list;
    *list = t;
}

void move_task(Task **from, Task **to, const char *title) {
    Task *prev = NULL, *cur = *from;
    while (cur) {
        if (strcmp(cur->title, title) == 0) {
            if (prev) prev->next = cur->next;
            else *from = cur->next;
            cur->next = *to;
            *to = cur;
            return;
        }
        prev = cur;
        cur = cur->next;
    }
}

void add_todo(const char *title, const char *note) {
    add_task(&todo, title, note);
}

void start_task(const char *title) {
    Task *list = NULL;
    Task *t = find_task(title, &list);
    if (!t) return;
    remove_from_list(&list, title);
    t->next = doing;
    doing = t;
}

void done_task(const char *title) {
    Task *list = NULL;
    Task *t = find_task(title, &list);
    if (!t) return;
    remove_from_list(&list, title);
    t->next = done;
    done = t;
}

void remove_task(const char *title) {
    Task *list = NULL;
    Task *t = find_task(title, &list);
    if (t) {
        remove_from_list(&list, title);
        free(t->title);
        free(t->note);
        free(t);
    }
}

void find_task_print(const char *title) {
    Task *list = NULL;
    Task *t = find_task(title, &list);
    if (!t) return;
    const char *lname;
    if (list == todo) lname = "todo";
    else if (list == doing) lname = "doing";
    else lname = "done";
    printf("%s %s\n", lname, t->note);
}

void print_list(const char *name, Task *list) {
    printf("%s\n", name);
    Task *t = list;
    while (t) {
        printf("%s\n", t->title);
        t = t->next;
    }
}

void free_tasks(Task **list) {
    while (*list) {
        Task *tmp = *list;
        *list = (*list)->next;
        free(tmp->title);
        free(tmp->note);
        free(tmp);
    }
}

void free_all(void) {
    free_tasks(&todo);
    free_tasks(&doing);
    free_tasks(&done);
}

int main(void) {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        char cmd[16];
        char title[256];
        int n = sscanf(line, "%s %s", cmd, title);
        if (strcmp(cmd, "TODO") == 0) {
            if (n < 2) continue;
            char *p = line + strlen(cmd) + 1 + strlen(title) + 1;
            while (*p == ' ') p++;
            if (*p) add_todo(title, p);
        } else if (strcmp(cmd, "START") == 0) {
            if (n < 2) continue;
            start_task(title);
        } else if (strcmp(cmd, "DONE") == 0) {
            if (n < 2) continue;
            done_task(title);
        } else if (strcmp(cmd, "REMOVE") == 0) {
            if (n < 2) continue;
            remove_task(title);
        } else if (strcmp(cmd, "FIND") == 0) {
            if (n < 2) continue;
            find_task_print(title);
        } else if (strcmp(cmd, "REPORT") == 0) {
            print_list("todo", todo);
            print_list("doing", doing);
            print_list("done", done);
        }
    }
    free_all();
    return 0;
}