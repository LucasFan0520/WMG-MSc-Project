// F002.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Msg {
    char *msg;
    struct Msg *next;
} Msg;

typedef struct Tag {
    char *tag;
    Msg *msgs;
    struct Tag *next;
} Tag;

Tag *tags = NULL;

Tag *find_tag(const char *tag) {
    Tag *t = tags;
    while (t) {
        if (strcmp(t->tag, tag) == 0) return t;
        t = t->next;
    }
    return NULL;
}

void add_msg(Tag *t, const char *msg) {
    Msg *newm = malloc(sizeof(Msg));
    newm->msg = malloc(strlen(msg) + 1);
    strcpy(newm->msg, msg);
    newm->next = t->msgs;
    t->msgs = newm;
}

void add_tag(const char *tag, const char *msg) {
    Tag *t = find_tag(tag);
    if (!t) {
        t = malloc(sizeof(Tag));
        t->tag = malloc(strlen(tag) + 1);
        strcpy(t->tag, tag);
        t->msgs = NULL;
        t->next = tags;
        tags = t;
    }
    add_msg(t, msg);
}

char *first_msg(Tag *t) {
    if (t && t->msgs) return t->msgs->msg;
    return NULL;
}

void concat_tags(const char *tag1, const char *tag2, const char *newtag) {
    Tag *t1 = find_tag(tag1);
    Tag *t2 = find_tag(tag2);
    if (!t1 || !t2) return;
    char *m1 = first_msg(t1);
    char *m2 = first_msg(t2);
    if (!m1 || !m2) return;
    size_t len = strlen(m1) + strlen(m2) + 2;
    char *res = malloc(len);
    sprintf(res, "%s %s", m1, m2);
    add_tag(newtag, res);
    free(res);
}

void remove_tag(const char *tag) {
    Tag *t = find_tag(tag);
    if (!t) return;
    Msg *m = t->msgs;
    while (m) {
        Msg *tmp = m->next;
        free(m->msg);
        free(m);
        m = tmp;
    }
    t->msgs = NULL;
}

void find_msg(const char *tag) {
    Tag *t = find_tag(tag);
    if (!t || !t->msgs) { return; }
    char *m = t->msgs->msg;
    for (char *c = m; *c; c++) {
        if (*c == ' ') putchar('_');
        else putchar(*c);
    }
    putchar('\n');
}

void report_all(void) {
    Tag *t = tags;
    while (t) {
        Msg *m = t->msgs;
        while (m) {
            printf("%s %s\n", t->tag, m->msg);
            m = m->next;
        }
        t = t->next;
    }
}

void free_all(void) {
    Tag *t = tags;
    while (t) {
        Tag *tmp = t->next;
        Msg *m = t->msgs;
        while (m) {
            Msg *tmpm = m->next;
            free(m->msg);
            free(m);
            m = tmpm;
        }
        free(t->tag);
        free(t);
        t = tmp;
    }
}

int main() {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        char cmd[32];
        char a[256], b[256], c[256];
        if (sscanf(line, "%31s", cmd) < 1) continue;
        const char *rest = line + strlen(cmd) + 1;
        while (*rest == ' ') rest++;
        if (strcmp(cmd, "ADD") == 0) {
            char tag[256], msg[4096];
            if (sscanf(rest, "%255s", tag) == 1) {
                const char *mstart = rest + strlen(tag) + 1;
                while (*mstart == ' ') mstart++;
                if (*mstart) add_tag(tag, mstart);
            }
        } else if (strcmp(cmd, "CONCAT") == 0) {
            if (sscanf(rest, "%255s %255s %255s", a, b, c) == 3) {
                concat_tags(a, b, c);
            }
        } else if (strcmp(cmd, "REMOVE") == 0) {
            char tag[256];
            if (sscanf(rest, "%255s", tag) == 1) remove_tag(tag);
        } else if (strcmp(cmd, "FIND") == 0) {
            char tag[256];
            if (sscanf(rest, "%255s", tag) == 1) find_msg(tag);
        } else if (strcmp(cmd, "REPORT") == 0) {
            report_all();
        }
    }
    free_all();
    return 0;
}