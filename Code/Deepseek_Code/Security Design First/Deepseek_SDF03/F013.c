// F013.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_LINE 4096
#define MAX_TITLE 64
#define MAX_NOTE 4096

typedef struct Task {
    char title[MAX_TITLE];
    char note[MAX_NOTE];
    struct Task *next;
} Task;

typedef struct List {
    char name[16];
    Task *head;
    struct List *next;
} List;

List *todo = NULL;
List *doing = NULL;
List *done = NULL;

List *create_list(const char *name) {
    List *l = malloc(sizeof(List));
    if (!l) return NULL;
    strcpy(l->name, name);
    l->head = NULL;
    l->next = NULL;
    return l;
}

List *find_list(const char *name) {
    if (strcmp(name, "todo") == 0) return todo;
    if (strcmp(name, "doing") == 0) return doing;
    if (strcmp(name, "done") == 0) return done;
    return NULL;
}

Task *find_task(List *list, const char *title) {
    Task *t = list->head;
    while (t) {
        if (strcmp(t->title, title) == 0) return t;
        t = t->next;
    }
    return NULL;
}

Task *find_task_any(const char *title, List **out_list) {
    List *lists[3] = {todo, doing, done};
    for (int i = 0; i < 3; i++) {
        Task *t = find_task(lists[i], title);
        if (t) { *out_list = lists[i]; return t; }
    }
    return NULL;
}

void remove_task_from_list(List *list, const char *title) {
    Task *t = list->head, *prev = NULL;
    while (t) {
        if (strcmp(t->title, title) == 0) {
            if (prev) prev->next = t->next;
            else list->head = t->next;
            free(t);
            return;
        }
        prev = t;
        t = t->next;
    }
}

void free_list(List *list) {
    Task *t = list->head;
    while (t) {
        Task *tmp = t;
        t = t->next;
        free(tmp);
    }
    free(list);
}

void free_all() {
    if (todo) free_list(todo);
    if (doing) free_list(doing);
    if (done) free_list(done);
    todo = doing = done = NULL;
}

void handle_todo(char *line) {
    char title[MAX_TITLE], note[MAX_NOTE];
    int n = sscanf(line, "TODO %63s %4095[^\n]", title, note);
    if (n < 2) return;
    if (!todo) todo = create_list("todo");
    if (!todo) return;
    if (find_task(todo, title)) return;
    Task *t = malloc(sizeof(Task));
    if (!t) return;
    strcpy(t->title, title);
    strcpy(t->note, note);
    t->next = todo->head;
    todo->head = t;
}

void move_task(const char *title, List *from, List *to) {
    Task *t = find_task(from, title);
    if (!t) return;
    // Remove from from
    Task *prev = NULL;
    Task *cur = from->head;
    while (cur) {
        if (cur == t) {
            if (prev) prev->next = cur->next;
            else from->head = cur->next;
            break;
        }
        prev = cur;
        cur = cur->next;
    }
    // Add to to
    t->next = to->head;
    to->head = t;
}

void handle_start(char *line) {
    char title[MAX_TITLE];
    if (sscanf(line, "START %63s", title) != 1) return;
    if (!todo || !doing) return;
    if (!find_task(todo, title)) return;
    move_task(title, todo, doing);
}

void handle_done(char *line) {
    char title[MAX_TITLE];
    if (sscanf(line, "DONE %63s", title) != 1) return;
    if (!doing || !done) return;
    if (!find_task(doing, title)) return;
    move_task(title, doing, done);
}

void handle_remove(char *line) {
    char title[MAX_TITLE];
    if (sscanf(line, "REMOVE %63s", title) != 1) return;
    List *lists[3] = {todo, doing, done};
    for (int i = 0; i < 3; i++) {
        if (lists[i] && find_task(lists[i], title)) {
            remove_task_from_list(lists[i], title);
            return;
        }
    }
}

void handle_find(char *line) {
    char title[MAX_TITLE];
    if (sscanf(line, "FIND %63s", title) != 1) return;
    List *found_list = NULL;
    Task *t = find_task_any(title, &found_list);
    if (!t) return;
    printf("%s %s\n", found_list->name, t->note);
}

void handle_report() {
    List *lists[3] = {todo, doing, done};
    const char *names[3] = {"todo", "doing", "done"};
    for (int i = 0; i < 3; i++) {
        printf("%s\n", names[i]);
        Task *t = lists[i] ? lists[i]->head : NULL;
        while (t) {
            printf("  %s %s\n", t->title, t->note);
            t = t->next;
        }
    }
}

int main() {
    todo = create_list("todo");
    doing = create_list("doing");
    done = create_list("done");
    if (!todo || !doing || !done) return 1;
    char line[MAX_LINE];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        if (strncmp(line, "TODO ", 5) == 0) handle_todo(line);
        else if (strncmp(line, "START ", 6) == 0) handle_start(line);
        else if (strncmp(line, "DONE ", 5) == 0) handle_done(line);
        else if (strncmp(line, "REMOVE ", 7) == 0) handle_remove(line);
        else if (strncmp(line, "FIND ", 5) == 0) handle_find(line);
        else if (strcmp(line, "REPORT") == 0) handle_report();
    }
    free_all();
    return 0;
}