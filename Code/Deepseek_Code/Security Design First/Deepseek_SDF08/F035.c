// F035.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_LINE 4096

typedef struct {
    char *name;
    char *payload;
} Job;

typedef struct {
    Job *items;
    size_t count;
    size_t capacity;
} Queue;

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

static void queue_init(Queue *q) {
    q->items = NULL;
    q->count = 0;
    q->capacity = 0;
}

static void queue_free(Queue *q) {
    for (size_t i = 0; i < q->count; i++) {
        free(q->items[i].name);
        free(q->items[i].payload);
    }
    free(q->items);
    q->items = NULL;
    q->count = 0;
    q->capacity = 0;
}

static int queue_find(Queue *q, const char *name) {
    for (size_t i = 0; i < q->count; i++) {
        if (strcmp(q->items[i].name, name) == 0)
            return (int)i;
    }
    return -1;
}

static void queue_enqueue(Queue *q, const char *name, const char *payload) {
    if (queue_find(q, name) >= 0) return;
    if (q->count == q->capacity) {
        size_t newcap = q->capacity ? q->capacity * 2 : 4;
        Job *tmp = realloc(q->items, newcap * sizeof(Job));
        if (!tmp) return;
        q->items = tmp;
        q->capacity = newcap;
    }
    q->items[q->count].name = safe_strdup(name);
    q->items[q->count].payload = safe_strdup(payload);
    if (q->items[q->count].name && q->items[q->count].payload)
        q->count++;
    else {
        free(q->items[q->count].name);
        free(q->items[q->count].payload);
    }
}

static void queue_clone(Queue *q, const char *old, const char *new) {
    if (queue_find(q, new) >= 0) return;
    int idx = queue_find(q, old);
    if (idx < 0) return;
    queue_enqueue(q, new, q->items[idx].payload);
}

static void queue_cancel(Queue *q, const char *name) {
    int idx = queue_find(q, name);
    if (idx < 0) return;
    free(q->items[idx].name);
    free(q->items[idx].payload);
    for (size_t i = idx; i < q->count - 1; i++)
        q->items[i] = q->items[i+1];
    q->count--;
}

static void queue_run(Queue *q, const char *name) {
    int idx = queue_find(q, name);
    if (idx < 0) return;
    const char *p = q->items[idx].payload;
    while (*p) {
        putchar(*p == ' ' ? '_' : *p);
        p++;
    }
    putchar('\n');
    queue_cancel(q, name);
}

static void queue_list(Queue *q) {
    for (size_t i = 0; i < q->count; i++) {
        printf("%s\n", q->items[i].name);
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
    Queue q;
    queue_init(&q);
    char line[MAX_LINE];
    while (fgets(line, sizeof(line), stdin)) {
        trim_newline(line);
        if (strcmp(line, "END") == 0) break;
        char cmd[32], a1[256], a2[4096];
        parse_cmd(line, cmd, sizeof(cmd), a1, sizeof(a1), a2, sizeof(a2));
        if (strcmp(cmd, "ENQUEUE") == 0) {
            queue_enqueue(&q, a1, a2);
        } else if (strcmp(cmd, "CLONE") == 0) {
            queue_clone(&q, a1, a2);
        } else if (strcmp(cmd, "CANCEL") == 0) {
            queue_cancel(&q, a1);
        } else if (strcmp(cmd, "RUN") == 0) {
            queue_run(&q, a1);
        } else if (strcmp(cmd, "LIST") == 0) {
            queue_list(&q);
        }
    }
    queue_free(&q);
    return 0;
}