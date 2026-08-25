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

Task *find_in_list(Task *list, const char *title) {
    Task *curr = list;
    while (curr) {
        if (strcmp(curr->title, title) == 0) return curr;
        curr = curr->next;
    }
    return NULL;
}

void add_to_list(Task **list, const char *title, const char *note) {
    Task *t = malloc(sizeof(Task));
    if (!t) exit(EXIT_FAILURE);
    t->title = strdup(title);
    t->note = strdup(note);
    t->next = NULL;
    if (!*list) {
        *list = t;
    } else {
        Task *curr = *list;
        while (curr->next) curr = curr->next;
        curr->next = t;
    }
}

void remove_from_list(Task **list, const char *title) {
    Task *curr = *list;
    Task *prev = NULL;
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

Task *extract_from_list(Task **list, const char *title) {
    Task *curr = *list;
    Task *prev = NULL;
    while (curr) {
        if (strcmp(curr->title, title) == 0) {
            if (prev) prev->next = curr->next;
            else *list = curr->next;
            curr->next = NULL;
            return curr;
        }
        prev = curr;
        curr = curr->next;
    }
    return NULL;
}

void append_to_list(Task **list, Task *t) {
    if (!*list) {
        *list = t;
    } else {
        Task *curr = *list;
        while (curr->next) curr = curr->next;
        curr->next = t;
    }
}

void todo_task(const char *title, const char *note) {
    add_to_list(&todo, title, note);
}

void start_task(const char *title) {
    Task *t = extract_from_list(&todo, title);
    if (t) append_to_list(&doing, t);
}

void done_task(const char *title) {
    Task *t = extract_from_list(&doing, title);
    if (t) append_to_list(&done, t);
}

void remove_task(const char *title) {
    remove_from_list(&todo, title);
    remove_from_list(&doing, title);
    remove_from_list(&done, title);
}

void find_task(const char *title) {
    Task *t = find_in_list(todo, title);
    if (t) {
        printf("todo %s\n", t->note);
        return;
    }
    t = find_in_list(doing, title);
    if (t) {
        printf("doing %s\n", t->note);
        return;
    }
    t = find_in_list(done, title);
    if (t) {
        printf("done %s\n", t->note);
    }
}

void print_list(Task *list) {
    Task *curr = list;
    while (curr) {
        printf("%s %s\n", curr->title, curr->note);
        curr = curr->next;
    }
}

void report_tasks() {
    print_list(todo);
    print_list(doing);
    print_list(done);
}

void free_list(Task *list) {
    Task *curr = list;
    while (curr) {
        Task *next = curr->next;
        free(curr->title);
        free(curr->note);
        free(curr);
        curr = next;
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
        line[strcspn(line, "\n")] = 0;
        if (strcmp(line, "END") == 0) break;
        
        char cmd[32] = {0};
        char title[256] = {0};
        char note[3800] = {0};
        
        int matched = sscanf(line, "%31s %255s %[^\n]", cmd, title, note);
        
        if (strcmp(cmd, "TODO") == 0 && matched == 3) {
            todo_task(title, note);
        } else if (strcmp(cmd, "START") == 0) {
            start_task(title);
        } else if (strcmp(cmd, "DONE") == 0) {
            done_task(title);
        } else if (strcmp(cmd, "REMOVE") == 0) {
            remove_task(title);
        } else if (strcmp(cmd, "FIND") == 0) {
            find_task(title);
        } else if (strcmp(cmd, "REPORT") == 0) {
            report_tasks();
        }
    }
    free_all();
    return 0;
}