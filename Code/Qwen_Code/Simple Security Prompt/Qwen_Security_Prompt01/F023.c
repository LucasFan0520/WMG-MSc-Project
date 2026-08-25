// F023.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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

void add_mail(const char *sender, const char *subject, const char *body) {
    Mail *m = malloc(sizeof(Mail));
    m->sender = strdup(sender);
    m->subject = strdup(subject);
    m->body = strdup(body);
    m->tags = NULL;
    m->next = head;
    head = m;
}

void tag_mail(const char *subject, const char *tag) {
    for (Mail *m = head; m; m = m->next) {
        if (strcmp(m->subject, subject) == 0) {
            Tag *t = malloc(sizeof(Tag));
            t->name = strdup(tag);
            t->next = m->tags;
            m->tags = t;
            return;
        }
    }
}

void untag_mail(const char *subject, const char *tag) {
    for (Mail *m = head; m; m = m->next) {
        if (strcmp(m->subject, subject) == 0) {
            Tag *p = m->tags, *prev = NULL;
            while (p) {
                if (strcmp(p->name, tag) == 0) {
                    if (prev) prev->next = p->next;
                    else m->tags = p->next;
                    free(p->name); free(p);
                    return;
                }
                prev = p; p = p->next;
            }
            return;
        }
    }
}

void delete_mail(const char *subject) {
    Mail *p = head, *prev = NULL;
    while (p) {
        if (strcmp(p->subject, subject) == 0) {
            if (prev) prev->next = p->next;
            else head = p->next;
            Tag *t = p->tags;
            while (t) {
                Tag *nt = t->next;
                free(t->name); free(t);
                t = nt;
            }
            free(p->sender); free(p->subject); free(p->body); free(p);
            return;
        }
        prev = p; p = p->next;
    }
}

void search_mail(const char *tag) {
    for (Mail *m = head; m; m = m->next) {
        for (Tag *t = m->tags; t; t = t->next) {
            if (strcmp(t->name, tag) == 0) {
                printf("%s\n", m->subject);
                break;
            }
        }
    }
}

void report_mail() {
    for (Mail *m = head; m; m = m->next) {
        printf("%s %s %s\n", m->sender, m->subject, m->body);
    }
}

int main() {
    char line[10000];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        if (strcmp(line, "END") == 0) break;
        char cmd[20], a[1000], b[1000], c[8000];
        c[0] = 0;
        sscanf(line, "%s", cmd);
        if (strcmp(cmd, "MAIL") == 0) {
            sscanf(line, "%*s %s %s %[^\n]", a, b, c);
            add_mail(a, b, c);
        } else if (strcmp(cmd, "TAG") == 0) {
            sscanf(line, "%*s %s %s", a, b);
            tag_mail(a, b);
        } else if (strcmp(cmd, "UNTAG") == 0) {
            sscanf(line, "%*s %s %s", a, b);
            untag_mail(a, b);
        } else if (strcmp(cmd, "DELETE") == 0) {
            sscanf(line, "%*s %s", a);
            delete_mail(a);
        } else if (strcmp(cmd, "SEARCH") == 0) {
            sscanf(line, "%*s %s", a);
            search_mail(a);
        } else if (strcmp(cmd, "REPORT") == 0) {
            report_mail();
        }
    }
    while (head) {
        Mail *n = head->next;
        Tag *t = head->tags;
        while (t) {
            Tag *nt = t->next;
            free(t->name); free(t);
            t = nt;
        }
        free(head->sender); free(head->subject); free(head->body); free(head);
        head = n;
    }
    return 0;
}