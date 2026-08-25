/* F002.c */
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

void add_tag(const char *name, const char *text) {
    Tag *t = find_tag(name);
    if (!t) {
        t = malloc(sizeof(Tag));
        t->name = strdup(name);
        t->msgs = NULL;
        t->next = tags;
        tags = t;
    }
    add_msg(t, text);
}

void concat(const char *t1, const char *t2, const char *newtag) {
    Tag *a = find_tag(t1), *b = find_tag(t2);
    if (!a || !b || !a->msgs || !b->msgs) return;
    char *combined = malloc(strlen(a->msgs->text) + strlen(b->msgs->text) + 2);
    sprintf(combined, "%s %s", a->msgs->text, b->msgs->text);
    add_tag(newtag, combined);
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

void find_msg(const char *name) {
    Tag *t = find_tag(name);
    if (!t || !t->msgs) { printf("NOT_FOUND\n"); return; }
    for (char *c = t->msgs->text; *c; c++) putchar(*c == ' ' ? '_' : *c);
    putchar('\n');
}

void report() {
    Tag *t = tags;
    while (t) {
        Msg *m = t->msgs;
        while (m) {
            printf("%s ", t->name);
            for (char *c = m->text; *c; c++) putchar(*c == ' ' ? '_' : *c);
            putchar('\n');
            m = m->next;
        }
        t = t->next;
    }
}

int main() {
    char line[1024];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        char cmd[16], tag1[256], tag2[256], newtag[256], text[1024];
        if (sscanf(line, "%15s", cmd) < 1) continue;
        if (strcmp(cmd, "ADD") == 0) {
            if (sscanf(line, "%*s %255s", tag1) < 1) continue;
            char *rest = line + strlen(cmd) + 1 + strlen(tag1) + 1;
            if (*rest == ' ') rest++;
            add_tag(tag1, rest);
        } else if (strcmp(cmd, "CONCAT") == 0) {
            if (sscanf(line, "%*s %255s %255s %255s", tag1, tag2, newtag) == 3)
                concat(tag1, tag2, newtag);
        } else if (strcmp(cmd, "REMOVE") == 0) {
            if (sscanf(line, "%*s %255s", tag1) == 1) remove_tag(tag1);
        } else if (strcmp(cmd, "FIND") == 0) {
            if (sscanf(line, "%*s %255s", tag1) == 1) find_msg(tag1);
        } else if (strcmp(cmd, "REPORT") == 0) report();
    }
    return 0;
}