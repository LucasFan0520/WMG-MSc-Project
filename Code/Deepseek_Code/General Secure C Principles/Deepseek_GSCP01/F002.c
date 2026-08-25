/* F002.c */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Message {
    char *message;
    struct Message *next;
} Message;

typedef struct Tag {
    char *tag;
    Message *messages;
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

void add_message(const char *tag, const char *msg) {
    Tag *t = find_tag(tag);
    if (!t) {
        t = malloc(sizeof(Tag));
        t->tag = strdup(tag);
        t->messages = NULL;
        t->next = tags;
        tags = t;
    }
    Message *m = malloc(sizeof(Message));
    m->message = strdup(msg);
    m->next = t->messages;
    t->messages = m;
}

void concat_tags(const char *tag1, const char *tag2, const char *newtag) {
    Tag *t1 = find_tag(tag1);
    Tag *t2 = find_tag(tag2);
    if (!t1 || !t2) return;
    if (!t1->messages || !t2->messages) return;
    char *first = t1->messages->message;
    char *second = t2->messages->message;
    char *combined = malloc(strlen(first) + strlen(second) + 2);
    strcpy(combined, first);
    strcat(combined, " ");
    strcat(combined, second);
    add_message(newtag, combined);
    free(combined);
}

void remove_tag(const char *tag) {
    Tag *t = find_tag(tag);
    if (!t) return;
    Tag **prev = &tags;
    while (*prev && (*prev) != t) prev = &(*prev)->next;
    if (*prev) *prev = t->next;
    free(t->tag);
    Message *m = t->messages;
    while (m) {
        Message *next = m->next;
        free(m->message);
        free(m);
        m = next;
    }
    free(t);
}

void find_message(const char *tag) {
    Tag *t = find_tag(tag);
    if (!t || !t->messages) { printf("NOT_FOUND\n"); return; }
    char *msg = t->messages->message;
    for (char *c = msg; *c; c++) {
        putchar(*c == ' ' ? '_' : *c);
    }
    putchar('\n');
}

void report_all(void) {
    Tag *t = tags;
    while (t) {
        printf("%s ", t->tag);
        Message *m = t->messages;
        int first = 1;
        while (m) {
            if (!first) putchar(' ');
            for (char *c = m->message; *c; c++) {
                putchar(*c == ' ' ? '_' : *c);
            }
            first = 0;
            m = m->next;
        }
        putchar('\n');
        t = t->next;
    }
}

void free_all(void) {
    Tag *t = tags;
    while (t) {
        Tag *nextt = t->next;
        free(t->tag);
        Message *m = t->messages;
        while (m) {
            Message *nextm = m->next;
            free(m->message);
            free(m);
            m = nextm;
        }
        free(t);
        t = nextt;
    }
}

int main(void) {
    char *line = NULL;
    size_t len = 0;
    while (getline(&line, &len, stdin) != -1) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        char cmd[16], tag1[256], tag2[256], newtag[256];
        if (sscanf(line, "%15s", cmd) < 1) continue;
        if (strcmp(cmd, "ADD") == 0) {
            char *p = line + 4;
            while (*p == ' ') p++;
            char *tag_start = p;
            while (*p && *p != ' ') p++;
            if (!*p) continue;
            *p = '\0';
            p++;
            while (*p == ' ') p++;
            add_message(tag_start, p);
        } else if (strcmp(cmd, "CONCAT") == 0) {
            char *p = line + 7;
            if (sscanf(p, "%255s %255s %255s", tag1, tag2, newtag) == 3) {
                concat_tags(tag1, tag2, newtag);
            }
        } else if (strcmp(cmd, "REMOVE") == 0) {
            char *p = line + 7;
            while (*p == ' ') p++;
            remove_tag(p);
        } else if (strcmp(cmd, "FIND") == 0) {
            char *p = line + 5;
            while (*p == ' ') p++;
            find_message(p);
        } else if (strcmp(cmd, "REPORT") == 0) {
            report_all();
        }
    }
    free_all();
    free(line);
    return 0;
}