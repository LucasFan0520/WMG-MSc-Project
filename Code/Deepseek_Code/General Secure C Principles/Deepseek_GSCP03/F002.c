/* F002.c */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Message {
    char *text;
    struct Message *next;
} Message;

typedef struct Tag {
    char *tag;
    Message *messages;
    struct Tag *next;
} Tag;

Tag *head = NULL;

Tag *find_tag(const char *tag) {
    Tag *t = head;
    while (t) {
        if (strcmp(t->tag, tag) == 0) return t;
        t = t->next;
    }
    return NULL;
}

void add_tag(const char *tag) {
    if (find_tag(tag)) return;
    Tag *t = malloc(sizeof(Tag));
    if (!t) return;
    t->tag = strdup(tag);
    t->messages = NULL;
    t->next = head;
    head = t;
}

void add_message(const char *tag, const char *text) {
    Tag *t = find_tag(tag);
    if (!t) {
        add_tag(tag);
        t = find_tag(tag);
        if (!t) return;
    }
    Message *m = malloc(sizeof(Message));
    if (!m) return;
    m->text = strdup(text);
    m->next = t->messages;
    t->messages = m;
}

char *first_message(const char *tag) {
    Tag *t = find_tag(tag);
    if (!t || !t->messages) return NULL;
    return t->messages->text;
}

void concat_messages(const char *tag1, const char *tag2, const char *newtag) {
    char *m1 = first_message(tag1);
    char *m2 = first_message(tag2);
    if (!m1 || !m2) return;
    char *combined = malloc(strlen(m1) + strlen(m2) + 2);
    if (!combined) return;
    sprintf(combined, "%s %s", m1, m2);
    add_message(newtag, combined);
    free(combined);
}

void remove_tag(const char *tag) {
    Tag *prev = NULL, *curr = head;
    while (curr) {
        if (strcmp(curr->tag, tag) == 0) {
            if (prev) prev->next = curr->next;
            else head = curr->next;
            Message *m = curr->messages;
            while (m) {
                Message *tmp = m;
                m = m->next;
                free(tmp->text);
                free(tmp);
            }
            free(curr->tag);
            free(curr);
            return;
        }
        prev = curr;
        curr = curr->next;
    }
}

void find_message(const char *tag) {
    char *m = first_message(tag);
    if (!m) {
        printf("NOT_FOUND\n");
        return;
    }
    for (char *c = m; *c; c++) {
        if (*c == ' ') putchar('_');
        else putchar(*c);
    }
    putchar('\n');
}

void report_all(void) {
    Tag *t = head;
    while (t) {
        Message *m = t->messages;
        while (m) {
            printf("%s %s\n", t->tag, m->text);
            m = m->next;
        }
        t = t->next;
    }
}

void free_all(void) {
    Tag *t = head;
    while (t) {
        Tag *tmp = t;
        t = t->next;
        Message *m = tmp->messages;
        while (m) {
            Message *tmpm = m;
            m = m->next;
            free(tmpm->text);
            free(tmpm);
        }
        free(tmp->tag);
        free(tmp);
    }
}

int main(void) {
    char line[1024];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        char cmd[16];
        char arg1[256], arg2[256], arg3[256];
        if (sscanf(line, "%15s %255s %255s %255s", cmd, arg1, arg2, arg3) < 1) continue;
        if (strcmp(cmd, "ADD") == 0) {
            char *p = line + 4;
            while (*p == ' ') p++;
            char *tag_end = p;
            while (*tag_end && *tag_end != ' ') tag_end++;
            if (*tag_end) {
                *tag_end = '\0';
                char *text = tag_end + 1;
                while (*text == ' ') text++;
                add_message(p, text);
            }
        } else if (strcmp(cmd, "CONCAT") == 0) {
            char t1[256], t2[256], nt[256];
            if (sscanf(line + 7, "%255s %255s %255s", t1, t2, nt) == 3) {
                concat_messages(t1, t2, nt);
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
    return 0;
}