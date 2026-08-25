// F002.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Message {
    char *text;
    struct Message *next;
} Message;

typedef struct Tag {
    char *name;
    Message *msgs;
    Message *tail;
    struct Tag *next;
} Tag;

Tag *tags = NULL;

Tag *find_tag(const char *name) {
    Tag *curr = tags;
    while (curr) {
        if (strcmp(curr->name, name) == 0) return curr;
        curr = curr->next;
    }
    return NULL;
}

void add_message(const char *tag_name, const char *msg) {
    Tag *t = find_tag(tag_name);
    if (!t) {
        t = malloc(sizeof(Tag));
        t->name = strdup(tag_name);
        t->msgs = NULL;
        t->tail = NULL;
        t->next = tags;
        tags = t;
    }
    Message *m = malloc(sizeof(Message));
    m->text = strdup(msg);
    m->next = NULL;
    if (t->tail) t->tail->next = m;
    else t->msgs = m;
    t->tail = m;
}

void concat_messages(const char *t1_name, const char *t2_name, const char *new_tag) {
    Tag *t1 = find_tag(t1_name);
    Tag *t2 = find_tag(t2_name);
    if (!t1 || !t1->msgs || !t2 || !t2->msgs) return;
    
    size_t len = strlen(t1->msgs->text) + strlen(t2->msgs->text) + 2;
    char *combined = malloc(len);
    snprintf(combined, len, "%s %s", t1->msgs->text, t2->msgs->text);
    add_message(new_tag, combined);
    free(combined);
}

void remove_tag(const char *name) {
    Tag *curr = tags, *prev = NULL;
    while (curr) {
        if (strcmp(curr->name, name) == 0) {
            if (prev) prev->next = curr->next;
            else tags = curr->next;
            Message *m = curr->msgs;
            while (m) {
                Message *next = m->next;
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

void find_message(const char *name) {
    Tag *t = find_tag(name);
    if (!t || !t->msgs) return;
    for (char *c = t->msgs->text; *c; c++) {
        if (*c == ' ') putchar('_');
        else putchar(*c);
    }
    putchar('\n');
}

void report_tags() {
    Tag *curr = tags;
    while (curr) {
        Message *m = curr->msgs;
        while (m) {
            printf("%s %s\n", curr->name, m->text);
            m = m->next;
        }
        curr = curr->next;
    }
}

void free_all() {
    Tag *curr = tags;
    while (curr) {
        Tag *next_t = curr->next;
        Message *m = curr->msgs;
        while (m) {
            Message *next_m = m->next;
            free(m->text);
            free(m);
            m = next_m;
        }
        free(curr->name);
        free(curr);
        curr = next_t;
    }
}

int main() {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        if (strcmp(line, "END") == 0) break;
        
        char cmd[32], arg1[256], arg2[256], arg3[256];
        if (sscanf(line, "%31s", cmd) != 1) continue;
        
        if (strcmp(cmd, "ADD") == 0) {
            char *tag = strtok(line + 4, " ");
            char *msg = tag ? strchr(line + 4 + strlen(tag) + 1, ' ') : NULL;
            if (tag && msg) add_message(tag, msg + 1);
        } else if (strcmp(cmd, "CONCAT") == 0) {
            if (sscanf(line, "%*s %255s %255s %255s", arg1, arg2, arg3) == 3) {
                concat_messages(arg1, arg2, arg3);
            }
        } else if (strcmp(cmd, "REMOVE") == 0) {
            if (sscanf(line, "%*s %255s", arg1) == 1) remove_tag(arg1);
        } else if (strcmp(cmd, "FIND") == 0) {
            if (sscanf(line, "%*s %255s", arg1) == 1) find_message(arg1);
        } else if (strcmp(cmd, "REPORT") == 0) {
            report_tags();
        }
    }
    free_all();
    return 0;
}