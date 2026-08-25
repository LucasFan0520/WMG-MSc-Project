/* F002.c */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

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

void add_message(Tag *t, const char *text) {
    Message *m = malloc(sizeof(Message));
    m->text = strdup(text);
    m->next = t->messages;
    t->messages = m;
}

void free_messages(Message *m) {
    while (m) {
        Message *tmp = m;
        m = m->next;
        free(tmp->text);
        free(tmp);
    }
}

Tag *add_tag(const char *name) {
    Tag *t = malloc(sizeof(Tag));
    t->name = strdup(name);
    t->messages = NULL;
    t->next = tags;
    tags = t;
    return t;
}

void remove_tag(const char *name) {
    Tag *prev = NULL, *curr = tags;
    while (curr) {
        if (strcmp(curr->name, name) == 0) {
            if (prev) prev->next = curr->next;
            else tags = curr->next;
            free_messages(curr->messages);
            free(curr->name);
            free(curr);
            return;
        }
        prev = curr;
        curr = curr->next;
    }
}

const char *first_message(Tag *t) {
    if (t && t->messages) return t->messages->text;
    return NULL;
}

void concat_tags(const char *t1, const char *t2, const char *newtag) {
    Tag *a = find_tag(t1);
    Tag *b = find_tag(t2);
    if (!a || !b) return;
    const char *m1 = first_message(a);
    const char *m2 = first_message(b);
    if (!m1 || !m2) return;
    size_t len = strlen(m1) + strlen(m2) + 2;
    char *joint = malloc(len);
    strcpy(joint, m1);
    strcat(joint, " ");
    strcat(joint, m2);
    Tag *nt = add_tag(newtag);
    add_message(nt, joint);
    free(joint);
}

void find_tag_and_print(const char *name) {
    Tag *t = find_tag(name);
    if (!t || !t->messages) {
        printf("NOT_FOUND\n");
        return;
    }
    const char *msg = t->messages->text;
    for (const char *c = msg; *c; c++) {
        putchar(*c == ' ' ? '_' : *c);
    }
    putchar('\n');
}

void report_all(void) {
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

void trim_newline(char *s) {
    size_t len = strlen(s);
    if (len > 0 && s[len-1] == '\n') s[len-1] = '\0';
}

int main(void) {
    char line[4096];
    char cmd[32];
    char tag1[256], tag2[256], newtag[256], tag[256];

    while (fgets(line, sizeof(line), stdin)) {
        trim_newline(line);
        if (strcmp(line, "END") == 0) break;
        if (sscanf(line, "%31s", cmd) != 1) continue;

        if (strcmp(cmd, "ADD") == 0) {
            char *p = line + 3;
            while (*p == ' ') p++;
            if (sscanf(p, "%255s", tag) != 1) continue;
            p += strlen(tag);
            while (*p == ' ') p++;
            Tag *t = find_tag(tag);
            if (!t) t = add_tag(tag);
            add_message(t, p);
        } else if (strcmp(cmd, "CONCAT") == 0) {
            char *p = line + 6;
            while (*p == ' ') p++;
            if (sscanf(p, "%255s %255s %255s", tag1, tag2, newtag) != 3) continue;
            concat_tags(tag1, tag2, newtag);
        } else if (strcmp(cmd, "REMOVE") == 0) {
            char *p = line + 6;
            while (*p == ' ') p++;
            if (sscanf(p, "%255s", tag) == 1) remove_tag(tag);
        } else if (strcmp(cmd, "FIND") == 0) {
            char *p = line + 4;
            while (*p == ' ') p++;
            if (sscanf(p, "%255s", tag) == 1) find_tag_and_print(tag);
        } else if (strcmp(cmd, "REPORT") == 0) {
            report_all();
        }
    }

    while (tags) {
        Tag *tmp = tags;
        tags = tags->next;
        free_messages(tmp->messages);
        free(tmp->name);
        free(tmp);
    }
    return 0;
}