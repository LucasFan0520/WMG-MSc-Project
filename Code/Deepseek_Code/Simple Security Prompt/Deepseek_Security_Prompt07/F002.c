// F002.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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

void add_message(const char *tag, const char *msg) {
    Tag *t = find_tag(tag);
    if (!t) {
        t = malloc(sizeof(Tag));
        t->name = strdup(tag);
        t->messages = NULL;
        t->next = tags;
        tags = t;
    }
    Message *m = malloc(sizeof(Message));
    m->text = strdup(msg);
    m->next = NULL;
    if (!t->messages) {
        t->messages = m;
    } else {
        Message *last = t->messages;
        while (last->next) last = last->next;
        last->next = m;
    }
}

void concat_tags(const char *tag1, const char *tag2, const char *newtag) {
    Tag *t1 = find_tag(tag1);
    Tag *t2 = find_tag(tag2);
    if (!t1 || !t2) return;
    if (!t1->messages || !t2->messages) return;
    char *joined = malloc(strlen(t1->messages->text) + 1 + strlen(t2->messages->text) + 1);
    sprintf(joined, "%s %s", t1->messages->text, t2->messages->text);
    add_message(newtag, joined);
    free(joined);
}

void remove_tag(const char *tag) {
    Tag *prev = NULL;
    Tag *t = tags;
    while (t) {
        if (strcmp(t->name, tag) == 0) {
            if (prev) prev->next = t->next;
            else tags = t->next;
            Message *m = t->messages;
            while (m) {
                Message *next = m->next;
                free(m->text);
                free(m);
                m = next;
            }
            free(t->name);
            free(t);
            return;
        }
        prev = t;
        t = t->next;
    }
}

void find_tag_first(const char *tag) {
    Tag *t = find_tag(tag);
    if (!t || !t->messages) {
        printf("NOT_FOUND\n");
        return;
    }
    const char *s = t->messages->text;
    for (; *s; s++) putchar(*s == ' ' ? '_' : *s);
    putchar('\n');
}

void report_all() {
    Tag *t = tags;
    while (t) {
        Message *m = t->messages;
        while (m) {
            printf("%s:", t->name);
            for (char *s = m->text; *s; s++) putchar(*s == ' ' ? '_' : *s);
            putchar('\n');
            m = m->next;
        }
        t = t->next;
    }
}

void free_all() {
    Tag *t = tags;
    while (t) {
        Tag *nextt = t->next;
        Message *m = t->messages;
        while (m) {
            Message *nextm = m->next;
            free(m->text);
            free(m);
            m = nextm;
        }
        free(t->name);
        free(t);
        t = nextt;
    }
}

int main() {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        char cmd[32], arg1[256], arg2[256], arg3[256];
        if (sscanf(line, "%31s %255s %255s %255s", cmd, arg1, arg2, arg3) < 2) continue;
        const char *rest = line + strlen(cmd) + 1;
        while (*rest == ' ') rest++;
        char *rest2 = strchr(rest, ' ');
        if (!rest2) rest2 = rest + strlen(rest);
        char *first = rest;
        char *second = NULL;
        char *third = NULL;
        if (rest2) {
            *rest2 = '\0';
            second = rest2 + 1;
            while (*second == ' ') second++;
            char *rest3 = strchr(second, ' ');
            if (rest3) {
                *rest3 = '\0';
                third = rest3 + 1;
                while (*third == ' ') third++;
            }
        }
        if (strcmp(cmd, "ADD") == 0 && first && second) {
            add_message(first, second);
        } else if (strcmp(cmd, "CONCAT") == 0 && first && second && third) {
            concat_tags(first, second, third);
        } else if (strcmp(cmd, "REMOVE") == 0 && first) {
            remove_tag(first);
        } else if (strcmp(cmd, "FIND") == 0 && first) {
            find_tag_first(first);
        } else if (strcmp(cmd, "REPORT") == 0) {
            report_all();
        }
    }
    free_all();
    return 0;
}