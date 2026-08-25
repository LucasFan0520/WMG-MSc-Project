// F023.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *read_line(FILE *fp) {
    size_t cap = 16;
    size_t len = 0;
    char *buf = malloc(cap);
    if (!buf) return NULL;
    int c;
    while ((c = fgetc(fp)) != EOF && c != '\n') {
        if (len + 1 >= cap) {
            cap *= 2;
            char *nb = realloc(buf, cap);
            if (!nb) {
                free(buf);
                return NULL;
            }
            buf = nb;
        }
        buf[len++] = c;
    }
    if (len == 0 && c == EOF) {
        free(buf);
        return NULL;
    }
    buf[len] = '\0';
    if (len > 0 && buf[len-1] == '\r') {
        buf[len-1] = '\0';
    }
    return buf;
}

typedef struct {
    char *sender;
    char *subject;
    char *body;
    char **tags;
    int t_count;
    int t_cap;
} Mail;

int main(void) {
    Mail *mails = NULL;
    int m_count = 0;
    int m_cap = 0;
    while (1) {
        char *line = read_line(stdin);
        if (!line) break;
        if (strcmp(line, "END") == 0) {
            free(line);
            break;
        }
        if (strncmp(line, "MAIL ", 5) == 0) {
            char *p = line + 5;
            while (*p == ' ') p++;
            char *sender = p;
            while (*p && *p != ' ') p++;
            if (*p == ' ') {
                *p = '\0';
                p++;
                while (*p == ' ') p++;
                char *subject = p;
                while (*p && *p != ' ') p++;
                if (*p == ' ') {
                    *p = '\0';
                    char *body = p + 1;
                    while (*body == ' ') body++;
                    if (m_count >= m_cap) {
                        m_cap = m_cap == 0 ? 16 : m_cap * 2;
                        mails = realloc(mails, m_cap * sizeof(Mail));
                    }
                    mails[m_count].sender = strdup(sender);
                    mails[m_count].subject = strdup(subject);
                    mails[m_count].body = strdup(body);
                    mails[m_count].tags = NULL;
                    mails[m_count].t_count = 0;
                    mails[m_count].t_cap = 0;
                    m_count++;
                }
            }
        } else if (strncmp(line, "TAG ", 4) == 0) {
            char *p = line + 4;
            while (*p == ' ') p++;
            char *subject = p;
            while (*p && *p != ' ') p++;
            if (*p == ' ') {
                *p = '\0';
                char *tag = p + 1;
                while (*tag == ' ') tag++;
                for (int i = 0; i < m_count; i++) {
                    if (strcmp(mails[i].subject, subject) == 0) {
                        Mail *m = &mails[i];
                        if (m->t_count >= m->t_cap) {
                            m->t_cap = m->t_cap == 0 ? 16 : m->t_cap * 2;
                            m->tags = realloc(m->tags, m->t_cap * sizeof(char *));
                        }
                        m->tags[m->t_count] = strdup(tag);
                        m->t_count++;
                        break;
                    }
                }
            }
        } else if (strncmp(line, "UNTAG ", 6) == 0) {
            char *p = line + 6;
            while (*p == ' ') p++;
            char *subject = p;
            while (*p && *p != ' ') p++;
            if (*p == ' ') {
                *p = '\0';
                char *tag = p + 1;
                while (*tag == ' ') tag++;
                for (int i = 0; i < m_count; i++) {
                    if (strcmp(mails[i].subject, subject) == 0) {
                        Mail *m = &mails[i];
                        int idx = -1;
                        for (int j = 0; j < m->t_count; j++) {
                            if (strcmp(m->tags[j], tag) == 0) {
                                idx = j;
                                break;
                            }
                        }
                        if (idx != -1) {
                            free(m->tags[idx]);
                            for (int j = idx; j < m->t_count - 1; j++) {
                                m->tags[j] = m->tags[j + 1];
                            }
                            m->t_count--;
                        }
                        break;
                    }
                }
            }
        } else if (strncmp(line, "DELETE ", 7) == 0) {
            char *subject = line + 7;
            while (*subject == ' ') subject++;
            int idx = -1;
            for (int i = 0; i < m_count; i++) {
                if (strcmp(mails[i].subject, subject) == 0) {
                    idx = i;
                    break;
                }
            }
            if (idx != -1) {
                free(mails[idx].sender);
                free(mails[idx].subject);
                free(mails[idx].body);
                for (int j = 0; j < mails[idx].t_count; j++) free(mails[idx].tags[j]);
                free(mails[idx].tags);
                for (int i = idx; i < m_count - 1; i++) {
                    mails[i] = mails[i + 1];
                }
                m_count--;
            }
        } else if (strncmp(line, "SEARCH ", 7) == 0) {
            char *tag = line + 7;
            while (*tag == ' ') tag++;
            for (int i = 0; i < m_count; i++) {
                int found = 0;
                for (int j = 0; j < mails[i].t_count; j++) {
                    if (strcmp(mails[i].tags[j], tag) == 0) {
                        found = 1;
                        break;
                    }
                }
                if (found) {
                    printf("%s\n", mails[i].subject);
                }
            }
        } else if (strcmp(line, "REPORT") == 0) {
            for (int i = 0; i < m_count; i++) {
                printf("From: %s Subject: %s Body: %s\n", mails[i].sender, mails[i].subject, mails[i].body);
            }
        }
        free(line);
    }
    for (int i = 0; i < m_count; i++) {
        free(mails[i].sender);
        free(mails[i].subject);
        free(mails[i].body);
        for (int j = 0; j < mails[i].t_count; j++) free(mails[i].tags[j]);
        free(mails[i].tags);
    }
    free(mails);
    return 0;
}
