// F023.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Tag {
    char *name;
    struct Tag *next;
} Tag;

typedef struct Msg {
    char *sender, *subject, *body;
    Tag *tags;
    struct Msg *next;
} Msg;

Msg *head = NULL;

void read_rest(char *buf, int size) {
    if (!fgets(buf, size, stdin)) { buf[0] = '\0'; return; }
    if (buf[0] == ' ') memmove(buf, buf + 1, strlen(buf));
    size_t len = strlen(buf);
    if (len > 0 && buf[len-1] == '\n') buf[len-1] = '\0';
}

Msg *find_first(const char *subject) {
    for (Msg *m = head; m; m = m->next) if (strcmp(m->subject, subject) == 0) return m;
    return NULL;
}

int main() {
    char cmd[20], sender[256], subject[256], body[1024], tag[256];
    while (scanf("%s", cmd) == 1 && strcmp(cmd, "END") != 0) {
        if (strcmp(cmd, "MAIL") == 0) {
            scanf("%s", sender); scanf("%s", subject); read_rest(body, sizeof(body));
            Msg *m = malloc(sizeof(Msg));
            m->sender = strdup(sender); m->subject = strdup(subject); m->body = strdup(body); m->tags = NULL; m->next = head; head = m;
        } else if (strcmp(cmd, "TAG") == 0) {
            scanf("%s %s", subject, tag);
            Msg *m = find_first(subject);
            if (m) {
                Tag *t = malloc(sizeof(Tag));
                t->name = strdup(tag); t->next = m->tags; m->tags = t;
            }
        } else if (strcmp(cmd, "UNTAG") == 0) {
            scanf("%s %s", subject, tag);
            Msg *m = find_first(subject);
            if (m) {
                Tag *prev = NULL, *curr = m->tags;
                while (curr) {
                    if (strcmp(curr->name, tag) == 0) {
                        if (prev) prev->next = curr->next; else m->tags = curr->next;
                        free(curr->name); free(curr); break;
                    }
                    prev = curr; curr = curr->next;
                }
            }
        } else if (strcmp(cmd, "DELETE") == 0) {
            scanf("%s", subject);
            Msg *prev = NULL, *curr = head;
            while (curr) {
                if (strcmp(curr->subject, subject) == 0) {
                    if (prev) prev->next = curr->next; else head = curr->next;
                    while (curr->tags) { Tag *nt = curr->tags->next; free(curr->tags->name); free(curr->tags); curr->tags = nt; }
                    free(curr->sender); free(curr->subject); free(curr->body); free(curr); break;
                }
                prev = curr; curr = curr->next;
            }
        } else if (strcmp(cmd, "SEARCH") == 0) {
            scanf("%s", tag);
            for (Msg *m = head; m; m = m->next) {
                for (Tag *t = m->tags; t; t = t->next) {
                    if (strcmp(t->name, tag) == 0) { printf("%s\n", m->subject); break; }
                }
            }
        } else if (strcmp(cmd, "REPORT") == 0) {
            for (Msg *m = head; m; m = m->next) {
                printf("%s %s %s", m->sender, m->subject, m->body);
                for (Tag *t = m->tags; t; t = t->next) printf(" [%s]", t->name);
                printf("\n");
            }
        }
    }
    return 0;
}