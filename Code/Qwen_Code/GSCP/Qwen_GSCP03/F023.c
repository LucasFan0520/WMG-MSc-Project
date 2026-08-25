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

void free_tags(Tag *t) {
    while (t) { Tag *tmp = t; t = t->next; free(tmp->name); free(tmp); }
}

void free_list() {
    while (head) {
        Mail *m = head; head = head->next;
        free(m->sender); free(m->subject); free(m->body); free_tags(m->tags); free(m);
    }
}

int main() {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        char cmd[16], sender[256], subject[256], body[4096] = "", tag[256];
        if (sscanf(line, "%15s", cmd) != 1) continue;
        if (strcmp(cmd, "END") == 0) break;

        if (strcmp(cmd, "MAIL") == 0) {
            if (sscanf(line, "%*s %255s %255s %4095[^\n]", sender, subject, body) < 2) continue;
            Mail *m = malloc(sizeof(Mail));
            m->sender = strdup(sender); m->subject = strdup(subject); m->body = strdup(body);
            m->tags = NULL; m->next = head; head = m;
        } else if (strcmp(cmd, "TAG") == 0) {
            if (sscanf(line, "%*s %255s %255s", subject, tag) != 2) continue;
            for (Mail *m = head; m; m = m->next) {
                if (strcmp(m->subject, subject) == 0) {
                    Tag *t = malloc(sizeof(Tag));
                    t->name = strdup(tag); t->next = m->tags; m->tags = t;
                    break;
                }
            }
        } else if (strcmp(cmd, "UNTAG") == 0) {
            if (sscanf(line, "%*s %255s %255s", subject, tag) != 2) continue;
            for (Mail *m = head; m; m = m->next) {
                if (strcmp(m->subject, subject) == 0) {
                    Tag *prev = NULL, *curr = m->tags;
                    while (curr) {
                        if (strcmp(curr->name, tag) == 0) {
                            Tag *tmp = curr; curr = curr->next;
                            if (prev) prev->next = curr; else m->tags = curr;
                            free(tmp->name); free(tmp);
                            break;
                        }
                        prev = curr; curr = curr->next;
                    }
                    break;
                }
            }
        } else if (strcmp(cmd, "DELETE") == 0) {
            if (sscanf(line, "%*s %255s", subject) != 1) continue;
            Mail *prev = NULL, *curr = head;
            while (curr) {
                if (strcmp(curr->subject, subject) == 0) {
                    Mail *tmp = curr; curr = curr->next;
                    if (prev) prev->next = curr; else head = curr;
                    free(tmp->sender); free(tmp->subject); free(tmp->body); free_tags(tmp->tags); free(tmp);
                    break;
                }
                prev = curr; curr = curr->next;
            }
        } else if (strcmp(cmd, "SEARCH") == 0) {
            if (sscanf(line, "%*s %255s", tag) != 1) continue;
            for (Mail *m = head; m; m = m->next) {
                for (Tag *t = m->tags; t; t = t->next) {
                    if (strcmp(t->name, tag) == 0) {
                        printf("%s\n", m->subject);
                        break;
                    }
                }
            }
        } else if (strcmp(cmd, "REPORT") == 0) {
            for (Mail *m = head; m; m = m->next) {
                printf("%s %s %s", m->sender, m->subject, m->body);
                for (Tag *t = m->tags; t; t = t->next) printf(" [%s]", t->name);
                printf("\n");
            }
        }
    }
    free_list();
    return 0;
}