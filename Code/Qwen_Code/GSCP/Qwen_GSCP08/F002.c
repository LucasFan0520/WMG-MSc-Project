// F002.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Msg {
    char *text;
    struct Msg *next;
} Msg;

typedef struct Tag {
    char *name;
    Msg *msgs;
    struct Tag *next;
} Tag;

Tag *head = NULL;

Tag *find_tag(char *name) {
    for (Tag *t = head; t; t = t->next) {
        if (strcmp(t->name, name) == 0) return t;
    }
    return NULL;
}

void add_msg(char *tag, char *text) {
    Tag *t = find_tag(tag);
    if (!t) {
        t = malloc(sizeof(Tag));
        t->name = strdup(tag);
        t->msgs = NULL;
        t->next = head;
        head = t;
    }
    Msg *m = malloc(sizeof(Msg));
    m->text = strdup(text);
    m->next = NULL;
    if (!t->msgs) {
        t->msgs = m;
    } else {
        Msg *last = t->msgs;
        while (last->next) last = last->next;
        last->next = m;
    }
}

void concat_msg(char *t1, char *t2, char *nt) {
    Tag *tag1 = find_tag(t1);
    Tag *tag2 = find_tag(t2);
    if (!tag1 || !tag2 || !tag1->msgs || !tag2->msgs) return;
    char *buf = malloc(strlen(tag1->msgs->text) + strlen(tag2->msgs->text) + 2);
    sprintf(buf, "%s %s", tag1->msgs->text, tag2->msgs->text);
    add_msg(nt, buf);
    free(buf);
}

void remove_tag(char *tag) {
    Tag *prev = NULL;
    Tag *curr = head;
    while (curr) {
        if (strcmp(curr->name, tag) == 0) {
            if (prev) prev->next = curr->next;
            else head = curr->next;
            free(curr->name);
            Msg *m = curr->msgs;
            while (m) {
                Msg *next = m->next;
                free(m->text);
                free(m);
                m = next;
            }
            free(curr);
            return;
        }
        prev = curr;
        curr = curr->next;
    }
}

void find_msg(char *tag) {
    Tag *t = find_tag(tag);
    if (t && t->msgs) {
        for (char *c = t->msgs->text; *c; c++) {
            putchar(*c == ' ' ? '_' : *c);
        }
        putchar('\n');
    }
}

void report_tags() {
    for (Tag *t = head; t; t = t->next) {
        for (Msg *m = t->msgs; m; m = m->next) {
            printf("%s %s\n", t->name, m->text);
        }
    }
}

int main() {
    char cmd[20], a[100], b[100], text[1000];
    while (scanf("%19s", cmd) == 1 && strcmp(cmd, "END") != 0) {
        if (strcmp(cmd, "ADD") == 0) {
            scanf("%99s", a);
            fgets(text, sizeof(text), stdin);
            while (text[0] == ' ') memmove(text, text + 1, strlen(text));
            text[strcspn(text, "\n")] = 0;
            add_msg(a, text);
        } else if (strcmp(cmd, "CONCAT") == 0) {
            scanf("%99s%99s%99s", a, b, text);
            concat_msg(a, b, text);
        } else if (strcmp(cmd, "REMOVE") == 0) {
            scanf("%99s", a);
            remove_tag(a);
        } else if (strcmp(cmd, "FIND") == 0) {
            scanf("%99s", a);
            find_msg(a);
        } else if (strcmp(cmd, "REPORT") == 0) {
            report_tags();
        }
    }
    return 0;
}