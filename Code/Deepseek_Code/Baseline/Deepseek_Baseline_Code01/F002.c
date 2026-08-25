// F002.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct msg {
    char *text;
    struct msg *next;
} Msg;

typedef struct tag {
    char *name;
    Msg *msgs;
    struct tag *next;
} Tag;

Tag *tags = NULL;

Tag *find_tag(const char *name) {
    Tag *t = tags;
    while (t) {
        if (strcmp(t->name, name) == 0) return t;
        t = t->next;
    }
    return NULL;
}

void add_msg(Tag *t, const char *text) {
    Msg *m = malloc(sizeof(Msg));
    m->text = strdup(text);
    m->next = t->msgs;
    t->msgs = m;
}

void add_tag_msg(const char *tag, const char *msg) {
    Tag *t = find_tag(tag);
    if (!t) {
        t = malloc(sizeof(Tag));
        t->name = strdup(tag);
        t->msgs = NULL;
        t->next = tags;
        tags = t;
    }
    add_msg(t, msg);
}

void concat_tags(const char *t1, const char *t2, const char *newtag) {
    Tag *a = find_tag(t1), *b = find_tag(t2);
    if (!a || !b || !a->msgs || !b->msgs) return;
    char *combined = malloc(strlen(a->msgs->text) + strlen(b->msgs->text) + 2);
    strcpy(combined, a->msgs->text);
    strcat(combined, " ");
    strcat(combined, b->msgs->text);
    add_tag_msg(newtag, combined);
    free(combined);
}

void remove_tag(const char *name) {
    Tag *t = tags, *prev = NULL;
    while (t) {
        if (strcmp(t->name, name) == 0) {
            if (prev) prev->next = t->next;
            else tags = t->next;
            Msg *m = t->msgs;
            while (m) { Msg *tmp = m; m = m->next; free(tmp->text); free(tmp); }
            free(t->name);
            free(t);
            return;
        }
        prev = t;
        t = t->next;
    }
}

void find_tag(const char *name) {
    Tag *t = find_tag(name);
    if (!t || !t->msgs) return;
    for (char *c = t->msgs->text; *c; c++)
        putchar(*c == ' ' ? '_' : *c);
    putchar('\n');
}

void report(void) {
    Tag *t = tags;
    while (t) {
        Msg *m = t->msgs;
        while (m) {
            printf("%s ", t->name);
            for (char *c = m->text; *c; c++)
                putchar(*c == ' ' ? '_' : *c);
            putchar('\n');
            m = m->next;
        }
        t = t->next;
    }
}

int main(void) {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        if (strcmp(line, "END\n") == 0 || strcmp(line, "END") == 0) break;
        char cmd[32];
        char arg1[256], arg2[256], arg3[256];
        if (sscanf(line, "%31s", cmd) != 1) continue;
        if (strcmp(cmd, "ADD") == 0) {
            if (sscanf(line, "%*s %255s %[^\n]", arg1, arg2) == 2)
                add_tag_msg(arg1, arg2);
        } else if (strcmp(cmd, "CONCAT") == 0) {
            if (sscanf(line, "%*s %255s %255s %255s", arg1, arg2, arg3) == 3)
                concat_tags(arg1, arg2, arg3);
        } else if (strcmp(cmd, "REMOVE") == 0) {
            if (sscanf(line, "%*s %255s", arg1) == 1)
                remove_tag(arg1);
        } else if (strcmp(cmd, "FIND") == 0) {
            if (sscanf(line, "%*s %255s", arg1) == 1)
                find_tag(arg1);
        } else if (strcmp(cmd, "REPORT") == 0) {
            report();
        }
    }
    return 0;
}