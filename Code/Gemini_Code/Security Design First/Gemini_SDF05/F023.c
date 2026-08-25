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

int main(void) {
    Message *mailbox = NULL;
    int count = 0;
    int capacity = 0;
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
        if (strncmp(line, "MAIL ", 5) == 0) {
            char *p = line + 5;
            while (*p == ' ') p++;
            char *sender = p;
            char *space1 = strchr(sender, ' ');
            if (!space1) continue;
            *space1 = '\0';
            char *subject = space1 + 1;
            while (*subject == ' ') subject++;
            char *space2 = strchr(subject, ' ');
            char *body = "";
            if (space2) {
                *space2 = '\0';
                body = space2 + 1;
                while (*body == ' ') body++;
            }
            if (count >= capacity) {
                int new_cap = capacity == 0 ? 4 : capacity * 2;
                Message *tmp = realloc(mailbox, new_cap * sizeof(Message));
                if (!tmp) return 1;
                mailbox = tmp;
                capacity = new_cap;
            }
            mailbox[count].sender = strdup(sender);
            mailbox[count].subject = strdup(subject);
            mailbox[count].body = strdup(body);
            mailbox[count].tags = NULL;
            mailbox[count].tag_count = 0;
            mailbox[count].tag_capacity = 0;
            count++;
        } else if (strncmp(line, "TAG ", 4) == 0) {
            char *p = line + 4;
            while (*p == ' ') p++;
            char *subject = p;
            char *space = strchr(subject, ' ');
            if (!space) continue;
            *space = '\0';
            char *tag = space + 1;
            while (*tag == ' ') tag++;
            int m_idx = -1;
            for (int i = 0; i < count; i++) {
                if (strcmp(mailbox[i].subject, subject) == 0) {
                    m_idx = i;
                    break;
                }
            }
            if (m_idx != -1) {
                Message *m = &mailbox[m_idx];
                if (m->tag_count >= m->tag_capacity) {
                    int new_tcap = m->tag_capacity == 0 ? 4 : m->tag_capacity * 2;
                    char **tmp = realloc(m->tags, new_tcap * sizeof(char *));
                    if (!tmp) return 1;
                    m->tags = tmp;
                    m->tag_capacity = new_tcap;
                }
                m->tags[m->tag_count] = strdup(tag);
                m->tag_count++;
            }
        } else if (strncmp(line, "UNTAG ", 6) == 0) {
            char *p = line + 6;
            while (*p == ' ') p++;
            char *subject = p;
            char *space = strchr(subject, ' ');
            if (!space) continue;
            *space = '\0';
            char *tag = space + 1;
            while (*tag == ' ') tag++;
            int m_idx = -1;
            for (int i = 0; i < count; i++) {
                if (strcmp(mailbox[i].subject, subject) == 0) {
                    m_idx = i;
                    break;
                }
            }
            if (m_idx != -1) {
                Message *m = &mailbox[m_idx];
                int t_idx = -1;
                for (int j = 0; j < m->tag_count; j++) {
                    if (strcmp(m->tags[j], tag) == 0) {
                        t_idx = j;
                        break;
                    }
                }
                if (t_idx != -1) {
                    free(m->tags[t_idx]);
                    for (int j = t_idx; j < m->tag_count - 1; j++) {
                        m->tags[j] = m->tags[j + 1];
                    }
                    m->tag_count--;
                }
            }
        } else if (strncmp(line, "DELETE ", 7) == 0) {
            char *subject = line + 7;
            while (*subject == ' ') subject++;
            int m_idx = -1;
            for (int i = 0; i < count; i++) {
                if (strcmp(mailbox[i].subject, subject) == 0) {
                    m_idx = i;
                    break;
                }
            }
            if (m_idx != -1) {
                free(mailbox[m_idx].sender);
                free(mailbox[m_idx].subject);
                free(mailbox[m_idx].body);
                for (int j = 0; j < mailbox[m_idx].tag_count; j++) {
                    free(mailbox[m_idx].tags[j]);
                }
                free(mailbox[m_idx].tags);
                for (int i = m_idx; i < count - 1; i++) {
                    mailbox[i] = mailbox[i + 1];
                }
                count--;
            }
        } else if (strncmp(line, "SEARCH ", 7) == 0) {
            char *tag = line + 7;
            while (*tag == ' ') tag++;
            for (int i = 0; i < count; i++) {
                int has_tag = 0;
                for (int j = 0; j < mailbox[i].tag_count; j++) {
                    if (strcmp(mailbox[i].tags[j], tag) == 0) {
                        has_tag = 1;
                        break;
                    }
                }
                if (has_tag) {
                    printf("%s\n", mailbox[i].subject);
                }
            }
        } else if (strcmp(line, "REPORT") == 0) {
            for (int i = 0; i < count; i++) {
                printf("From: %s Subject: %s Body: %s\n", mailbox[i].sender, mailbox[i].subject, mailbox[i].body);
            }
        }
    }
    free(line);
    for (int i = 0; i < count; i++) {
        free(mailbox[i].sender);
        free(mailbox[i].subject);
        free(mailbox[i].body);
        for (int j = 0; j < mailbox[i].tag_count; j++) {
            free(mailbox[i].tags[j]);
        }
        free(mailbox[i].tags);
    }
    free(mailbox);
    return 0;
}
