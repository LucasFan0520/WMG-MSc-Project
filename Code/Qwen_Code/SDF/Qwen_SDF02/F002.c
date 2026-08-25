// F002.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_LINE 4096

typedef struct Msg {
    char *text;
    struct Msg *next;
} Msg;

typedef struct Tag {
    char *name;
    Msg *msgs;
    struct Tag *next;
} Tag;

Tag *tags = NULL;

Tag* find_tag(const char *name) {
    for (Tag *t = tags; t; t = t->next)
        if (strcmp(t->name, name) == 0) return t;
    return NULL;
}

void print_text(const char *text) {
    for (const char *p = text; *p; p++) putchar(*p == ' ' ? '_' : *p);
    putchar('\n');
}

int main() {
    char line[MAX_LINE];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        if (strcmp(line, "END") == 0) break;
        
        char cmd[32];
        if (sscanf(line, "%31s", cmd) != 1) continue;
        char *rest = line + strlen(cmd);
        while (*rest == ' ') rest++;
        
        if (strcmp(cmd, "ADD") == 0) {
            char tag[256];
            int n = 0;
            if (sscanf(rest, "%255s%n", tag, &n) != 1) continue;
            char *msg = rest + n;
            while (*msg == ' ') msg++;
            Tag *t = find_tag(tag);
            if (!t) {
                t = malloc(sizeof(Tag));
                if (!t) continue;
                t->name = strdup(tag);
                t->msgs = NULL;
                t->next = tags;
                tags = t;
            }
            Msg *m = malloc(sizeof(Msg));
            if (!m) continue;
            m->text = strdup(msg);
            m->next = t->msgs;
            t->msgs = m;
        } else if (strcmp(cmd, "CONCAT") == 0) {
            char t1[256], t2[256], t3[256];
            if (sscanf(rest, "%255s %255s %255s", t1, t2, t3) != 3) continue;
            Tag *tag1 = find_tag(t1);
            Tag *tag2 = find_tag(t2);
            if (!tag1 || !tag1->msgs || !tag2 || !tag2->msgs) continue;
            
            Tag *new_tag = find_tag(t3);
            if (!new_tag) {
                new_tag = malloc(sizeof(Tag));
                if (!new_tag) continue;
                new_tag->name = strdup(t3);
                new_tag->msgs = NULL;
                new_tag->next = tags;
                tags = new_tag;
            }
            
            size_t len1 = strlen(tag1->msgs->text);
            size_t len2 = strlen(tag2->msgs->text);
            char *combined = malloc(len1 + 1 + len2 + 1);
            if (!combined) continue;
            memcpy(combined, tag1->msgs->text, len1);
            combined[len1] = ' ';
            memcpy(combined + len1 + 1, tag2->msgs->text, len2 + 1);
            
            Msg *m = malloc(sizeof(Msg));
            if (!m) { free(combined); continue; }
            m->text = combined;
            m->next = new_tag->msgs;
            new_tag->msgs = m;
        } else if (strcmp(cmd, "REMOVE") == 0) {
            char tag[256];
            if (sscanf(rest, "%255s", tag) != 1) continue;
            Tag *prev = NULL;
            for (Tag *t = tags; t; prev = t, t = t->next) {
                if (strcmp(t->name, tag) == 0) {
                    if (prev) prev->next = t->next;
                    else tags = t->next;
                    while (t->msgs) {
                        Msg *m = t->msgs;
                        t->msgs = m->next;
                        free(m->text);
                        free(m);
                    }
                    free(t->name);
                    free(t);
                    break;
                }
            }
        } else if (strcmp(cmd, "FIND") == 0) {
            char tag[256];
            if (sscanf(rest, "%255s", tag) != 1) continue;
            Tag *t = find_tag(tag);
            if (t && t->msgs) {
                Msg *last = t->msgs;
                while (last->next) last = last->next;
                print_text(last->text);
            }
        } else if (strcmp(cmd, "REPORT") == 0) {
            for (Tag *t = tags; t; t = t->next) {
                if (t->msgs) {
                    Msg *last = t->msgs;
                    while (last->next) last = last->next;
                    printf("%s ", t->name);
                    print_text(last->text);
                }
            }
        }
    }
    while (tags) {
        Tag *t = tags;
        tags = tags->next;
        while (t->msgs) {
            Msg *m = t->msgs;
            t->msgs = m->next;
            free(m->text);
            free(m);
        }
        free(t->name);
        free(t);
    }
    return 0;
}