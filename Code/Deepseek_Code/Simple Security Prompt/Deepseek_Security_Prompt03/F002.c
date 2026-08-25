/*
 * F002.c
 */
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

void add_message(const char *tag, const char *msg) {
    Tag *t = find_tag(tag);
    if (!t) {
        t = malloc(sizeof(Tag));
        t->name = strdup(tag);
        t->msgs = NULL;
        t->next = tags;
        tags = t;
    }
    Msg *m = malloc(sizeof(Msg));
    m->text = strdup(msg);
    m->next = t->msgs;
    t->msgs = m;
}

char *first_message(const char *tag) {
    Tag *t = find_tag(tag);
    if (!t || !t->msgs) return NULL;
    return t->msgs->text;
}

void concat_messages(const char *tag1, const char *tag2, const char *newtag) {
    char *m1 = first_message(tag1);
    char *m2 = first_message(tag2);
    if (!m1 || !m2) return;
    char *combined = malloc(strlen(m1) + strlen(m2) + 2);
    sprintf(combined, "%s %s", m1, m2);
    add_message(newtag, combined);
    free(combined);
}

void remove_tag(const char *tag) {
    Tag *prev = NULL, *t = tags;
    while (t) {
        if (strcmp(t->name, tag) == 0) {
            if (prev) prev->next = t->next;
            else tags = t->next;
            Msg *m = t->msgs;
            while (m) {
                Msg *tmp = m;
                m = m->next;
                free(tmp->text);
                free(tmp);
            }
            free(t->name);
            free(t);
            return;
        }
        prev = t;
        t = t->next;
    }
}

void find_tag_cmd(const char *tag) {
    Tag *t = find_tag(tag);
    if (!t || !t->msgs) {
        printf("NOT_FOUND\n");
        return;
    }
    char *s = t->msgs->text;
    for (; *s; s++) putchar(*s == ' ' ? '_' : *s);
    putchar('\n');
}

void report(void) {
    Tag *t = tags;
    while (t) {
        Msg *m = t->msgs;
        while (m) {
            printf("%s %s\n", t->name, m->text);
            m = m->next;
        }
        t = t->next;
    }
}

int main(void) {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        if (strcmp(line, "END") == 0) break;
        char cmd[32], arg1[256], arg2[256], arg3[256];
        if (sscanf(line, "%31s %255s %255s %255s", cmd, arg1, arg2, arg3) < 1) continue;
        if (strcmp(cmd, "ADD") == 0) {
            char *msg = line + strlen(cmd) + strlen(arg1) + 1;
            while (*msg == ' ') msg++;
            add_message(arg1, msg);
        } else if (strcmp(cmd, "CONCAT") == 0) {
            concat_messages(arg1, arg2, arg3);
        } else if (strcmp(cmd, "REMOVE") == 0) {
            remove_tag(arg1);
        } else if (strcmp(cmd, "FIND") == 0) {
            find_tag_cmd(arg1);
        } else if (strcmp(cmd, "REPORT") == 0) {
            report();
        }
    }
    return 0;
}