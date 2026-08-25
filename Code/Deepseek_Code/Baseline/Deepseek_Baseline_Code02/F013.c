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

Task *find_task(const char *title, Task **list) {
    Task *t = todo;
    while (t) { if (strcmp(t->title, title)==0) { *list = todo; return t; } t = t->next; }
    t = doing;
    while (t) { if (strcmp(t->title, title)==0) { *list = doing; return t; } t = t->next; }
    t = done;
    while (t) { if (strcmp(t->title, title)==0) { *list = done; return t; } t = t->next; }
    *list = NULL;
    return NULL;
}

void add_task(const char *title, const char *note, Task **head, Task **tail) {
    // we add to todo
    Task *t = malloc(sizeof(Task));
    t->title = malloc(strlen(title) + 1);
    strcpy(t->title, title);
    t->note = malloc(strlen(note) + 1);
    strcpy(t->note, note);
    t->next = NULL;
    if (*tail) (*tail)->next = t;
    else *head = t;
    *tail = t;
}

void move_task(const char *title, Task **from_head, Task **from_tail, Task **to_head, Task **to_tail) {
    Task *prev = NULL;
    Task *t = *from_head;
    while (t) {
        if (strcmp(t->title, title) == 0) {
            if (prev) prev->next = t->next;
            else *from_head = t->next;
            if (*from_tail == t) *from_tail = prev;
            t->next = NULL;
            if (*to_tail) (*to_tail)->next = t;
            else *to_head = t;
            *to_tail = t;
            return;
        }
        prev = t;
        t = t->next;
    }
}

void start_task(const char *title) {
    Task *list;
    if (find_task(title, &list) == NULL) return;
    if (list == todo) {
        move_task(title, &todo, &todo_tail, &doing, &doing_tail);
    } else if (list == done) {
        // can't start from done? spec says moves to doing from whichever list contains it
        move_task(title, &done, &done_tail, &doing, &doing_tail);
    } else {
        // already doing? ignore
    }
}

void done_task(const char *title) {
    Task *list;
    if (find_task(title, &list) == NULL) return;
    if (list == todo) {
        move_task(title, &todo, &todo_tail, &done, &done_tail);
    } else if (list == doing) {
        move_task(title, &doing, &doing_tail, &done, &done_tail);
    } else {
        // already done? ignore
    }
}

void remove_task(const char *title) {
    Task *list;
    Task *t = find_task(title, &list);
    if (!t) return;
    Task *prev = NULL;
    Task *cur = list;
    while (cur) {
        if (cur == t) {
            if (prev) prev->next = cur->next;
            else list = cur->next;
            if (list == todo && todo_tail == cur) todo_tail = prev;
            else if (list == doing && doing_tail == cur) doing_tail = prev;
            else if (list == done && done_tail == cur) done_tail = prev;
            free(cur->title);
            free(cur->note);
            free(cur);
            return;
        }
        prev = cur;
        cur = cur->next;
    }
}

void find_task_print(const char *title) {
    Task *list;
    Task *t = find_task(title, &list);
    if (!t) return;
    if (list == todo) printf("todo ");
    else if (list == doing) printf("doing ");
    else printf("done ");
    for (char *c = t->note; *c; c++) {
        putchar(*c == ' ' ? '_' : *c);
    }
    putchar('\n');
}

void report_all(void) {
    Task *t;
    printf("todo:");
    t = todo;
    while (t) { printf(" %s", t->title); t = t->next; }
    putchar('\n');
    printf("doing:");
    t = doing;
    while (t) { printf(" %s", t->title); t = t->next; }
    putchar('\n');
    printf("done:");
    t = done;
    while (t) { printf(" %s", t->title); t = t->next; }
    putchar('\n');
}

Task *todo_tail = NULL;
Task *doing_tail = NULL;
Task *done_tail = NULL;

void free_all(void) {
    Task *t;
    while (todo) { t = todo->next; free(todo->title); free(todo->note); free(todo); todo = t; }
    while (doing) { t = doing->next; free(doing->title); free(doing->note); free(doing); doing = t; }
    while (done) { t = done->next; free(done->title); free(done->note); free(done); done = t; }
}

int main() {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        if (strcmp(line, "END") == 0) break;
        char cmd[16];
        if (sscanf(line, "%15s", cmd) < 1) continue;
        if (strcmp(cmd, "TODO") == 0) {
            char *p = line + 5;
            while (*p == ' ') p++;
            char *title = p;
            while (*p && *p != ' ') p++;
            if (*p) *p++ = 0;
            while (*p == ' ') p++;
            add_task(title, p, &todo, &todo_tail);
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
            find_task_print(p);
        } else if (strcmp(cmd, "REPORT") == 0) {
            report_all();
        }
    }
    free_all();
    return 0;
}