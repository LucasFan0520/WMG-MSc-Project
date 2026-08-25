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

int main() {
    char cmd[20];
    char sender[100];
    char subject[100];
    char body[1000];
    char tag[100];
    while (scanf("%s", cmd) == 1 && strcmp(cmd, "END")) {
        if (!strcmp(cmd, "MAIL")) {
            scanf("%s %s", sender, subject);
            fgets(body, 1000, stdin);
            body[strcspn(body, "\n")] = 0;
            if (body[0] == ' ') memmove(body, body + 1, strlen(body));
            Mail *m = malloc(sizeof(Mail));
            m->sender = strdup(sender);
            m->subject = strdup(subject);
            m->body = strdup(body);
            m->tags = NULL;
            m->next = head;
            head = m;
        } else if (!strcmp(cmd, "TAG")) {
            scanf("%s %s", subject, tag);
            for (Mail *m = head; m; m = m->next) {
                if (!strcmp(m->subject, subject)) {
                    Tag *t = malloc(sizeof(Tag));
                    t->name = strdup(tag);
                    t->next = m->tags;
                    m->tags = t;
                    break;
                }
            }
        } else if (!strcmp(cmd, "UNTAG")) {
            scanf("%s %s", subject, tag);
            for (Mail *m = head; m; m = m->next) {
                if (!strcmp(m->subject, subject)) {
                    Tag *p = m->tags;
                    Tag *prev = NULL;
                    while (p) {
                        if (!strcmp(p->name, tag)) {
                            if (prev) prev->next = p->next;
                            else m->tags = p->next;
                            free(p->name);
                            free(p);
                            break;
                        }
                        prev = p;
                        p = p->next;
                    }
                    break;
                }
            }
        } else if (!strcmp(cmd, "DELETE")) {
            scanf("%s", subject);
            Mail *p = head;
            Mail *prev = NULL;
            while (p) {
                if (!strcmp(p->subject, subject)) {
                    if (prev) prev->next = p->next;
                    else head = p->next;
                    Tag *t = p->tags;
                    while (t) {
                        Tag *tmp = t;
                        t = t->next;
                        free(tmp->name);
                        free(tmp);
                    }
                    free(p->sender);
                    free(p->subject);
                    free(p->body);
                    free(p);
                    break;
                }
                prev = p;
                p = p->next;
            }
        } else if (!strcmp(cmd, "SEARCH")) {
            scanf("%s", tag);
            for (Mail *m = head; m; m = m->next) {
                for (Tag *t = m->tags; t; t = t->next) {
                    if (!strcmp(t->name, tag)) {
                        printf("%s\n", m->subject);
                        break;
                    }
                }
            }
        } else if (!strcmp(cmd, "REPORT")) {
            for (Mail *m = head; m; m = m->next) {
                printf("%s %s %s\n", m->sender, m->subject, m->body);
            }
        }
    }
    return 0;
}