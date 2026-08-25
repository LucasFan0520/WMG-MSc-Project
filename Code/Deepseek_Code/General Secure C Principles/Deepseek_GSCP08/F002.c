// F002.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Msg { char *msg; struct Msg *next; } Msg;
typedef struct Tag { char *tag; Msg *msgs; struct Tag *next; } Tag;
Tag *tags = NULL;

Tag *find_tag(const char *tag) {
    for (Tag *t = tags; t; t = t->next)
        if (strcmp(t->tag, tag) == 0) return t;
    return NULL;
}

void add_msg(const char *tag, const char *msg) {
    Tag *t = find_tag(tag);
    if (!t) {
        t = malloc(sizeof(Tag));
        if (!t) return;
        t->tag = strdup(tag); t->msgs = NULL; t->next = tags; tags = t;
    }
    Msg *m = malloc(sizeof(Msg));
    if (!m) return;
    m->msg = strdup(msg);
    m->next = t->msgs;
    t->msgs = m;
}

void concat(const char *t1, const char *t2, const char *nt) {
    Tag *a = find_tag(t1), *b = find_tag(t2);
    if (!a || !b || !a->msgs || !b->msgs) return;
    size_t len = strlen(a->msgs->msg) + 1 + strlen(b->msgs->msg) + 1;
    char *buf = malloc(len);
    if (!buf) return;
    sprintf(buf, "%s %s", a->msgs->msg, b->msgs->msg);
    add_msg(nt, buf);
    free(buf);
}

void remove_tag(const char *tag) {
    Tag *prev = NULL;
    for (Tag *t = tags; t; prev = t, t = t->next) {
        if (strcmp(t->tag, tag) == 0) {
            if (prev) prev->next = t->next; else tags = t->next;
            while (t->msgs) { Msg *m = t->msgs; t->msgs = m->next; free(m->msg); free(m); }
            free(t->tag); free(t);
            return;
        }
    }
}

void find_msg(const char *tag) {
    Tag *t = find_tag(tag);
    if (!t || !t->msgs) { printf("NOT_FOUND\n"); return; }
    for (char *c = t->msgs->msg; *c; c++) putchar(*c == ' ' ? '_' : *c);
    putchar('\n');
}

void report(void) {
    for (Tag *t = tags; t; t = t->next) {
        for (Msg *m = t->msgs; m; m = m->next) {
            printf("%s ", t->tag);
            for (char *c = m->msg; *c; c++) putchar(*c == ' ' ? '_' : *c);
            putchar('\n');
        }
    }
}

int main() {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        if (strcmp(line, "END") == 0) break;
        char cmd[16];
        if (sscanf(line, "%15s", cmd) != 1) continue;
        char *rest = line + strlen(cmd);
        while (*rest == ' ') rest++;
        if (strcmp(cmd, "ADD") == 0) {
            char tag[256], msg[4096];
            if (sscanf(rest, "%255s", tag) == 1) {
                char *m = rest + strlen(tag);
                while (*m == ' ') m++;
                strcpy(msg, m);
                add_msg(tag, msg);
            }
        } else if (strcmp(cmd, "CONCAT") == 0) {
            char t1[256], t2[256], nt[256];
            if (sscanf(rest, "%255s %255s %255s", t1, t2, nt) == 3)
                concat(t1, t2, nt);
        } else if (strcmp(cmd, "REMOVE") == 0) {
            char tag[256];
            if (sscanf(rest, "%255s", tag) == 1) remove_tag(tag);
        } else if (strcmp(cmd, "FIND") == 0) {
            char tag[256];
            if (sscanf(rest, "%255s", tag) == 1) find_msg(tag);
        } else if (strcmp(cmd, "REPORT") == 0) {
            report();
        }
    }
    while (tags) { Tag *t = tags; tags = t->next; while (t->msgs) { Msg *m = t->msgs; t->msgs = m->next; free(m->msg); free(m); } free(t->tag); free(t); }
    return 0;
}