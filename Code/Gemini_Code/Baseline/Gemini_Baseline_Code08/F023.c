// F023.c
#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *sender;
    char *subject;
    char *body;
    char **tags;
    int tag_count;
    int tag_capacity;
} Message;

Message *messages = NULL;
int msg_count = 0;
int msg_capacity = 0;

void add_mail(const char *sender, const char *subject, const char *body) {
    if (msg_count >= msg_capacity) {
        msg_capacity = msg_capacity == 0 ? 4 : msg_capacity * 2;
        messages = realloc(messages, msg_capacity * sizeof(Message));
    }
    messages[msg_count].sender = strdup(sender);
    messages[msg_count].subject = strdup(subject);
    messages[msg_count].body = strdup(body);
    messages[msg_count].tags = NULL;
    messages[msg_count].tag_count = 0;
    messages[msg_count].tag_capacity = 0;
    msg_count++;
}

int find_first_by_subject(const char *subj) {
    for (int i = 0; i < msg_count; i++) {
        if (strcmp(messages[i].subject, subj) == 0) {
            return i;
        }
    }
    return -1;
}

void tag_mail(const char *subj, const char *tag) {
    int idx = find_first_by_subject(subj);
    if (idx == -1) return;
    Message *m = &messages[idx];
    if (m->tag_count >= m->tag_capacity) {
        m->tag_capacity = m->tag_capacity == 0 ? 4 : m->tag_capacity * 2;
        m->tags = realloc(m->tags, m->tag_capacity * sizeof(char *));
    }
    m->tags[m->tag_count] = strdup(tag);
    m->tag_count++;
}

void untag_mail(const char *subj, const char *tag) {
    int idx = find_first_by_subject(subj);
    if (idx == -1) return;
    Message *m = &messages[idx];
    for (int i = 0; i < m->tag_count; i++) {
        if (strcmp(m->tags[i], tag) == 0) {
            free(m->tags[i]);
            for (int j = i; j < m->tag_count - 1; j++) {
                m->tags[j] = m->tags[j + 1];
            }
            m->tag_count--;
            break;
        }
    }
}

void delete_mail(const char *subj) {
    int idx = find_first_by_subject(subj);
    if (idx == -1) return;
    free(messages[idx].sender);
    free(messages[idx].subject);
    free(messages[idx].body);
    for (int i = 0; i < messages[idx].tag_count; i++) {
        free(messages[idx].tags[i]);
    }
    free(messages[idx].tags);
    for (int i = idx; i < msg_count - 1; i++) {
        messages[i] = messages[i + 1];
    }
    msg_count--;
}

void search_tag(const char *tag) {
    for (int i = 0; i < msg_count; i++) {
        int found = 0;
        for (int j = 0; j < messages[i].tag_count; j++) {
            if (strcmp(messages[i].tags[j], tag) == 0) {
                found = 1;
                break;
            }
        }
        if (found) {
            printf("%s\n", messages[i].subject);
        }
    }
}

void report_mailbox(void) {
    for (int i = 0; i < msg_count; i++) {
        printf("%s %s %s", messages[i].sender, messages[i].subject, messages[i].body);
        for (int j = 0; j < messages[i].tag_count; j++) {
            printf(" %s", messages[i].tags[j]);
        }
        printf("\n");
    }
}

int main(void) {
    char *line = NULL;
    size_t len = 0;
    ssize_t read;
    while ((read = getline(&line, &len, stdin)) != -1) {
        if (read > 0 && line[read - 1] == '\n') {
            line[read - 1] = '\0';
        }
        if (strcmp(line, "END") == 0) {
            break;
        }
        char *cmd = strtok(line, " ");
        if (!cmd) continue;
        if (strcmp(cmd, "MAIL") == 0) {
            char *sender = strtok(NULL, " ");
            char *subject = strtok(NULL, " ");
            char *body = strtok(NULL, "");
            if (sender && subject && body) add_mail(sender, subject, body);
        } else if (strcmp(cmd, "TAG") == 0) {
            char *subj = strtok(NULL, " ");
            char *tag = strtok(NULL, "");
            if (subj && tag) tag_mail(subj, tag);
        } else if (strcmp(cmd, "UNTAG") == 0) {
            char *subj = strtok(NULL, " ");
            char *tag = strtok(NULL, "");
            if (subj && tag) untag_mail(subj, tag);
        } else if (strcmp(cmd, "DELETE") == 0) {
            char *subj = strtok(NULL, "");
            if (subj) delete_mail(subj);
        } else if (strcmp(cmd, "SEARCH") == 0) {
            char *tag = strtok(NULL, "");
            if (tag) search_tag(tag);
        } else if (strcmp(cmd, "REPORT") == 0) {
            report_mailbox();
        }
    }
    free(line);
    for (int i = 0; i < msg_count; i++) {
        free(messages[i].sender);
        free(messages[i].subject);
        free(messages[i].body);
        for (int j = 0; j < messages[i].tag_count; j++) {
            free(messages[i].tags[j]);
        }
        free(messages[i].tags);
    }
    free(messages);
    return 0;
}
