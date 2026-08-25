/* F023.c */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Tag {
    char *tag;
    struct Tag *next;
} Tag;

typedef struct Message {
    char *sender;
    char *subject;
    char *body;
    Tag *tags;
    struct Message *next;
} Message;

Message *head = NULL;

Message *find_by_subject(char *subject) {
    for (Message *m = head; m; m = m->next)
        if (strcmp(m->subject, subject) == 0) return m;
    return NULL;
}

void add_message(char *sender, char *subject, char *body) {
    if (find_by_subject(subject)) return; // duplicate subject ignored
    Message *m = malloc(sizeof(Message));
    m->sender = strdup(sender);
    m->subject = strdup(subject);
    m->body = strdup(body);
    m->tags = NULL;
    m->next = head;
    head = m;
}

void tag_message(char *subject, char *tag) {
    Message *m = find_by_subject(subject);
    if (!m) return;
    Tag *t = malloc(sizeof(Tag));
    t->tag = strdup(tag);
    t->next = m->tags;
    m->tags = t;
}

void untag_message(char *subject, char *tag) {
    Message *m = find_by_subject(subject);
    if (!m) return;
    Tag *prev = NULL;
    for (Tag *t = m->tags; t; t = t->next) {
        if (strcmp(t->tag, tag) == 0) {
            if (prev) prev->next = t->next;
            else m->tags = t->next;
            free(t->tag);
            free(t);
            return;
        }
        prev = t;
    }
}

void delete_message(char *subject) {
    Message *prev = NULL;
    for (Message *m = head; m; m = m->next) {
        if (strcmp(m->subject, subject) == 0) {
            if (prev) prev->next = m->next;
            else head = m->next;
            free(m->sender);
            free(m->subject);
            free(m->body);
            Tag *t = m->tags;
            while (t) {
                Tag *next = t->next;
                free(t->tag);
                free(t);
                t = next;
            }
            free(m);
            return;
        }
        prev = m;
    }
}

void search_tag(char *tag) {
    for (Message *m = head; m; m = m->next) {
        for (Tag *t = m->tags; t; t = t->next) {
            if (strcmp(t->tag, tag) == 0) {
                printf("%s\n", m->subject);
                break;
            }
        }
    }
}

void report_all(void) {
    for (Message *m = head; m; m = m->next) {
        printf("%s %s %s\n", m->sender, m->subject, m->body);
        for (Tag *t = m->tags; t; t = t->next)
            printf("  %s\n", t->tag);
    }
}

void free_all(void) {
    Message *m = head;
    while (m) {
        Message *next = m->next;
        free(m->sender);
        free(m->subject);
        free(m->body);
        Tag *t = m->tags;
        while (t) {
            Tag *tnext = t->next;
            free(t->tag);
            free(t);
            t = tnext;
        }
        free(m);
        m = next;
    }
}

int main() {
    char *line = NULL;
    size_t len = 0;
    while (getline(&line, &len, stdin) != -1) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        char cmd[16], arg1[256], arg2[256], arg3[1024];
        int n = sscanf(line, "%15s %255s %255s %[^\n]", cmd, arg1, arg2, arg3);
        if (n < 1) continue;
        if (strcmp(cmd, "MAIL") == 0 && n == 4) {
            add_message(arg1, arg2, arg3);
        } else if (strcmp(cmd, "TAG") == 0 && n == 3) {
            tag_message(arg1, arg2);
        } else if (strcmp(cmd, "UNTAG") == 0 && n == 3) {
            untag_message(arg1, arg2);
        } else if (strcmp(cmd, "DELETE") == 0 && n == 2) {
            delete_message(arg1);
        } else if (strcmp(cmd, "SEARCH") == 0 && n == 2) {
            search_tag(arg1);
        } else if (strcmp(cmd, "REPORT") == 0 && n == 1) {
            report_all();
        }
    }
    free(line);
    free_all();
    return 0;
}