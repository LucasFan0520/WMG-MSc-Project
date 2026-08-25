// F023.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct TagNode {
    char *tag;
    struct TagNode *next;
} TagNode;

typedef struct Message {
    char *sender;
    char *subject;
    char *body;
    TagNode *tags;
    struct Message *next;
} Message;

Message *messages = NULL;

Message *find_message_by_subject(const char *subject) {
    Message *m = messages;
    while (m) {
        if (strcmp(m->subject, subject) == 0) return m;
        m = m->next;
    }
    return NULL;
}

void add_tag_to_message(Message *m, const char *tag) {
    TagNode *t = m->tags;
    while (t) {
        if (strcmp(t->tag, tag) == 0) return;
        t = t->next;
    }
    TagNode *new = malloc(sizeof(TagNode));
    new->tag = malloc(strlen(tag) + 1);
    strcpy(new->tag, tag);
    new->next = m->tags;
    m->tags = new;
}

void remove_tag_from_message(Message *m, const char *tag) {
    TagNode *t = m->tags, *prev = NULL;
    while (t) {
        if (strcmp(t->tag, tag) == 0) {
            if (prev) prev->next = t->next;
            else m->tags = t->next;
            free(t->tag);
            free(t);
            return;
        }
        prev = t;
        t = t->next;
    }
}

void add_message(const char *sender, const char *subject, const char *body) {
    if (find_message_by_subject(subject)) return;
    Message *m = malloc(sizeof(Message));
    m->sender = malloc(strlen(sender) + 1);
    strcpy(m->sender, sender);
    m->subject = malloc(strlen(subject) + 1);
    strcpy(m->subject, subject);
    m->body = malloc(strlen(body) + 1);
    strcpy(m->body, body);
    m->tags = NULL;
    m->next = messages;
    messages = m;
}

void tag_message(const char *subject, const char *tag) {
    Message *m = find_message_by_subject(subject);
    if (m) add_tag_to_message(m, tag);
}

void untag_message(const char *subject, const char *tag) {
    Message *m = find_message_by_subject(subject);
    if (m) remove_tag_from_message(m, tag);
}

void delete_message(const char *subject) {
    Message *m = messages, *prev = NULL;
    while (m) {
        if (strcmp(m->subject, subject) == 0) {
            if (prev) prev->next = m->next;
            else messages = m->next;
            free(m->sender);
            free(m->subject);
            free(m->body);
            TagNode *t = m->tags;
            while (t) {
                TagNode *tmp = t->next;
                free(t->tag);
                free(t);
                t = tmp;
            }
            free(m);
            return;
        }
        prev = m;
        m = m->next;
    }
}

void search_tag(const char *tag) {
    Message *m = messages;
    while (m) {
        TagNode *t = m->tags;
        while (t) {
            if (strcmp(t->tag, tag) == 0) {
                printf("%s\n", m->subject);
                break;
            }
            t = t->next;
        }
        m = m->next;
    }
}

void report_all(void) {
    Message *m = messages;
    while (m) {
        printf("%s %s %s", m->sender, m->subject, m->body);
        TagNode *t = m->tags;
        while (t) {
            printf(" %s", t->tag);
            t = t->next;
        }
        putchar('\n');
        m = m->next;
    }
}

void free_all(void) {
    Message *m = messages;
    while (m) {
        Message *tmp = m->next;
        free(m->sender);
        free(m->subject);
        free(m->body);
        TagNode *t = m->tags;
        while (t) {
            TagNode *tt = t->next;
            free(t->tag);
            free(t);
            t = tt;
        }
        free(m);
        m = tmp;
    }
}

int main() {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        char cmd[32];
        char sender[256], subject[256], body[4096], tag[256];
        if (sscanf(line, "%31s", cmd) < 1) continue;
        const char *rest = line + strlen(cmd) + 1;
        while (*rest == ' ') rest++;
        if (strcmp(cmd, "MAIL") == 0) {
            if (sscanf(rest, "%255s %255s", sender, subject) == 2) {
                const char *b = rest + strlen(sender) + 1 + strlen(subject) + 1;
                while (*b == ' ') b++;
                if (*b) add_message(sender, subject, b);
                else add_message(sender, subject, "");
            }
        } else if (strcmp(cmd, "TAG") == 0) {
            if (sscanf(rest, "%255s %255s", subject, tag) == 2) tag_message(subject, tag);
        } else if (strcmp(cmd, "UNTAG") == 0) {
            if (sscanf(rest, "%255s %255s", subject, tag) == 2) untag_message(subject, tag);
        } else if (strcmp(cmd, "DELETE") == 0) {
            if (sscanf(rest, "%255s", subject) == 1) delete_message(subject);
        } else if (strcmp(cmd, "SEARCH") == 0) {
            if (sscanf(rest, "%255s", tag) == 1) search_tag(tag);
        } else if (strcmp(cmd, "REPORT") == 0) {
            report_all();
        }
    }
    free_all();
    return 0;
}