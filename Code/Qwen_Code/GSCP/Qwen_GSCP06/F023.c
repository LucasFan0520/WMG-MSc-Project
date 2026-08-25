// F023.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_LINE 4096

char* get_word(char** str) {
    while (**str && isspace((unsigned char)**str)) (*str)++;
    if (!**str) return NULL;
    char* start = *str;
    while (**str && !isspace((unsigned char)**str)) (*str)++;
    if (**str) { **str = '\0'; (*str)++; }
    return start;
}

typedef struct Tag {
    char* name;
    struct Tag* next;
} Tag;

typedef struct Mail {
    char* sender;
    char* subject;
    char* body;
    Tag* tags;
    struct Mail* next;
} Mail;

Mail* head = NULL;

Mail* find_mail_subj(const char* subj) {
    for (Mail* m = head; m; m = m->next)
        if (strcmp(m->subject, subj) == 0) return m;
    return NULL;
}

int main() {
    char line[MAX_LINE];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        if (strcmp(line, "END") == 0) break;
        char* p = line;
        char* cmd = get_word(&p);
        if (!cmd) continue;

        if (strcmp(cmd, "MAIL") == 0) {
            char* sender = get_word(&p);
            char* subj = get_word(&p);
            if (sender && subj) {
                Mail* n = malloc(sizeof(Mail));
                n->sender = strdup(sender);
                n->subject = strdup(subj);
                n->body = strdup(p);
                n->tags = NULL;
                n->next = head;
                head = n;
            }
        } else if (strcmp(cmd, "TAG") == 0) {
            char* subj = get_word(&p);
            char* tag = get_word(&p);
            Mail* m = find_mail_subj(subj);
            if (m && tag) {
                Tag* n = malloc(sizeof(Tag));
                n->name = strdup(tag);
                n->next = m->tags;
                m->tags = n;
            }
        } else if (strcmp(cmd, "UNTAG") == 0) {
            char* subj = get_word(&p);
            char* tag = get_word(&p);
            Mail* m = find_mail_subj(subj);
            if (m) {
                Tag* prev = NULL;
                for (Tag* t = m->tags; t; prev = t, t = t->next) {
                    if (strcmp(t->name, tag) == 0) {
                        if (prev) prev->next = t->next;
                        else m->tags = t->next;
                        free(t->name);
                        free(t);
                        break;
                    }
                }
            }
        } else if (strcmp(cmd, "DELETE") == 0) {
            char* subj = get_word(&p);
            Mail* prev = NULL;
            for (Mail* m = head; m; prev = m, m = m->next) {
                if (strcmp(m->subject, subj) == 0) {
                    if (prev) prev->next = m->next;
                    else head = m->next;
                    free(m->sender);
                    free(m->subject);
                    free(m->body);
                    while (m->tags) {
                        Tag* temp = m->tags;
                        m->tags = m->tags->next;
                        free(temp->name);
                        free(temp);
                    }
                    free(m);
                    break;
                }
            }
        } else if (strcmp(cmd, "SEARCH") == 0) {
            char* tag = get_word(&p);
            for (Mail* m = head; m; m = m->next) {
                for (Tag* t = m->tags; t; t = t->next) {
                    if (strcmp(t->name, tag) == 0) {
                        printf("%s\n", m->subject);
                        break;
                    }
                }
            }
        } else if (strcmp(cmd, "REPORT") == 0) {
            for (Mail* m = head; m; m = m->next) {
                printf("%s %s %s\n", m->sender, m->subject, m->body);
            }
        }
    }
    return 0;
}