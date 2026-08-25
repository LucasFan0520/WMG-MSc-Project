// F013.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Task {
    char *title;
    char *note;
    struct Task *next;
} Task;

typedef struct List {
    char *name;
    Task *head;
    Task *tail;
} List;

List lists[3]; // TODO, DOING, DONE

Task *find_task_in_list(List *list, const char *title) {
    Task *t = list->head;
    while (t) {
        if (strcmp(t->title, title) == 0) return t;
        t = t->next;
    }
    return NULL;
}

Task *find_task_any(const char *title, int *list_index) {
    for (int i = 0; i < 3; i++) {
        Task *t = find_task_in_list(&lists[i], title);
        if (t) { *list_index = i; return t; }
    }
    return NULL;
}

void add_task_to_list(List *list, const char *title, const char *note) {
    Task *t = malloc(sizeof(Task));
    t->title = malloc(strlen(title) + 1);
    strcpy(t->title, title);
    t->note = malloc(strlen(note) + 1);
    strcpy(t->note, note);
    t->next = NULL;
    if (list->tail) {
        list->tail->next = t;
        list->tail = t;
    } else {
        list->head = list->tail = t;
    }
}

void remove_task_from_list(List *list, const char *title) {
    Task *prev = NULL, *curr = list->head;
    while (curr) {
        if (strcmp(curr->title, title) == 0) {
            if (prev) prev->next = curr->next;
            else list->head = curr->next;
            if (curr == list->tail) list->tail = prev;
            free(curr->title);
            free(curr->note);
            free(curr);
            return;
        }
        prev = curr;
        curr = curr->next;
    }
}

void move_task(int from, int to, const char *title) {
    Task *t = find_task_in_list(&lists[from], title);
    if (!t) return;
    char *note = t->note;
    char *title_copy = strdup(t->title);
    remove_task_from_list(&lists[from], title);
    add_task_to_list(&lists[to], title_copy, note);
    free(title_copy);
}

void todo_task(const char *title, const char *note) {
    add_task_to_list(&lists[0], title, note);
}

void start_task(const char *title) {
    int idx;
    if (find_task_any(title, &idx) && idx == 0) {
        move_task(0, 1, title);
    }
}

void done_task(const char *title) {
    int idx;
    if (find_task_any(title, &idx) && idx == 1) {
        move_task(1, 2, title);
    }
}

void remove_task_any(const char *title) {
    int idx;
    if (find_task_any(title, &idx)) {
        remove_task_from_list(&lists[idx], title);
    }
}

void find_task_print(const char *title) {
    int idx;
    Task *t = find_task_any(title, &idx);
    if (t) {
        printf("%s %s\n", lists[idx].name, t->note);
    }
}

void report_lists(void) {
    for (int i = 0; i < 3; i++) {
        printf("%s\n", lists[i].name);
        Task *t = lists[i].head;
        while (t) {
            printf("%s %s\n", t->title, t->note);
            t = t->next;
        }
    }
}

void free_all(void) {
    for (int i = 0; i < 3; i++) {
        Task *t = lists[i].head;
        while (t) {
            Task *next = t->next;
            free(t->title);
            free(t->note);
            free(t);
            t = next;
        }
        lists[i].head = lists[i].tail = NULL;
    }
}

int main(void) {
    lists[0].name = "TODO";
    lists[1].name = "DOING";
    lists[2].name = "DONE";
    lists[0].head = lists[0].tail = NULL;
    lists[1].head = lists[1].tail = NULL;
    lists[2].head = lists[2].tail = NULL;

    char *line = NULL;
    size_t len = 0;
    ssize_t read;
    while ((read = getline(&line, &len, stdin)) != -1) {
        if (line[read - 1] == '\n') line[read - 1] = '\0';
        if (strcmp(line, "END") == 0) break;
        char *cmd = strtok(line, " ");
        if (!cmd) continue;
        if (strcmp(cmd, "TODO") == 0) {
            char *title = strtok(NULL, " ");
            char *note = strtok(NULL, "");
            if (title && note) todo_task(title, note);
        } else if (strcmp(cmd, "START") == 0) {
            char *title = strtok(NULL, " ");
            if (title) start_task(title);
        } else if (strcmp(cmd, "DONE") == 0) {
            char *title = strtok(NULL, " ");
            if (title) done_task(title);
        } else if (strcmp(cmd, "REMOVE") == 0) {
            char *title = strtok(NULL, " ");
            if (title) remove_task_any(title);
        } else if (strcmp(cmd, "FIND") == 0) {
            char *title = strtok(NULL, " ");
            if (title) find_task_print(title);
        } else if (strcmp(cmd, "REPORT") == 0) {
            report_lists();
        }
    }
    free(line);
    free_all();
    return 0;
}