// F023.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_LINE 65536

typedef struct Tag {
    char *name;
    struct Tag *next;
} Tag;

typedef struct Mail {
    char *sender;
    char *subject;
    char *body;
    Tag *tags;
    struct Mail *next;
} Mail;

Mail *head = NULL;

char *get_token(char **s) {
    while (**s == ' ') (*s)++;
    if (!**s) return NULL;
    char *start = *s;
    while (**s && **s != ' ') (*s)++;
    if (**s) { **s = '\0'; (*s)++; }
    return start;
}

Mail *find_mail_subject(const char *subject) {
    for (Mail *m = head; m; m = m->next)
        if (strcmp(m->subject, subject) == 0) return m;
    return NULL;
}

void free_mail(Mail *m) {
    Tag *t = m->tags;
    while (t) {
        Tag *next = t->next;
        free(t->name);
        free(t);
        t = next;
    }
    free(m->sender);
    free(m->subject);
    free(m->body);
    free(m);
}

int main() {
    char line[MAX_LINE];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        char *ptr = line;
        char *cmd = get_token(&ptr);
        if (!cmd) continue;
        if (strcmp(cmd, "MAIL") == 0) {
            char *sender = get_token(&ptr);
            char *subject = get_token(&ptr);
            if (sender && subject) {
                Mail *m = malloc(sizeof(Mail));
                if (m) {
                    m->sender = strdup(sender);
                    m->subject = strdup(subject);
                    m->body = strdup(ptr);
                    m->tags = NULL;
                    m->next = head;
                    head = m;
                }
            }
        } else if (strcmp(cmd, "TAG") == 0) {
            char *subject = get_token(&ptr);
            char *tag = get_token(&ptr);
            if (subject && tag) {
                Mail *m = find_mail_subject(subject);
                if (m) {
                    Tag *t = malloc(sizeof(Tag));
                    if (t) {
                        t->name = strdup(tag);
                        t->next = m->tags;
                        m->tags = t;
                    }
                }
            }
        } else if (strcmp(cmd, "UNTAG") == 0) {
            char *subject = get_token(&ptr);
            char *tag = get_token(&ptr);
            if (subject && tag) {
                Mail *m = find_mail_subject(subject);
                if (m) {
                    Tag *prev = NULL, *curr = m->tags;
                    while (curr) {
                        if (strcmp(curr->name, tag) == 0) {
                            if (prev) prev->next = curr->next;
                            else m->tags = curr->next;
                            free(curr->name);
                            free(curr);
                            break;
                        }
                        prev = curr;
                        curr = curr->next;
                    }
                }
            }
        } else if (strcmp(cmd, "DELETE") == 0) {
            char *subject = get_token(&ptr);
            if (subject) {
                Mail *prev = NULL, *curr = head;
                while (curr) {
                    if (strcmp(curr->subject, subject) == 0) {
                        if (prev) prev->next = curr->next;
                        else head = curr->next;
                        free_mail(curr);
                        break;
                    }
                    prev = curr;
                    curr = curr->next;
                }
            }
        } else if (strcmp(cmd, "SEARCH") == 0) {
            char *tag = get_token(&ptr);
            if (tag) {
                for (Mail *m = head; m; m = m->next) {
                    for (Tag *t = m->tags; t; t = t->next) {
                        if (strcmp(t->name, tag) == 0) {
                            printf("%s\n", m->subject);
                            break;
                        }
                    }
                }
            }
        } else if (strcmp(cmd, "REPORT") == 0) {
            for (Mail *m = head; m; m = m->next) {
                printf("%s %s %s\n", m->sender, m->subject, m->body);
            }
        }
    }
    while (head) {
        Mail *next = head->next;
        free_mail(head);
        head = next;
    }
    return 0;
}