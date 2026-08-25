/* F002.c */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Message {
    char *text;
    struct Message *next;
} Message;

typedef struct Tag {
    char *name;
    Message *messages;
    struct Tag *next;
} Tag;

Tag *tag_list = NULL;

Tag *find_tag(const char *name) {
    Tag *t = tag_list;
    while (t) {
        if (strcmp(t->name, name) == 0) return t;
        t = t->next;
    }
    return NULL;
}

void free_messages(Message *m) {
    while (m) {
        Message *tmp = m;
        m = m->next;
        free(tmp->text);
        free(tmp);
    }
}

void add_message(const char *tag, const char *msg) {
    Tag *t = find_tag(tag);
    if (!t) {
        t = malloc(sizeof(Tag));
        if (!t) return;
        t->name = strdup(tag);
        t->messages = NULL;
        t->next = tag_list;
        tag_list = t;
    }
    Message *m = malloc(sizeof(Message));
    if (!m) return;
    m->text = strdup(msg);
    m->next = t->messages;
    t->messages = m;
}

void concat_tags(const char *t1, const char *t2, const char *newtag) {
    Tag *a = find_tag(t1), *b = find_tag(t2);
    if (!a || !b || !a->messages || !b->messages) return;
    char *combined = malloc(strlen(a->messages->text) + strlen(b->messages->text) + 2);
    if (!combined) return;
    sprintf(combined, "%s %s", a->messages->text, b->messages->text);
    add_message(newtag, combined);
    free(combined);
}

void remove_tag(const char *tag) {
    Tag *t = tag_list, *prev = NULL;
    while (t) {
        if (strcmp(t->name, tag) == 0) {
            if (prev) prev->next = t->next;
            else tag_list = t->next;
            free_messages(t->messages);
            free(t->name);
            free(t);
            return;
        }
        prev = t;
        t = t->next;
    }
}

void find_tag(const char *tag) {
    Tag *t = find_tag(tag);
    if (!t || !t->messages) {
        printf("NOT_FOUND\n");
        return;
    }
    char *out = strdup(t->messages->text);
    for (char *c = out; *c; c++) if (*c == ' ') *c = '_';
    printf("%s\n", out);
    free(out);
}

void report_all(void) {
    Tag *t = tag_list;
    while (t) {
        Message *m = t->messages;
        while (m) {
            printf("%s %s\n", t->name, m->text);
            m = m->next;
        }
        t = t->next;
    }
}

int main(void) {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        char cmd[16], arg1[256], arg2[256], arg3[256];
        int n = sscanf(line, "%15s %255s %255s %255s", cmd, arg1, arg2, arg3);
        if (n < 2) continue;
        if (strcmp(cmd, "ADD") == 0) {
            char *msg = line + strlen(cmd) + strlen(arg1) + 2;
            while (*msg == ' ') msg++;
            add_message(arg1, msg);
        } else if (strcmp(cmd, "CONCAT") == 0 && n == 4) {
            concat_tags(arg1, arg2, arg3);
        } else if (strcmp(cmd, "REMOVE") == 0) {
            remove_tag(arg1);
        } else if (strcmp(cmd, "FIND") == 0) {
            find_tag(arg1);
        } else if (strcmp(cmd, "REPORT") == 0) {
            report_all();
        }
    }
    while (tag_list) {
        Tag *tmp = tag_list;
        tag_list = tag_list->next;
        free_messages(tmp->messages);
        free(tmp->name);
        free(tmp);
    }
    return 0;
}