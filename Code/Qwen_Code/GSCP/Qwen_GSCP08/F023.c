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

Mail *find_mail_by_subject(char *subject) {
    for (Mail *m = head; m; m = m->next) {
        if (strcmp(m->subject, subject) == 0) return m;
    }
    return NULL;
}

void create_mail(char *sender, char *subject, char *body) {
    Mail *m = malloc(sizeof(Mail));
    m->sender = strdup(sender);
    m->subject = strdup(subject);
    m->body = strdup(body);
    m->tags = NULL;
    m->next = head;
    head = m;
}

void add_tag(char *subject, char *tag_name) {
    Mail *m = find_mail_by_subject(subject);
    if (!m) return;
    for (Tag *t = m->tags; t; t = t->next) {
        if (strcmp(t->name, tag_name) == 0) return;
    }
    Tag *t = malloc(sizeof(Tag));
    t->name = strdup(tag_name);
    t->next = m->tags;
    m->tags = t;
}

void remove_tag(char *subject, char *tag_name) {
    Mail *m = find_mail_by_subject(subject);
    if (!m) return;
    Tag *prev = NULL;
    Tag *curr = m->tags;
    while (curr) {
        if (strcmp(curr->name, tag_name) == 0) {
            if (prev) prev->next = curr->next;
            else m->tags = curr->next;
            free(curr->name);
            free(curr);
            return;
        }
        prev = curr;
        curr = curr->next;
    }
}

void delete_mail(char *subject) {
    Mail *prev = NULL;
    Mail *curr = head;
    while (curr) {
        if (strcmp(curr->subject, subject) == 0) {
            if (prev) prev->next = curr->next;
            else head = curr->next;
            free(curr->sender);
            free(curr->subject);
            free(curr->body);
            Tag *t = curr->tags;
            while (t) {
                Tag *next = t->next;
                free(t->name);
                free(t);
                t = next;
            }
            free(curr);
            return;
        }
        prev = curr;
        curr = curr->next;
    }
}

void search_tag(char *tag_name) {
    for (Mail *m = head; m; m = m->next) {
        for (Tag *t = m->tags; t; t = t->next) {
            if (strcmp(t->name, tag_name) == 0) {
                printf("%s\n", m->subject);
                break;
            }
        }
    }
}

void report_mails() {
    for (Mail *m = head; m; m = m->next) {
        printf("%s %s %s", m->sender, m->subject, m->body);
        for (Tag *t = m->tags; t; t = t->next) {
            printf(" %s", t->name);
        }
        printf("\n");
    }
}

int main() {
    char cmd[20], a[100], b[100], c[1000];
    while (scanf("%19s", cmd) == 1 && strcmp(cmd, "END") != 0) {
        if (strcmp(cmd, "MAIL") == 0) {
            scanf("%99s%99s", a, b);
            fgets(c, sizeof(c), stdin);
            while (c[0] == ' ') memmove(c, c + 1, strlen(c));
            c[strcspn(c, "\n")] = 0;
            create_mail(a, b, c);
        } else if (strcmp(cmd, "TAG") == 0) {
            scanf("%99s%99s", a, b);
            add_tag(a, b);
        } else if (strcmp(cmd, "UNTAG") == 0) {
            scanf("%99s%99s", a, b);
            remove_tag(a, b);
        } else if (strcmp(cmd, "DELETE") == 0) {
            scanf("%99s", a);
            delete_mail(a);
        } else if (strcmp(cmd, "SEARCH") == 0) {
            scanf("%99s", a);
            search_tag(a);
        } else if (strcmp(cmd, "REPORT") == 0) {
            report_mails();
        }
    }
    return 0;
}