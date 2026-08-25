// F022.c
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
    char *name;
    Task *head;
    size_t count;
} Project;

typedef struct {
    Project *items;
    size_t count;
    size_t capacity;
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

static void task_free(Task *head) {
    while (head) {
        Task *next = head->next;
        free(head->title);
        free(head->note);
        free(head);
        head = next;
    }
}

static void project_free(Project *p) {
    free(p->name);
    task_free(p->head);
    p->head = NULL;
    p->count = 0;
}

static void board_init(Board *b) {
    b->items = NULL;
    b->count = 0;
    b->capacity = 0;
}

static void board_free(Board *b) {
    for (size_t i = 0; i < b->count; i++)
        project_free(&b->items[i]);
    free(b->items);
    b->items = NULL;
    b->count = 0;
    b->capacity = 0;
}

static int board_find(Board *b, const char *name) {
    for (size_t i = 0; i < b->count; i++) {
        if (strcmp(b->items[i].name, name) == 0)
            return (int)i;
    }
    return -1;
}

static Task *board_find_task(Board *b, int idx, const char *title) {
    Task *cur = b->items[idx].head;
    while (cur) {
        if (strcmp(cur->title, title) == 0)
            return cur;
        cur = cur->next;
    }
    return NULL;
}

static void board_project(Board *b, const char *name) {
    if (board_find(b, name) >= 0) return;
    if (b->count == b->capacity) {
        size_t newcap = b->capacity ? b->capacity * 2 : 4;
        Project *tmp = realloc(b->items, newcap * sizeof(Project));
        if (!tmp) return;
        b->items = tmp;
        b->capacity = newcap;
    }
    b->items[b->count].name = safe_strdup(name);
    b->items[b->count].head = NULL;
    b->items[b->count].count = 0;
    if (b->items[b->count].name)
        b->count++;
    else
        free(b->items[b->count].name);
}

static void board_task(Board *b, const char *proj, const char *title, const char *note) {
    int idx = board_find(b, proj);
    if (idx < 0) return;
    if (board_find_task(b, idx, title)) return;
    Task *t = malloc(sizeof(Task));
    if (!t) return;
    t->title = safe_strdup(title);
    t->note = safe_strdup(note);
    t->next = b->items[idx].head;
    if (t->title && t->note) {
        b->items[idx].head = t;
        b->items[idx].count++;
    } else {
        free(t->title);
        free(t->note);
        free(t);
    }
}

static void board_movetask(Board *b, const char *from, const char *to, const char *title) {
    int fi = board_find(b, from);
    int ti = board_find(b, to);
    if (fi < 0 || ti < 0) return;
    Task **ptr = &b->items[fi].head;
    Task *found = NULL;
    while (*ptr) {
        if (strcmp((*ptr)->title, title) == 0) {
            found = *ptr;
            *ptr = found->next;
            b->items[fi].count--;
            break;
        }
        ptr = &(*ptr)->next;
    }
    if (!found) return;
    found->next = b->items[ti].head;
    b->items[ti].head = found;
    b->items[ti].count++;
}

static void board_donetask(Board *b, const char *proj, const char *title) {
    int idx = board_find(b, proj);
    if (idx < 0) return;
    Task **ptr = &b->items[idx].head;
    while (*ptr) {
        if (strcmp((*ptr)->title, title) == 0) {
            Task *del = *ptr;
            *ptr = del->next;
            free(del->title);
            free(del->note);
            free(del);
            b->items[idx].count--;
            return;
        }
        ptr = &(*ptr)->next;
    }
}

static void board_deleteproject(Board *b, const char *name) {
    int idx = board_find(b, name);
    if (idx < 0) return;
    project_free(&b->items[idx]);
    for (size_t i = idx; i < b->count - 1; i++)
        b->items[i] = b->items[i+1];
    b->count--;
}

static void board_report(Board *b) {
    for (size_t i = 0; i < b->count; i++) {
        printf("%s:\n", b->items[i].name);
        Task *cur = b->items[i].head;
        while (cur) {
            printf("  %s ", cur->title);
            const char *p = cur->note;
            while (*p) {
                putchar(*p == ' ' ? '_' : *p);
                p++;
            }
            putchar('\n');
            cur = cur->next;
        }
    }
}

static void parse_cmd(const char *line, char *cmd, size_t cmdsz,
                      char *a1, size_t a1sz, char *a2, size_t a2sz, char *a3, size_t a3sz) {
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
    while (*p && !isspace(*p) && i < a2sz - 1) a2[i++] = *p++;
    a2[i] = '\0';
    while (isspace(*p)) p++;
    i = 0;
    while (*p && i < a3sz - 1) a3[i++] = *p++;
    a3[i] = '\0';
}

int main(void) {
    Board b;
    board_init(&b);
    char line[MAX_LINE];
    while (fgets(line, sizeof(line), stdin)) {
        trim_newline(line);
        if (strcmp(line, "END") == 0) break;
        char cmd[32], a1[256], a2[256], a3[4096];
        parse_cmd(line, cmd, sizeof(cmd), a1, sizeof(a1), a2, sizeof(a2), a3, sizeof(a3));
        if (strcmp(cmd, "PROJECT") == 0) {
            board_project(&b, a1);
        } else if (strcmp(cmd, "TASK") == 0) {
            board_task(&b, a1, a2, a3);
        } else if (strcmp(cmd, "MOVETASK") == 0) {
            board_movetask(&b, a1, a2, a3);
        } else if (strcmp(cmd, "DONETASK") == 0) {
            board_donetask(&b, a1, a2);
        } else if (strcmp(cmd, "DELETEPROJECT") == 0) {
            board_deleteproject(&b, a1);
        } else if (strcmp(cmd, "REPORT") == 0) {
            board_report(&b);
        }
    }
    board_free(&b);
    return 0;
}