// F013.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_LINE 8192

static char *safe_strdup(const char *s) {
    if (!s) return NULL;
    size_t len = strlen(s);
    char *d = malloc(len + 1);
    if (d) memcpy(d, s, len + 1);
    return d;
}

typedef struct { char *title; char *note; } Task;

typedef struct {
    Task *items;
    size_t cnt, cap;
} TaskList;

static TaskList todo, doing, done;

static int tl_grow(TaskList *tl) {
    if (tl->cnt < tl->cap) return 0;
    size_t nc = tl->cap ? tl->cap * 2 : 4;
    if (nc < tl->cap) return -1;
    Task *t = realloc(tl->items, nc * sizeof(Task));
    if (!t) return -1;
    tl->items = t; tl->cap = nc;
    return 0;
}

static int tl_find(TaskList *tl, const char *title) {
    for (size_t i = 0; i < tl->cnt; i++)
        if (strcmp(tl->items[i].title, title) == 0) return (int)i;
    return -1;
}

static int tl_remove(TaskList *tl, const char *title, Task *out) {
    int idx = tl_find(tl, title);
    if (idx < 0) return 0;
    if (out) *out = tl->items[idx];
    for (size_t i = (size_t)idx; i + 1 < tl->cnt; i++)
        tl->items[i] = tl->items[i + 1];
    tl->cnt--;
    return 1;
}

static void tl_add(TaskList *tl, char *title, char *note) {
    if (tl_grow(tl) < 0) return;
    tl->items[tl->cnt].title = safe_strdup(title);
    tl->items[tl->cnt].note = safe_strdup(note);
    if (!tl->items[tl->cnt].title || !tl->items[tl->cnt].note) {
        free(tl->items[tl->cnt].title); free(tl->items[tl->cnt].note);
        tl->items[tl->cnt].title = NULL; tl->items[tl->cnt].note = NULL;
        return;
    }
    tl->cnt++;
}

static void cmd_todo(char *title, char *note) {
    tl_add(&todo, title, note);
}

static void cmd_start(char *title) {
    Task t;
    if (!tl_remove(&todo, title, &t)) return;
    if (tl_grow(&doing) < 0) { free(t.title); free(t.note); return; }
    doing.items[doing.cnt] = t;
    doing.cnt++;
}

static void cmd_done(char *title) {
    Task t;
    if (!tl_remove(&doing, title, &t)) {
        if (!tl_remove(&todo, title, &t)) return;
    }
    if (tl_grow(&done) < 0) { free(t.title); free(t.note); return; }
    done.items[done.cnt] = t;
    done.cnt++;
}

static void cmd_remove(char *title) {
    Task t;
    if (tl_remove(&todo, title, &t) || tl_remove(&doing, title, &t) || tl_remove(&done, title, &t)) {
        free(t.title); free(t.note);
    }
}

static void cmd_find(char *title) {
    const char *list_name = NULL;
    Task *found = NULL;
    int idx;
    if ((idx = tl_find(&todo, title)) >= 0) { list_name = "TODO"; found = &todo.items[idx]; }
    else if ((idx = tl_find(&doing, title)) >= 0) { list_name = "DOING"; found = &doing.items[idx]; }
    else if ((idx = tl_find(&done, title)) >= 0) { list_name = "DONE"; found = &done.items[idx]; }
    if (found) printf("%s %s\n", list_name, found->note);
}

static void cmd_report(void) {
    printf("TODO:\n");
    for (size_t i = 0; i < todo.cnt; i++) printf("  %s %s\n", todo.items[i].title, todo.items[i].note);
    printf("DOING:\n");
    for (size_t i = 0; i < doing.cnt; i++) printf("  %s %s\n", doing.items[i].title, doing.items[i].note);
    printf("DONE:\n");
    for (size_t i = 0; i < done.cnt; i++) printf("  %s %s\n", done.items[i].title, done.items[i].note);
}

static void tl_free(TaskList *tl) {
    for (size_t i = 0; i < tl->cnt; i++) { free(tl->items[i].title); free(tl->items[i].note); }
    free(tl->items); tl->items = NULL; tl->cnt = 0; tl->cap = 0;
}

static void cleanup(void) {
    tl_free(&todo); tl_free(&doing); tl_free(&done);
}

int main(void) {
    char line[MAX_LINE];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        char cmd[64] = {0}; int n1 = 0;
        if (sscanf(line, "%63s%n", cmd, &n1) < 1) continue;
        char *rest = line + n1;
        while (*rest == ' ') rest++;
        if (strcmp(cmd, "TODO") == 0) {
            char title[256] = {0}; int n2 = 0;
            if (sscanf(rest, "%255s%n", title, &n2) < 1) continue;
            char *note = rest + n2;
            while (*note == ' ') note++;
            cmd_todo(title, note);
        } else if (strcmp(cmd, "START") == 0 || strcmp(cmd, "DONE") == 0 || strcmp(cmd, "REMOVE") == 0 || strcmp(cmd, "FIND") == 0) {
            char title[256] = {0};
            if (sscanf(rest, "%255s", title) < 1) continue;
            if (strcmp(cmd, "START") == 0) cmd_start(title);
            else if (strcmp(cmd, "DONE") == 0) cmd_done(title);
            else if (strcmp(cmd, "REMOVE") == 0) cmd_remove(title);
            else cmd_find(title);
        } else if (strcmp(cmd, "REPORT") == 0) {
            cmd_report();
        }
    }
    cleanup();
    return 0;
}