// F027.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_LINE 4096

typedef struct {
    int priority;
    char *title;
    char *note;
} Reminder;

typedef struct {
    Reminder *items;
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

static void board_init(Board *b) {
    b->items = NULL;
    b->count = 0;
    b->capacity = 0;
}

static void board_free(Board *b) {
    for (size_t i = 0; i < b->count; i++) {
        free(b->items[i].title);
        free(b->items[i].note);
    }
    free(b->items);
    b->items = NULL;
    b->count = 0;
    b->capacity = 0;
}

static int board_find(Board *b, const char *title) {
    for (size_t i = 0; i < b->count; i++) {
        if (strcmp(b->items[i].title, title) == 0)
            return (int)i;
    }
    return -1;
}

static void board_add(Board *b, int priority, const char *title, const char *note) {
    if (board_find(b, title) >= 0) return;
    if (b->count == b->capacity) {
        size_t newcap = b->capacity ? b->capacity * 2 : 4;
        Reminder *tmp = realloc(b->items, newcap * sizeof(Reminder));
        if (!tmp) return;
        b->items = tmp;
        b->capacity = newcap;
    }
    b->items[b->count].priority = priority;
    b->items[b->count].title = safe_strdup(title);
    b->items[b->count].note = safe_strdup(note);
    if (b->items[b->count].title && b->items[b->count].note)
        b->count++;
    else {
        free(b->items[b->count].title);
        free(b->items[b->count].note);
    }
}

static void board_bump(Board *b, const char *title, int delta) {
    int idx = board_find(b, title);
    if (idx < 0) return;
    b->items[idx].priority += delta;
}

static void board_remove(Board *b, const char *title) {
    int idx = board_find(b, title);
    if (idx < 0) return;
    free(b->items[idx].title);
    free(b->items[idx].note);
    for (size_t i = idx; i < b->count - 1; i++)
        b->items[i] = b->items[i+1];
    b->count--;
}

static int cmp_priority(const void *a, const void *b) {
    const Reminder *pa = (const Reminder *)a;
    const Reminder *pb = (const Reminder *)b;
    return pb->priority - pa->priority;
}

static void board_top(Board *b, int n) {
    if (b->count == 0 || n <= 0) return;
    size_t m = (size_t)n < b->count ? (size_t)n : b->count;
    Reminder *copy = malloc(m * sizeof(Reminder));
    if (!copy) return;
    for (size_t i = 0; i < m; i++) {
        copy[i].priority = b->items[i].priority;
        copy[i].title = safe_strdup(b->items[i].title);
        copy[i].note = safe_strdup(b->items[i].note);
        if (!copy[i].title || !copy[i].note) {
            for (size_t j = 0; j <= i; j++) {
                free(copy[j].title);
                free(copy[j].note);
            }
            free(copy);
            return;
        }
    }
    qsort(copy, m, sizeof(Reminder), cmp_priority);
    for (size_t i = 0; i < m; i++) {
        printf("%d %s ", copy[i].priority, copy[i].title);
        const char *p = copy[i].note;
        while (*p) {
            putchar(*p == ' ' ? '_' : *p);
            p++;
        }
        putchar('\n');
        free(copy[i].title);
        free(copy[i].note);
    }
    free(copy);
}

static void board_list(Board *b) {
    for (size_t i = 0; i < b->count; i++) {
        printf("%s\n", b->items[i].title);
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
        char cmd[32], a1[64], a2[256], a3[4096];
        parse_cmd(line, cmd, sizeof(cmd), a1, sizeof(a1), a2, sizeof(a2), a3, sizeof(a3));
        if (strcmp(cmd, "ADD") == 0) {
            int pri = atoi(a1);
            board_add(&b, pri, a2, a3);
        } else if (strcmp(cmd, "BUMP") == 0) {
            int delta = atoi(a2);
            board_bump(&b, a1, delta);
        } else if (strcmp(cmd, "REMOVE") == 0) {
            board_remove(&b, a1);
        } else if (strcmp(cmd, "TOP") == 0) {
            int n = atoi(a1);
            board_top(&b, n);
        } else if (strcmp(cmd, "LIST") == 0) {
            board_list(&b);
        }
    }
    board_free(&b);
    return 0;
}