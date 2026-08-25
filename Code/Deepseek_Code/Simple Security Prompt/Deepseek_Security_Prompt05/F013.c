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
    Task *t;
    t = todo; while (t) { if (strcmp(t->title, title)==0) { *list = todo; return t; } t = t->next; }
    t = doing; while (t) { if (strcmp(t->title, title)==0) { *list = doing; return t; } t = t->next; }
    t = done; while (t) { if (strcmp(t->title, title)==0) { *list = done; return t; } t = t->next; }
    return NULL;
}

void remove_from_list(Task **list, Task *task) {
    Task *p = *list, *prev = NULL;
    while (p) {
        if (p == task) {
            if (prev) prev->next = p->next;
            else *list = p->next;
            return;
        }
        prev = p;
        p = p->next;
    }
}

void add_task(Task **list, const char *title, const char *note) {
    Task *t = malloc(sizeof(Task));
    if (!t) return;
    t->title = strdup(title);
    t->note = strdup(note);
    t->next = *list;
    *list = t;
}

void todo_task(const char *title, const char *note) {
    if (find_task(title, &(Task*){0})) return;
    add_task(&todo, title, note);
}

void start_task(const char *title) {
    Task *list = NULL;
    Task *t = find_task(title, &list);
    if (!t || list != todo) return;
    remove_from_list(&todo, t);
    t->next = doing;
    doing = t;
}

void done_task(const char *title) {
    Task *list = NULL;
    Task *t = find_task(title, &list);
    if (!t || list != doing) return;
    remove_from_list(&doing, t);
    t->next = done;
    done = t;
}

void remove_task(const char *title) {
    Task *list = NULL;
    Task *t = find_task(title, &list);
    if (!t) return;
    remove_from_list(&list, t);
    free(t->title);
    free(t->note);
    free(t);
}

void find_task_print(const char *title) {
    Task *list = NULL;
    Task *t = find_task(title, &list);
    if (!t) {
        printf("NOT_FOUND\n");
        return;
    }
    const char *listname = (list == todo) ? "todo" : (list == doing) ? "doing" : "done";
    printf("%s %s\n", listname, t->note);
}

void report_all(void) {
    Task *t = todo;
    while (t) { printf("todo %s %s\n", t->title, t->note); t = t->next; }
    t = doing;
    while (t) { printf("doing %s %s\n", t->title, t->note); t = t->next; }
    t = done;
    while (t) { printf("done %s %s\n", t->title, t->note); t = t->next; }
}

int main(void) {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        char cmd[16], arg1[256], arg2[2048];
        int n = sscanf(line, "%15s %255s %2047s", cmd, arg1, arg2);
        if (n < 2) continue;
        if (strcmp(cmd, "TODO") == 0) {
            char *note = line + strlen(cmd) + strlen(arg1) + 2;
            while (*note == ' ') note++;
            todo_task(arg1, note);
        } else if (strcmp(cmd, "START") == 0) {
            start_task(arg1);
        } else if (strcmp(cmd, "DONE") == 0) {
            done_task(arg1);
        } else if (strcmp(cmd, "REMOVE") == 0) {
            remove_task(arg1);
        } else if (strcmp(cmd, "FIND") == 0) {
            find_task_print(arg1);
        } else if (strcmp(cmd, "REPORT") == 0) {
            report_all();
        }
    }
    while (todo) { Task *t = todo; todo = todo->next; free(t->title); free(t->note); free(t); }
    while (doing) { Task *t = doing; doing = doing->next; free(t->title); free(t->note); free(t); }
    while (done) { Task *t = done; done = done->next; free(t->title); free(t->note); free(t); }
    return 0;
}