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

Task* find_task(Task *list, const char *title) {
    Task *cur = list;
    while (cur) {
        if (strcmp(cur->title, title) == 0) return cur;
        cur = cur->next;
    }
    return NULL;
}

void add_task(Task **list, const char *title, const char *note) {
    if (find_task(*list, title)) return;
    Task *new = malloc(sizeof(Task));
    new->title = strdup(title);
    new->note = strdup(note);
    new->next = NULL;
    if (!*list) *list = new;
    else {
        Task *cur = *list;
        while (cur->next) cur = cur->next;
        cur->next = new;
    }
}

void move_task(Task **from, Task **to, const char *title) {
    Task *prev = NULL, *cur = *from;
    while (cur) {
        if (strcmp(cur->title, title) == 0) {
            if (prev) prev->next = cur->next;
            else *from = cur->next;
            cur->next = NULL;
            if (!*to) *to = cur;
            else {
                Task *t = *to;
                while (t->next) t = t->next;
                t->next = cur;
            }
            return;
        }
        prev = cur;
        cur = cur->next;
    }
}

void remove_task(const char *title) {
    Task **lists[] = {&todo, &doing, &done};
    for (int i = 0; i < 3; i++) {
        Task *prev = NULL, *cur = *lists[i];
        while (cur) {
            if (strcmp(cur->title, title) == 0) {
                if (prev) prev->next = cur->next;
                else *lists[i] = cur->next;
                free(cur->title);
                free(cur->note);
                free(cur);
                return;
            }
            prev = cur;
            cur = cur->next;
        }
    }
}

void find(const char *title) {
    Task **lists[] = {&todo, &doing, &done};
    const char *names[] = {"todo", "doing", "done"};
    for (int i = 0; i < 3; i++) {
        Task *t = find_task(*lists[i], title);
        if (t) {
            printf("%s %s\n", names[i], t->note);
            return;
        }
    }
}

void print_list(Task *list, const char *name) {
    Task *cur = list;
    while (cur) {
        printf("%s %s\n", name, cur->note);
        cur = cur->next;
    }
}

void free_list(Task *list) {
    Task *cur = list;
    while (cur) {
        Task *tmp = cur;
        cur = cur->next;
        free(tmp->title);
        free(tmp->note);
        free(tmp);
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
        char *cmd = strtok(line, " ");
        if (!cmd) continue;
        if (strcmp(cmd, "TODO") == 0) {
            char *title = strtok(NULL, " ");
            if (!title) continue;
            char *note = strtok(NULL, "");
            if (!note) note = "";
            add_task(&todo, title, note);
        } else if (strcmp(cmd, "START") == 0) {
            char *title = strtok(NULL, " ");
            if (title) move_task(&todo, &doing, title);
        } else if (strcmp(cmd, "DONE") == 0) {
            char *title = strtok(NULL, " ");
            if (title) move_task(&doing, &done, title);
        } else if (strcmp(cmd, "REMOVE") == 0) {
            char *title = strtok(NULL, " ");
            if (title) remove_task(title);
        } else if (strcmp(cmd, "FIND") == 0) {
            char *title = strtok(NULL, " ");
            if (title) find(title);
        } else if (strcmp(cmd, "REPORT") == 0) {
            print_list(todo, "todo");
            print_list(doing, "doing");
            print_list(done, "done");
        }
    }
    free_all();
    return 0;
}