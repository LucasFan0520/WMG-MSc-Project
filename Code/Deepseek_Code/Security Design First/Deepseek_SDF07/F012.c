// F012.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_LINE 4096

typedef struct Person {
    char *name;
    char *note;
    struct Person *next;
} Person;

typedef struct {
    Person *head;
    Person *tail;
    size_t count;
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

static void q_init(Queue *q) {
    q->head = NULL;
    q->tail = NULL;
    q->count = 0;
}

static void q_free(Queue *q) {
    Person *cur = q->head;
    while (cur) {
        Person *next = cur->next;
        free(cur->name);
        free(cur->note);
        free(cur);
        cur = next;
    }
    q->head = NULL;
    q->tail = NULL;
    q->count = 0;
}

static Person *q_find(Queue *q, const char *name) {
    Person *cur = q->head;
    while (cur) {
        if (strcmp(cur->name, name) == 0)
            return cur;
        cur = cur->next;
    }
    return NULL;
}

static void q_join(Queue *q, const char *name, const char *note) {
    if (q_find(q, name)) return;
    Person *p = malloc(sizeof(Person));
    if (!p) return;
    p->name = safe_strdup(name);
    p->note = safe_strdup(note);
    p->next = NULL;
    if (!p->name || !p->note) {
        free(p->name);
        free(p->note);
        free(p);
        return;
    }
    if (q->tail)
        q->tail->next = p;
    else
        q->head = p;
    q->tail = p;
    q->count++;
}

static void q_serve(Queue *q) {
    if (!q->head) return;
    Person *p = q->head;
    printf("%s ", p->name);
    const char *n = p->note;
    while (*n) {
        putchar(*n == ' ' ? '_' : *n);
        n++;
    }
    putchar('\n');
    q->head = p->next;
    if (!q->head) q->tail = NULL;
    free(p->name);
    free(p->note);
    free(p);
    q->count--;
}

static void q_cancel(Queue *q, const char *name) {
    Person *prev = NULL;
    Person *cur = q->head;
    while (cur) {
        if (strcmp(cur->name, name) == 0) {
            if (prev)
                prev->next = cur->next;
            else
                q->head = cur->next;
            if (cur == q->tail)
                q->tail = prev;
            free(cur->name);
            free(cur->note);
            free(cur);
            q->count--;
            return;
        }
        prev = cur;
        cur = cur->next;
    }
}

static void q_rejoin(Queue *q, const char *name, const char *note) {
    q_cancel(q, name); // remove if exists
    q_join(q, name, note);
}

static void q_find_print(Queue *q, const char *name) {
    Person *p = q_find(q, name);
    if (!p) return;
    const char *n = p->note;
    while (*n) {
        putchar(*n == ' ' ? '_' : *n);
        n++;
    }
    putchar('\n');
}

static void q_queue(Queue *q) {
    Person *cur = q->head;
    while (cur) {
        printf("%s\n", cur->name);
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
    Queue q;
    q_init(&q);
    char line[MAX_LINE];
    while (fgets(line, sizeof(line), stdin)) {
        trim_newline(line);
        if (strcmp(line, "END") == 0) break;
        char cmd[32], a1[256], a2[4096];
        parse_cmd(line, cmd, sizeof(cmd), a1, sizeof(a1), a2, sizeof(a2));
        if (strcmp(cmd, "JOIN") == 0) {
            q_join(&q, a1, a2);
        } else if (strcmp(cmd, "SERVE") == 0) {
            q_serve(&q);
        } else if (strcmp(cmd, "CANCEL") == 0) {
            q_cancel(&q, a1);
        } else if (strcmp(cmd, "REJOIN") == 0) {
            q_rejoin(&q, a1, a2);
        } else if (strcmp(cmd, "FIND") == 0) {
            q_find_print(&q, a1);
        } else if (strcmp(cmd, "QUEUE") == 0) {
            q_queue(&q);
        }
    }
    q_free(&q);
    return 0;
}