/* F002.c */
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

static Tag *tags = NULL;

static Tag *find_tag(const char *name) {
    Tag *t = tags;
    while (t) {
        if (strcmp(t->name, name) == 0) return t;
        t = t->next;
    }
    return NULL;
}

static void add_message(Tag *tag, const char *text) {
    Message *m = malloc(sizeof(Message));
    if (!m) { perror("malloc"); exit(1); }
    m->text = strdup(text);
    m->next = tag->messages;
    tag->messages = m;
}

static void add_tag(const char *name) {
    Tag *t = malloc(sizeof(Tag));
    if (!t) { perror("malloc"); exit(1); }
    t->name = strdup(name);
    t->messages = NULL;
    t->next = tags;
    tags = t;
}

static void remove_tag(const char *name) {
    Tag **curr = &tags;
    while (*curr) {
        if (strcmp((*curr)->name, name) == 0) {
            Tag *t = *curr;
            *curr = t->next;
            Message *m = t->messages;
            while (m) {
                Message *tmp = m;
                m = m->next;
                free(tmp->text);
                free(tmp);
            }
            free(t->name);
            free(t);
            return;
        }
        curr = &(*curr)->next;
    }
}

static void concat_tags(const char *tag1, const char *tag2, const char *newtag) {
    Tag *t1 = find_tag(tag1);
    Tag *t2 = find_tag(tag2);
    if (!t1 || !t2) return;
    Message *m1 = t1->messages;
    Message *m2 = t2->messages;
    if (!m1 || !m2) return;
    size_t len = strlen(m1->text) + 1 + strlen(m2->text) + 1;
    char *combined = malloc(len);
    if (!combined) { perror("malloc"); exit(1); }
    sprintf(combined, "%s %s", m1->text, m2->text);
    // Remove existing tag if any
    remove_tag(newtag);
    add_tag(newtag);
    Tag *nt = find_tag(newtag);
    if (nt) add_message(nt, combined);
    free(combined);
}

static void find_tag_show(const char *name) {
    Tag *t = find_tag(name);
    if (!t) return;
    Message *m = t->messages;
    if (!m) return;
    for (char *c = m->text; *c; c++) {
        putchar(*c == ' ' ? '_' : *c);
    }
    putchar('\n');
}

static void report(void) {
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

int main(void) {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        char *cmd = strtok(line, " ");
        if (!cmd) continue;
        if (strcmp(cmd, "ADD") == 0) {
            char *tag = strtok(NULL, " ");
            if (!tag) continue;
            char *msg = strtok(NULL, "");
            if (!msg) msg = "";
            Tag *t = find_tag(tag);
            if (!t) { add_tag(tag); t = find_tag(tag); }
            if (t) add_message(t, msg);
        } else if (strcmp(cmd, "CONCAT") == 0) {
            char *tag1 = strtok(NULL, " ");
            char *tag2 = strtok(NULL, " ");
            char *newtag = strtok(NULL, " ");
            if (tag1 && tag2 && newtag) concat_tags(tag1, tag2, newtag);
        } else if (strcmp(cmd, "REMOVE") == 0) {
            char *tag = strtok(NULL, " ");
            if (tag) remove_tag(tag);
        } else if (strcmp(cmd, "FIND") == 0) {
            char *tag = strtok(NULL, " ");
            if (tag) find_tag_show(tag);
        } else if (strcmp(cmd, "REPORT") == 0) {
            report();
        }
    }
    // clean up not needed for demo
    return 0;
}