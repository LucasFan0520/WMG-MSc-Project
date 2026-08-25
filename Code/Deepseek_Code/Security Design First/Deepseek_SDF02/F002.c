/* F002.c */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct msg {
    char *text;
    struct msg *next;
} msg_t;

typedef struct tag {
    char *name;
    msg_t *msgs;
    struct tag *next;
} tag_t;

tag_t *tags = NULL;

tag_t *find_tag(const char *name) {
    tag_t *t = tags;
    while (t) {
        if (strcmp(t->name, name) == 0) return t;
        t = t->next;
    }
    return NULL;
}

void add_tag_msg(const char *tagname, const char *msg) {
    tag_t *t = find_tag(tagname);
    if (!t) {
        t = malloc(sizeof(tag_t));
        if (!t) return;
        t->name = strdup(tagname);
        t->msgs = NULL;
        t->next = tags;
        tags = t;
        if (!t->name) { free(t); return; }
    }
    msg_t *m = malloc(sizeof(msg_t));
    if (!m) return;
    m->text = strdup(msg);
    m->next = NULL;
    if (!m->text) { free(m); return; }
    msg_t **last = &t->msgs;
    while (*last) last = &(*last)->next;
    *last = m;
}

void concat_tags(const char *t1, const char *t2, const char *newtag) {
    tag_t *a = find_tag(t1);
    tag_t *b = find_tag(t2);
    if (!a || !b || !a->msgs || !b->msgs) return;
    if (find_tag(newtag)) return; // ignore if newtag exists
    size_t len = strlen(a->msgs->text) + 1 + strlen(b->msgs->text) + 1;
    char *combined = malloc(len);
    if (!combined) return;
    sprintf(combined, "%s %s", a->msgs->text, b->msgs->text);
    add_tag_msg(newtag, combined);
    free(combined);
}

void remove_tag(const char *name) {
    tag_t *prev = NULL, *t = tags;
    while (t) {
        if (strcmp(t->name, name) == 0) {
            if (prev) prev->next = t->next;
            else tags = t->next;
            msg_t *m = t->msgs;
            while (m) {
                msg_t *nxt = m->next;
                free(m->text);
                free(m);
                m = nxt;
            }
            free(t->name);
            free(t);
            return;
        }
        prev = t;
        t = t->next;
    }
}

void find_tag_msg(const char *name) {
    tag_t *t = find_tag(name);
    if (!t || !t->msgs) {
        printf("NOT_FOUND\n");
        return;
    }
    for (char *s = t->msgs->text; *s; s++) {
        if (*s == ' ') putchar('_');
        else putchar(*s);
    }
    putchar('\n');
}

void report_all(void) {
    tag_t *t = tags;
    while (t) {
        msg_t *m = t->msgs;
        while (m) {
            printf("%s %s\n", t->name, m->text);
            m = m->next;
        }
        t = t->next;
    }
}

void free_all(void) {
    tag_t *t = tags;
    while (t) {
        tag_t *nxt = t->next;
        msg_t *m = t->msgs;
        while (m) {
            msg_t *n = m->next;
            free(m->text);
            free(m);
            m = n;
        }
        free(t->name);
        free(t);
        t = nxt;
    }
}

int main(void) {
    char *line = NULL;
    size_t len = 0;
    while (getline(&line, &len, stdin) != -1) {
        line[strcspn(line, "\n")] = 0;
        if (strcmp(line, "END") == 0) break;
        char cmd[10], a[256], b[256], c[256];
        if (sscanf(line, "%9s %255s %255s %255s", cmd, a, b, c) >= 2) {
            const char *msg = line + strlen(cmd) + 1 + strlen(a) + 1;
            if (strcmp(cmd, "ADD") == 0) {
                add_tag_msg(a, msg);
            } else if (strcmp(cmd, "CONCAT") == 0) {
                if (sscanf(line, "%*s %255s %255s %255s", a, b, c) == 3)
                    concat_tags(a, b, c);
            } else if (strcmp(cmd, "REMOVE") == 0) {
                remove_tag(a);
            } else if (strcmp(cmd, "FIND") == 0) {
                find_tag_msg(a);
            } else if (strcmp(cmd, "REPORT") == 0) {
                report_all();
            }
        }
    }
    free(line);
    free_all();
    return 0;
}