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
    Message *messages;
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
        if (!t) exit(EXIT_FAILURE);
        t->name = strdup(tag_name);
        t->messages = NULL;
        t->next = tags;
        tags = t;
    }
    Message *m = malloc(sizeof(Message));
    if (!m) exit(EXIT_FAILURE);
    m->text = strdup(msg);
    m->next = NULL;
    if (!t->messages) {
        t->messages = m;
    } else {
        Message *curr = t->messages;
        while (curr->next) curr = curr->next;
        curr->next = m;
    }
}

void concat_messages(const char *t1_name, const char *t2_name, const char *new_tag) {
    Tag *t1 = find_tag(t1_name);
    Tag *t2 = find_tag(t2_name);
    if (!t1 || !t1->messages || !t2 || !t2->messages) return;
    
    size_t len1 = strlen(t1->messages->text);
    size_t len2 = strlen(t2->messages->text);
    char *combined = malloc(len1 + len2 + 2);
    if (!combined) exit(EXIT_FAILURE);
    strcpy(combined, t1->messages->text);
    combined[len1] = ' ';
    strcpy(combined + len1 + 1, t2->messages->text);
    
    add_message(new_tag, combined);
    free(combined);
}

void remove_tag(const char *tag_name) {
    Tag *curr = tags;
    Tag *prev = NULL;
    while (curr) {
        if (strcmp(curr->name, tag_name) == 0) {
            if (prev) prev->next = curr->next;
            else tags = curr->next;
            Message *m = curr->messages;
            while (m) {
                Message *next_m = m->next;
                free(m->text);
                free(m);
                m = next_m;
            }
            free(curr->name);
            free(curr);
            return;
        }
        prev = curr;
        curr = curr->next;
    }
}

void find_message(const char *tag_name) {
    Tag *t = find_tag(tag_name);
    if (!t || !t->messages) return;
    for (int i = 0; t->messages->text[i]; i++) {
        if (t->messages->text[i] == ' ') putchar('_');
        else putchar(t->messages->text[i]);
    }
    putchar('\n');
}

void report_tags() {
    Tag *curr = tags;
    while (curr) {
        if (curr->messages) {
            printf("%s %s\n", curr->name, curr->messages->text);
        }
        curr = curr->next;
    }
}

void free_all() {
    Tag *curr = tags;
    while (curr) {
        Tag *next_t = curr->next;
        Message *m = curr->messages;
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
        
        char cmd[32] = {0};
        char arg1[256] = {0};
        char arg2[256] = {0};
        char arg3[256] = {0};
        char text[3500] = {0};
        
        int matched = sscanf(line, "%31s %255s %255s %255s %[^\n]", cmd, arg1, arg2, arg3, text);
        
        if (strcmp(cmd, "ADD") == 0 && matched >= 3) {
            char *msg_start = strchr(line, ' ');
            if (msg_start) {
                msg_start = strchr(msg_start + 1, ' ');
                if (msg_start) add_message(arg1, msg_start + 1);
            }
        } else if (strcmp(cmd, "CONCAT") == 0 && matched >= 3) {
            concat_messages(arg1, arg2, arg3);
        } else if (strcmp(cmd, "REMOVE") == 0) {
            remove_tag(arg1);
        } else if (strcmp(cmd, "FIND") == 0) {
            find_message(arg1);
        } else if (strcmp(cmd, "REPORT") == 0) {
            report_tags();
        }
    }
    free_all();
    return 0;
}