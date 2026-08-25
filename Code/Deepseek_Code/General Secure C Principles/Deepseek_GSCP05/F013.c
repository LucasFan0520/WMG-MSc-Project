// F013.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Task {
    char *title;
    char *note;
    struct Task *next;
} Task;

Task *todo = NULL, *doing = NULL, *done = NULL;

Task *find_task(const char *title, Task **list_ptr) {
    Task *lists[] = {todo, doing, done};
    const char *names[] = {"todo", "doing", "done"};
    for (int i = 0; i < 3; i++) {
        Task *t = lists[i];
        while (t) {
            if (strcmp(t->title, title) == 0) {
                if (list_ptr) *list_ptr = lists[i];
                return t;
            }
            t = t->next;
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

void add_to_list(Task **list, const char *title, const char *note) {
    Task *t = malloc(sizeof(Task));
    if (!t) return;
    t->title = strdup(title);
    t->note = strdup(note);
    t->next = *list;
    *list = t;
}

void todo_task(const char *title, const char *note) {
    if (find_task(title, NULL)) return;
    add_to_list(&todo, title, note);
}

void start_task(const char *title) {
    Task *list = NULL;
    Task *t = find_task(title, &list);
    if (!t) return;
    remove_from_list(&list, title);
    add_to_list(&doing, t->title, t->note);
    free(t->title);
    free(t->note);
    free(t);
}

void done_task(const char *title) {
    Task *list = NULL;
    Task *t = find_task(title, &list);
    if (!t) return;
    remove_from_list(&list, title);
    add_to_list(&done, t->title, t->note);
    free(t->title);
    free(t->note);
    free(t);
}

void remove_task(const char *title) {
    Task *list = NULL;
    Task *t = find_task(title, &list);
    if (!t) return;
    remove_from_list(&list, title);
    free(t->title);
    free(t->note);
    free(t);
}

void find_task_print(const char *title) {
    Task *list = NULL;
    Task *t = find_task(title, &list);
    if (!t) return;
    const char *list_name;
    if (list == todo) list_name = "todo";
    else if (list == doing) list_name = "doing";
    else list_name = "done";
    printf("%s: ", list_name);
    char *s = t->note;
    for (; *s; s++) putchar(*s == ' ' ? '_' : *s);
    putchar('\n');
}

void report_all(void) {
    Task *lists[] = {todo, doing, done};
    const char *names[] = {"todo", "doing", "done"};
    for (int i = 0; i < 3; i++) {
        printf("%s:\n", names[i]);
        Task *t = lists[i];
        while (t) {
            printf("%s: ", t->title);
            char *s = t->note;
            for (; *s; s++) putchar(*s == ' ' ? '_' : *s);
            putchar('\n');
            t = t->next;
        }
    }
}

void free_list(Task *list) {
    while (list) {
        Task *next = list->next;
        free(list->title);
        free(list->note);
        free(list);
        list = next;
    }
}

void free_all(void) {
    free_list(todo);
    free_list(doing);
    free_list(done);
}

int main(void) {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        char cmd[16];
        if (sscanf(line, "%15s", cmd) != 1) continue;
        if (strcmp(cmd, "TODO") == 0) {
            char *p = line + 5;
            while (*p == ' ') p++;
            char *title = p;
            char *space = strchr(title, ' ');
            if (!space) continue;
            *space = '\0';
            char *note = space + 1;
            while (*note == ' ') note++;
            todo_task(title, note);
        } else if (strcmp(cmd, "START") == 0) {
            char *p = line + 6;
            while (*p == ' ') p++;
            char *title = p;
            char *space = strchr(title, ' ');
            if (space) *space = '\0';
            start_task(title);
        } else if (strcmp(cmd, "DONE") == 0) {
            char *p = line + 5;
            while (*p == ' ') p++;
            char *title = p;
            char *space = strchr(title, ' ');
            if (space) *space = '\0';
            done_task(title);
        } else if (strcmp(cmd, "REMOVE") == 0) {
            char *p = line + 7;
            while (*p == ' ') p++;
            char *title = p;
            char *space = strchr(title, ' ');
            if (space) *space = '\0';
            remove_task(title);
        } else if (strcmp(cmd, "FIND") == 0) {
            char *p = line + 5;
            while (*p == ' ') p++;
            char *title = p;
            char *space = strchr(title, ' ');
            if (space) *space = '\0';
            find_task_print(title);
        } else if (strcmp(cmd, "REPORT") == 0) {
            report_all();
        }
    }
    free_all();
    return 0;
}