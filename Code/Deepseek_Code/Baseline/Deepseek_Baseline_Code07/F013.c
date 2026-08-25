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
    Task *lists[3] = {todo, doing, done};
    for (int i=0; i<3; i++) {
        Task *t = lists[i];
        while (t) {
            if (strcmp(t->title, title) == 0) {
                *list_ptr = lists[i];
                return t;
            }
            t = t->next;
        }
    }
    return NULL;
}

void add_task(Task **list, const char *title, const char *note) {
    Task *t = malloc(sizeof(Task));
    t->title = malloc(strlen(title)+1);
    strcpy(t->title, title);
    t->note = malloc(strlen(note)+1);
    strcpy(t->note, note);
    t->next = *list;
    *list = t;
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

void move_task(Task **from, Task **to, const char *title) {
    Task *t = find_task(title, from);
    if (!t || *from != t) return; // t must be in that list
    remove_task_from_list(from, title);
    // re-add to destination
    Task *new = malloc(sizeof(Task));
    new->title = malloc(strlen(t->title)+1);
    strcpy(new->title, t->title);
    new->note = malloc(strlen(t->note)+1);
    strcpy(new->note, t->note);
    new->next = *to;
    *to = new;
}

void todo_task(const char *title, const char *note) {
    add_task(&todo, title, note);
}

void start_task(const char *title) {
    Task *dummy;
    Task *t = find_task(title, &dummy);
    if (!t) return;
    // find which list it's in
    Task *list_ptr = NULL;
    find_task(title, &list_ptr);
    if (list_ptr == &todo) {
        remove_task_from_list(&todo, title);
        add_task(&doing, title, t->note);
    } else if (list_ptr == &doing) {
        // already doing? ignore or move? spec says move to doing from todo only? Actually "START title moves it to doing" from whatever list? Probably from todo, but if already doing, ignore.
        // We'll just do nothing if not in todo.
    }
}

void done_task(const char *title) {
    Task *list_ptr;
    Task *t = find_task(title, &list_ptr);
    if (!t) return;
    if (list_ptr == &todo || list_ptr == &doing) {
        remove_task_from_list(list_ptr, title);
        add_task(&done, title, t->note);
    }
    // if already done, ignore
}

void remove_task(const char *title) {
    Task *list_ptr;
    if (find_task(title, &list_ptr)) {
        remove_task_from_list(list_ptr, title);
    }
}

void find_task_print(const char *title) {
    Task *list_ptr;
    Task *t = find_task(title, &list_ptr);
    if (!t) return;
    char *list_name = "todo";
    if (list_ptr == &doing) list_name = "doing";
    else if (list_ptr == &done) list_name = "done";
    printf("%s %s\n", list_name, t->note);
}

void report_lists(void) {
    printf("todo:\n");
    Task *t = todo;
    while (t) { printf("%s %s\n", t->title, t->note); t = t->next; }
    printf("doing:\n");
    t = doing;
    while (t) { printf("%s %s\n", t->title, t->note); t = t->next; }
    printf("done:\n");
    t = done;
    while (t) { printf("%s %s\n", t->title, t->note); t = t->next; }
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
    char line[1024];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        if (strcmp(line, "END") == 0) break;
        char *cmd = strtok(line, " ");
        if (!cmd) continue;
        char *rest = strtok(NULL, "");
        if (!rest) rest = "";
        if (strcmp(cmd, "TODO") == 0) {
            char *title = strtok(rest, " ");
            if (!title) continue;
            char *note = strtok(NULL, "");
            if (!note) note = "";
            todo_task(title, note);
        } else if (strcmp(cmd, "START") == 0) {
            char *title = strtok(rest, " ");
            if (title) start_task(title);
        } else if (strcmp(cmd, "DONE") == 0) {
            char *title = strtok(rest, " ");
            if (title) done_task(title);
        } else if (strcmp(cmd, "REMOVE") == 0) {
            char *title = strtok(rest, " ");
            if (title) remove_task(title);
        } else if (strcmp(cmd, "FIND") == 0) {
            char *title = strtok(rest, " ");
            if (title) find_task_print(title);
        } else if (strcmp(cmd, "REPORT") == 0) {
            report_lists();
        }
    }
    free_all();
    return 0;
}