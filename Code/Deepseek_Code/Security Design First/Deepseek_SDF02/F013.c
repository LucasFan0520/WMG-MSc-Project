/* F013.c */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct task {
    char *title;
    char *note;
    struct task *next;
} task_t;

task_t *todo = NULL, *doing = NULL, *done = NULL;

task_t *find_task(const char *title, task_t **list) {
    task_t *t;
    if (todo) { t = todo; while (t) { if (strcmp(t->title, title)==0) { *list = todo; return t; } t = t->next; } }
    if (doing) { t = doing; while (t) { if (strcmp(t->title, title)==0) { *list = doing; return t; } t = t->next; } }
    if (done) { t = done; while (t) { if (strcmp(t->title, title)==0) { *list = done; return t; } t = t->next; } }
    return NULL;
}

void remove_from_list(task_t **list, const char *title) {
    task_t *prev = NULL, *t = *list;
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

void add_to_list(task_t **list, const char *title, const char *note) {
    task_t *t = malloc(sizeof(task_t));
    if (!t) return;
    t->title = strdup(title);
    t->note = strdup(note);
    t->next = *list;
    *list = t;
    if (!t->title || !t->note) {
        free(t->title); free(t->note); free(t);
    }
}

void todo_cmd(const char *title, const char *note) {
    task_t *list;
    if (find_task(title, &list)) return; // already exists
    add_to_list(&todo, title, note);
}

void start_cmd(const char *title) {
    task_t *list;
    task_t *t = find_task(title, &list);
    if (!t || list != todo) return;
    remove_from_list(&todo, title);
    add_to_list(&doing, title, t->note);
    free(t->title); free(t->note); free(t);
}

void done_cmd(const char *title) {
    task_t *list;
    task_t *t = find_task(title, &list);
    if (!t || list != doing) return;
    remove_from_list(&doing, title);
    add_to_list(&done, title, t->note);
    free(t->title); free(t->note); free(t);
}

void remove_task(const char *title) {
    task_t *list;
    if (find_task(title, &list)) {
        remove_from_list(&list, title);
    }
}

void find_cmd(const char *title) {
    task_t *list;
    task_t *t = find_task(title, &list);
    if (!t) {
        printf("NOT_FOUND\n");
        return;
    }
    const char *listname;
    if (list == todo) listname = "TODO";
    else if (list == doing) listname = "DOING";
    else listname = "DONE";
    printf("%s %s\n", listname, t->note);
}

void report_cmd(void) {
    task_t *t;
    printf("TODO:\n");
    t = todo;
    while (t) { printf("%s %s\n", t->title, t->note); t = t->next; }
    printf("DOING:\n");
    t = doing;
    while (t) { printf("%s %s\n", t->title, t->note); t = t->next; }
    printf("DONE:\n");
    t = done;
    while (t) { printf("%s %s\n", t->title, t->note); t = t->next; }
}

void free_list(task_t **list) {
    task_t *t = *list;
    while (t) {
        task_t *nxt = t->next;
        free(t->title);
        free(t->note);
        free(t);
        t = nxt;
    }
    *list = NULL;
}

int main(void) {
    char *line = NULL;
    size_t len = 0;
    while (getline(&line, &len, stdin) != -1) {
        line[strcspn(line, "\n")] = 0;
        if (strcmp(line, "END") == 0) break;
        char cmd[10], title[256];
        if (sscanf(line, "%9s %255s", cmd, title) >= 1) {
            const char *rest = line + strlen(cmd) + 1;
            if (strcmp(cmd, "TODO") == 0) {
                const char *note = rest + strlen(title) + 1;
                todo_cmd(title, note);
            } else if (strcmp(cmd, "START") == 0) {
                start_cmd(title);
            } else if (strcmp(cmd, "DONE") == 0) {
                done_cmd(title);
            } else if (strcmp(cmd, "REMOVE") == 0) {
                remove_task(title);
            } else if (strcmp(cmd, "FIND") == 0) {
                find_cmd(title);
            } else if (strcmp(cmd, "REPORT") == 0) {
                report_cmd();
            }
        }
    }
    free(line);
    free_list(&todo);
    free_list(&doing);
    free_list(&done);
    return 0;
}