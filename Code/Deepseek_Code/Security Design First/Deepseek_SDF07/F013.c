// F013.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_LINE 4096

typedef struct Task {
    char *title;
    char *note;
    struct Task *next;
} Task;

typedef struct {
    Task *head;
    Task *tail;
    size_t count;
} TaskList;

typedef struct {
    TaskList todo;
    TaskList doing;
    TaskList done;
} Board;

static void trim_newline(char *s) {
    size_t len = strlen(s);
    if (len && s[len-1] == '\n') s[len-1] = '\0';
}

static char *safe_strdup(const char *s) {
    if (!s) return NULL;
    size_t len = strlen(s);
    char *p = malloc(len + 1);
    if (p) memcpy(p, s, len + 1);
    return p;
}

static void tl_init(TaskList *tl) {
    tl->head = NULL;
    tl->tail = NULL;
    tl->count = 0;
}

static void tl_free(TaskList *tl) {
    Task *cur = tl->head;
    while (cur) {
        Task *next = cur->next;
        free(cur->title);
        free(cur->note);
        free(cur);
        cur = next;
    }
    tl->head = NULL;
    tl->tail = NULL;
    tl->count = 0;
}

static Task *tl_find(TaskList *tl, const char *title) {
    Task *cur = tl->head;
    while (cur) {
        if (strcmp(cur->title, title) == 0)
            return cur;
        cur = cur->next;
    }
    return NULL;
}

static void tl_add(TaskList *tl, const char *title, const char *note) {
    Task *t = malloc(sizeof(Task));
    if (!t) return;
    t->title = safe_strdup(title);
    t->note = safe_strdup(note);
    t->next = NULL;
    if (!t->title || !t->note) {
        free(t->title);
        free(t->note);
        free(t);
        return;
    }
    if (tl->tail)
        tl->tail->next = t;
    else
        tl->head = t;
    tl->tail = t;
    tl->count++;
}

static Task *tl_remove(TaskList *tl, const char *title) {
    Task *prev = NULL;
    Task *cur = tl->head;
    while (cur) {
        if (strcmp(cur->title, title) == 0) {
            if (prev)
                prev->next = cur->next;
            else
                tl->head = cur->next;
            if (cur == tl->tail)
                tl->tail = prev;
            tl->count--;
            return cur;
        }
        prev = cur;
        cur = cur->next;
    }
    return NULL;
}

static void board_init(Board *b) {
    tl_init(&b->todo);
    tl_init(&b->doing);
    tl_init(&b->done);
}

static void board_free(Board *b) {
    tl_free(&b->todo);
    tl_free(&b->doing);
    tl_free(&b->done);
}

static Task *board_find(Board *b, const char *title, char *listname) {
    Task *t;
    t = tl_find(&b->todo, title);
    if (t) { strcpy(listname, "todo"); return t; }
    t = tl_find(&b->doing, title);
    if (t) { strcpy(listname, "doing"); return t; }
    t = tl_find(&b->done, title);
    if (t) { strcpy(listname, "done"); return t; }
    return NULL;
}

static void board_todo(Board *b, const char *title, const char *note) {
    if (board_find(b, title, (char[8]){0})) return; // already exists
    tl_add(&b->todo, title, note);
}

static void board_start(Board *b, const char *title) {
    char list[8];
    Task *t = board_find(b, title, list);
    if (!t || strcmp(list, "todo") != 0) return;
    Task *removed = tl_remove(&b->todo, title);
    if (removed) {
        tl_add(&b->doing, removed->title, removed->note);
        free(removed->title);
        free(removed->note);
        free(removed);
    }
}

static void board_done(Board *b, const char *title) {
    char list[8];
    Task *t = board_find(b, title, list);
    if (!t || strcmp(list, "doing") != 0) return;
    Task *removed = tl_remove(&b->doing, title);
    if (removed) {
        tl_add(&b->done, removed->title, removed->note);
        free(removed->title);
        free(removed->note);
        free(removed);
    }
}

static void board_remove(Board *b, const char *title) {
    char list[8];
    Task *t = board_find(b, title, list);
    if (!t) return;
    if (strcmp(list, "todo") == 0)
        tl_remove(&b->todo, title);
    else if (strcmp(list, "doing") == 0)
        tl_remove(&b->doing, title);
    else
        tl_remove(&b->done, title);
    free(t->title);
    free(t->note);
    free(t);
}

static void board_find_print(Board *b, const char *title) {
    char list[8];
    Task *t = board_find(b, title, list);
    if (!t) return;
    printf("%s ", list);
    const char *p = t->note;
    while (*p) {
        putchar(*p == ' ' ? '_' : *p);
        p++;
    }
    putchar('\n');
}

static void board_report(Board *b) {
    printf("todo\n");
    Task *cur = b->todo.head;
    while (cur) {
        printf("%s\n", cur->title);
        cur = cur->next;
    }
    printf("doing\n");
    cur = b->doing.head;
    while (cur) {
        printf("%s\n", cur->title);
        cur = cur->next;
    }
    printf("done\n");
    cur = b->done.head;
    while (cur) {
        printf("%s\n", cur->title);
        cur = cur->next;
    }
}

static void parse_cmd(const char *line, char *cmd, size_t cmdsz,
                      char *a1, size_t a1sz, char *a2, size_t a2sz) {
    const char *p = line;
    while (isspace(*p)) p++;
    size_t i = 0;
    while (*p && !isspace(*p) && i < cmdsz - 1) cmd[i++] = *p++;
    cmd[i] = '\0';
    while (isspace(*p)) p++;
    i = 0;
    while (*p && !isspace(*p) && i < a1sz - 1) a1[i++] = *p++;
    a1[i] = '\0';
    while (isspace(*p)) p++;
    i = 0;
    while (*p && i < a2sz - 1) a2[i++] = *p++;
    a2[i] = '\0';
}

int main(void) {
    Board b;
    board_init(&b);
    char line[MAX_LINE];
    while (fgets(line, sizeof(line), stdin)) {
        trim_newline(line);
        if (strcmp(line, "END") == 0) break;
        char cmd[32], a1[256], a2[4096];
        parse_cmd(line, cmd, sizeof(cmd), a1, sizeof(a1), a2, sizeof(a2));
        if (strcmp(cmd, "TODO") == 0) {
            board_todo(&b, a1, a2);
        } else if (strcmp(cmd, "START") == 0) {
            board_start(&b, a1);
        } else if (strcmp(cmd, "DONE") == 0) {
            board_done(&b, a1);
        } else if (strcmp(cmd, "REMOVE") == 0) {
            board_remove(&b, a1);
        } else if (strcmp(cmd, "FIND") == 0) {
            board_find_print(&b, a1);
        } else if (strcmp(cmd, "REPORT") == 0) {
            board_report(&b);
        }
    }
    board_free(&b);
    return 0;
}