// F002.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_LINE 4096

typedef struct Message {
    char *text;
    struct Message *next;
} Message;

typedef struct Tag {
    char *name;
    Message *messages;
    struct Tag *next;
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

char *safe_strdup(const char *s) {
    if (!s) return NULL;
    size_t len = strlen(s);
    char *copy = malloc(len + 1);
    if (!copy) return NULL;
    memcpy(copy, s, len + 1);
    return copy;
}

void add_message_to_tag(const char *tag_name, const char *msg_text) {
    Tag *t = find_tag(tag_name);
    if (!t) {
        t = malloc(sizeof(Tag));
        if (!t) return;
        t->name = safe_strdup(tag_name);
        t->messages = NULL;
        t->next = tags;
        tags = t;
    }
    Message *m = malloc(sizeof(Message));
    if (!m) return;
    m->text = safe_strdup(msg_text);
    m->next = t->messages;
    t->messages = m;
}

void concat_tags(const char *t1, const char *t2, const char *newtag) {
    Tag *a = find_tag(t1);
    Tag *b = find_tag(t2);
    if (!a || !b) return;
    Message *m1 = a->messages;
    Message *m2 = b->messages;
    if (!m1 || !m2) return;
    size_t len = strlen(m1->text) + 1 + strlen(m2->text) + 1;
    char *combined = malloc(len);
    if (!combined) return;
    sprintf(combined, "%s %s", m1->text, m2->text);
    add_message_to_tag(newtag, combined);
    free(combined);
}

void remove_tag(const char *tag_name) {
    Tag *prev = NULL, *cur = tags;
    while (cur) {
        if (strcmp(cur->name, tag_name) == 0) {
            if (prev) prev->next = cur->next;
            else tags = cur->next;
            free(cur->name);
            Message *m = cur->messages;
            while (m) {
                Message *next = m->next;
                free(m->text);
                free(m);
                m = next;
            }
            free(cur);
            return;
        }
        prev = cur;
        cur = cur->next;
    }
}

void find_tag_cmd(const char *tag_name) {
    Tag *t = find_tag(tag_name);
    if (!t || !t->messages) {
        printf("NOT_FOUND\n");
        return;
    }
    const char *s = t->messages->text;
    for (; *s; s++) putchar(*s == ' ' ? '_' : *s);
    putchar('\n');
}

void report(void) {
    Tag *t = tags;
    while (t) {
        Message *m = t->messages;
        while (m) {
            printf("%s %s\n", t->name, m->text);
            m = m->next;
        }
        t = t->next;
    }
}

void free_all(void) {
    Tag *t = tags;
    while (t) {
        Tag *next_t = t->next;
        free(t->name);
        Message *m = t->messages;
        while (m) {
            Message *next_m = m->next;
            free(m->text);
            free(m);
            m = next_m;
        }
        free(t);
        t = next_t;
    }
}

int main(void) {
    char line[MAX_LINE];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;

        char cmd[32];
        char arg1[512], arg2[512], arg3[512];
        int n = sscanf(line, "%31s %511s %511s %511s", cmd, arg1, arg2, arg3);
        if (n < 1) continue;

        if (strcmp(cmd, "ADD") == 0 && n >= 3) {
            const char *rest = line;
            const char *p = rest;
            while (*p && !isspace(*p)) p++;
            while (*p && isspace(*p)) p++;
            while (*p && !isspace(*p)) p++;
            while (*p && isspace(*p)) p++;
            add_message_to_tag(arg1, p);
        } else if (strcmp(cmd, "CONCAT") == 0 && n >= 4) {
            concat_tags(arg1, arg2, arg3);
        } else if (strcmp(cmd, "REMOVE") == 0 && n >= 2) {
            remove_tag(arg1);
        } else if (strcmp(cmd, "FIND") == 0 && n >= 2) {
            find_tag_cmd(arg1);
        } else if (strcmp(cmd, "REPORT") == 0) {
            report();
        }
    }
    free_all();
    return 0;
}