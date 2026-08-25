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
    Msg *messages;
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

void add_msg(const char *tag, const char *msg) {
    Tag *t = find_tag(tag);
    if (!t) {
        t = malloc(sizeof(Tag));
        t->tag = strdup(tag);
        t->messages = NULL;
        t->next = tags;
        tags = t;
    }
    Msg *m = malloc(sizeof(Msg));
    m->msg = strdup(msg);
    m->next = NULL;
    if (!t->messages) t->messages = m;
    else {
        Msg *cur = t->messages;
        while (cur->next) cur = cur->next;
        cur->next = m;
    }
}

char *first_msg(const char *tag) {
    Tag *t = find_tag(tag);
    if (!t || !t->messages) return NULL;
    return t->messages->msg;
}

void concat_tags(const char *tag1, const char *tag2, const char *newtag) {
    char *m1 = first_msg(tag1);
    char *m2 = first_msg(tag2);
    if (!m1 || !m2) return;
    char *combined = malloc(strlen(m1) + strlen(m2) + 2);
    sprintf(combined, "%s %s", m1, m2);
    add_msg(newtag, combined);
    free(combined);
}

void remove_tag(const char *tag) {
    Tag *t = find_tag(tag);
    if (!t) return;
    // remove from list
    Tag *prev = NULL, *cur = tags;
    while (cur) {
        if (cur == t) {
            if (prev) prev->next = cur->next;
            else tags = cur->next;
            break;
        }
        prev = cur;
        cur = cur->next;
    }
    // free messages
    Msg *m = t->messages;
    while (m) {
        Msg *tmp = m;
        m = m->next;
        free(tmp->msg);
        free(tmp);
    }
    free(t->tag);
    free(t);
}

void find_tag_cmd(const char *tag) {
    char *m = first_msg(tag);
    if (!m) return;
    for (char *c = m; *c; c++) {
        if (*c == ' ') putchar('_');
        else putchar(*c);
    }
    putchar('\n');
}

void report(void) {
    Tag *t = tags;
    while (t) {
        Msg *m = t->messages;
        while (m) {
            printf("%s %s\n", t->tag, m->msg);
            m = m->next;
        }
        t = t->next;
    }
}

void cleanup_all(void) {
    Tag *t = tags;
    while (t) {
        Tag *tmp = t;
        t = t->next;
        Msg *m = tmp->messages;
        while (m) {
            Msg *tmpm = m;
            m = m->next;
            free(tmpm->msg);
            free(tmpm);
        }
        free(tmp->tag);
        free(tmp);
    }
}

int main(void) {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        if (strcmp(line, "END") == 0) break;
        char cmd[32], arg1[256], arg2[256], arg3[256], rest[2048];
        int n = sscanf(line, "%31s %255s %255s %255s", cmd, arg1, arg2, arg3);
        if (n < 2) continue;
        if (strcmp(cmd, "ADD") == 0 && n >= 3) {
            char *msg = line + strlen(cmd) + 1 + strlen(arg1) + 1;
            add_msg(arg1, msg);
        } else if (strcmp(cmd, "CONCAT") == 0 && n == 4) {
            concat_tags(arg1, arg2, arg3);
        } else if (strcmp(cmd, "REMOVE") == 0) {
            remove_tag(arg1);
        } else if (strcmp(cmd, "FIND") == 0) {
            find_tag_cmd(arg1);
        } else if (strcmp(cmd, "REPORT") == 0) {
            report();
        }
    }
    cleanup_all();
    return 0;
}