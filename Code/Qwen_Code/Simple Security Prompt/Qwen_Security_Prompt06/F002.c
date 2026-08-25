// F002.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *read_line() {
    size_t cap = 256, len = 0;
    char *buf = malloc(cap);
    if (!buf) return NULL;
    while (1) {
        if (!fgets(buf + len, cap - len, stdin)) {
            if (len == 0) { free(buf); return NULL; }
            break;
        }
        len = strlen(buf);
        if (len > 0 && buf[len-1] == '\n') { buf[len-1] = '\0'; break; }
        if (len == cap - 1) {
            cap *= 2;
            char *tmp = realloc(buf, cap);
            if (!tmp) { free(buf); return NULL; }
            buf = tmp;
        }
    }
    return buf;
}

void print_underscores(const char *s) {
    while (*s) {
        putchar(*s == ' ' ? '_' : *s);
        s++;
    }
    putchar('\n');
}

typedef struct Msg {
    char *text;
    struct Msg *next;
} Msg;

typedef struct Tag {
    char *name;
    Msg *msgs;
    Msg *tail;
    struct Tag *next;
} Tag;

Tag *tags = NULL;

Tag *find_tag(const char *name) {
    for (Tag *t = tags; t; t = t->next) {
        if (strcmp(t->name, name) == 0) return t;
    }
    return NULL;
}

void add_msg(Tag *t, const char *text) {
    Msg *m = malloc(sizeof(Msg));
    m->text = strdup(text);
    m->next = NULL;
    if (t->tail) t->tail->next = m;
    else t->msgs = m;
    t->tail = m;
}

void add_tag_msg(const char *tag, const char *msg) {
    Tag *t = find_tag(tag);
    if (!t) {
        t = malloc(sizeof(Tag));
        t->name = strdup(tag);
        t->msgs = t->tail = NULL;
        t->next = tags;
        tags = t;
    }
    add_msg(t, msg);
}

void concat_tags(const char *t1, const char *t2, const char *nt) {
    Tag *tag1 = find_tag(t1);
    Tag *tag2 = find_tag(t2);
    if (!tag1 || !tag1->msgs || !tag2 || !tag2->msgs) return;
    size_t len = strlen(tag1->msgs->text) + 1 + strlen(tag2->msgs->text) + 1;
    char *res = malloc(len);
    snprintf(res, len, "%s %s", tag1->msgs->text, tag2->msgs->text);
    add_tag_msg(nt, res);
    free(res);
}

void remove_tag(const char *name) {
    Tag *prev = NULL, *curr = tags;
    while (curr) {
        if (strcmp(curr->name, name) == 0) {
            if (prev) prev->next = curr->next;
            else tags = curr->next;
            Msg *m = curr->msgs;
            while (m) {
                Msg *next = m->next;
                free(m->text);
                free(m);
                m = next;
            }
            free(curr->name);
            free(curr);
            return;
        }
        prev = curr;
        curr = curr->next;
    }
}

int main() {
    char *line;
    while ((line = read_line())) {
        if (strcmp(line, "END") == 0) break;
        char cmd[16] = {0}, arg1[256] = {0}, arg2[256] = {0}, arg3[256] = {0};
        sscanf(line, "%15s %255s %255s %255s", cmd, arg1, arg2, arg3);
        char *text = strchr(line, ' ');
        if (text) {
            text++;
            while (*text == ' ') text++;
        }
        
        if (strcmp(cmd, "ADD") == 0 && text) {
            add_tag_msg(arg1, text);
        } else if (strcmp(cmd, "CONCAT") == 0) {
            concat_tags(arg1, arg2, arg3);
        } else if (strcmp(cmd, "REMOVE") == 0) {
            remove_tag(arg1);
        } else if (strcmp(cmd, "FIND") == 0) {
            Tag *t = find_tag(arg1);
            if (t && t->msgs) print_underscores(t->msgs->text);
        } else if (strcmp(cmd, "REPORT") == 0) {
            for (Tag *t = tags; t; t = t->next) {
                for (Msg *m = t->msgs; m; m = m->next) {
                    printf("%s ", t->name);
                    print_underscores(m->text);
                }
            }
        }
        free(line);
    }
    
    Tag *t = tags;
    while (t) {
        Tag *next_t = t->next;
        Msg *m = t->msgs;
        while (m) {
            Msg *next_m = m->next;
            free(m->text);
            free(m);
            m = next_m;
        }
        free(t->name);
        free(t);
        t = next_t;
    }
    return 0;
}