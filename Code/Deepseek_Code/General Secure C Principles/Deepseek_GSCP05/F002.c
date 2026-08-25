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
        if (!t) return;
        t->name = strdup(tag);
        t->messages = NULL;
        t->next = tags;
        tags = t;
    }
    Message *m = malloc(sizeof(Message));
    if (!m) return;
    m->text = strdup(msg);
    m->next = t->messages;
    t->messages = m;
}

void concat_tags(const char *tag1, const char *tag2, const char *newtag) {
    Tag *t1 = find_tag(tag1);
    Tag *t2 = find_tag(tag2);
    if (!t1 || !t2 || !t1->messages || !t2->messages) return;
    char *first = t1->messages->text;
    char *second = t2->messages->text;
    char *combined = malloc(strlen(first) + strlen(second) + 2);
    if (!combined) return;
    sprintf(combined, "%s %s", first, second);
    add_message(newtag, combined);
    free(combined);
}

void remove_tag(const char *tag) {
    Tag *t = find_tag(tag);
    if (!t) return;
    Message *m = t->messages;
    while (m) {
        Message *next = m->next;
        free(m->text);
        free(m);
        m = next;
    }
    t->messages = NULL;
    // remove tag from list
    Tag *prev = NULL, *cur = tags;
    while (cur) {
        if (cur == t) {
            if (prev) prev->next = cur->next;
            else tags = cur->next;
            free(cur->name);
            free(cur);
            return;
        }
        prev = cur;
        cur = cur->next;
    }
}

void find_tag(const char *tag) {
    Tag *t = find_tag(tag);
    if (!t || !t->messages) {
        printf("NOT_FOUND\n");
        return;
    }
    char *s = t->messages->text;
    for (; *s; s++) putchar(*s == ' ' ? '_' : *s);
    putchar('\n');
}

void report_all(void) {
    Tag *t = tags;
    while (t) {
        Message *m = t->messages;
        while (m) {
            printf("%s: ", t->name);
            char *s = m->text;
            for (; *s; s++) putchar(*s == ' ' ? '_' : *s);
            putchar('\n');
            m = m->next;
        }
        t = t->next;
    }
}

void free_all(void) {
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

int main(void) {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        char cmd[16];
        if (sscanf(line, "%15s", cmd) != 1) continue;
        if (strcmp(cmd, "ADD") == 0) {
            char *p = line + 4;
            while (*p == ' ') p++;
            char *tag = p;
            char *space = strchr(tag, ' ');
            if (!space) continue;
            *space = '\0';
            char *msg = space + 1;
            while (*msg == ' ') msg++;
            add_message(tag, msg);
        } else if (strcmp(cmd, "CONCAT") == 0) {
            char *p = line + 7;
            while (*p == ' ') p++;
            char *tag1 = p;
            char *space = strchr(tag1, ' ');
            if (!space) continue;
            *space = '\0';
            char *tag2 = space + 1;
            while (*tag2 == ' ') tag2++;
            space = strchr(tag2, ' ');
            if (!space) continue;
            *space = '\0';
            char *newtag = space + 1;
            while (*newtag == ' ') newtag++;
            concat_tags(tag1, tag2, newtag);
        } else if (strcmp(cmd, "REMOVE") == 0) {
            char *p = line + 7;
            while (*p == ' ') p++;
            char *tag = p;
            char *space = strchr(tag, ' ');
            if (space) *space = '\0';
            remove_tag(tag);
        } else if (strcmp(cmd, "FIND") == 0) {
            char *p = line + 5;
            while (*p == ' ') p++;
            char *tag = p;
            char *space = strchr(tag, ' ');
            if (space) *space = '\0';
            find_tag(tag);
        } else if (strcmp(cmd, "REPORT") == 0) {
            report_all();
        }
    }
    free_all();
    return 0;
}