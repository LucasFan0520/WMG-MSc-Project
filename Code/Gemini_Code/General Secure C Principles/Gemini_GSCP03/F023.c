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
    if (c == EOF && len == 0) {
        free(buf);
        return NULL;
    }
    buf[len] = '\0';
    return buf;
}

typedef struct {
    char *sender;
    char *subject;
    char *body;
    char **tags;
    int tag_count;
    int tag_cap;
} Message;

int main() {
    Message *mails = NULL;
    int m_count = 0;
    int m_cap = 0;
    while (1) {
        char *line = read_line(stdin);
        if (!line) break;
        char *p = line;
        while (*p == ' ') p++;
        if (*p == '\0') {
            free(line);
            continue;
        }
        char *cmd = p;
        while (*p && *p != ' ') p++;
        if (*p) {
            *p = '\0';
            p++;
        }
        if (strcmp(cmd, "END") == 0) {
            free(line);
            break;
        }
        if (strcmp(cmd, "REPORT") == 0) {
            for (int i = 0; i < m_count; i++) {
                printf("%s %s %s", mails[i].sender, mails[i].subject, mails[i].body);
                for (int j = 0; j < mails[i].tag_count; j++) {
                    printf(" #%s", mails[i].tags[j]);
                }
                printf("\n");
            }
            free(line);
            continue;
        }
        while (*p == ' ') p++;
        if (strcmp(cmd, "MAIL") == 0) {
            char *sender = p;
            while (*p && *p != ' ') p++;
            if (*p) {
                *p = '\0';
                p++;
            }
            while (*p == ' ') p++;
            char *subject = p;
            while (*p && *p != ' ') p++;
            if (*p) {
                *p = '\0';
                p++;
            }
            while (*p == ' ') p++;
            char *body = p;
            if (m_count >= m_cap) {
                m_cap = m_cap == 0 ? 16 : m_cap * 2;
                Message *nb = realloc(mails, m_cap * sizeof(Message));
                if (nb) mails = nb;
            }
            mails[m_count].sender = strdup(sender);
            mails[m_count].subject = strdup(subject);
            mails[m_count].body = strdup(body);
            mails[m_count].tags = NULL;
            mails[m_count].tag_count = 0;
            mails[m_count].tag_cap = 0;
            m_count++;
        } else if (strcmp(cmd, "TAG") == 0) {
            char *subject = p;
            while (*p && *p != ' ') p++;
            if (*p) {
                *p = '\0';
                p++;
            }
            while (*p == ' ') p++;
            char *tag = p;
            while (*p && *p != ' ') p++;
            if (*p) *p = '\0';
            for (int i = 0; i < m_count; i++) {
                if (strcmp(mails[i].subject, subject) == 0) {
                    Message *m = &mails[i];
                    if (m->tag_count >= m->tag_cap) {
                        m->tag_cap = m->tag_cap == 0 ? 16 : m->tag_cap * 2;
                        char **nb = realloc(m->tags, m->tag_cap * sizeof(char *));
                        if (nb) m->tags = nb;
                    }
                    m->tags[m->tag_count] = strdup(tag);
                    m->tag_count++;
                    break;
                }
            }
        } else if (strcmp(cmd, "UNTAG") == 0) {
            char *subject = p;
            while (*p && *p != ' ') p++;
            if (*p) {
                *p = '\0';
                p++;
            }
            while (*p == ' ') p++;
            char *tag = p;
            while (*p && *p != ' ') p++;
            if (*p) *p = '\0';
            for (int i = 0; i < m_count; i++) {
                if (strcmp(mails[i].subject, subject) == 0) {
                    Message *m = &mails[i];
                    for (int j = 0; j < m->tag_count; j++) {
                        if (strcmp(m->tags[j], tag) == 0) {
                            free(m->tags[j]);
                            for (int k = j; k < m->tag_count - 1; k++) {
                                m->tags[k] = m->tags[k + 1];
                            }
                            m->tag_count--;
                            break;
                        }
                    }
                    break;
                }
            }
        } else if (strcmp(cmd, "DELETE") == 0) {
            char *subject = p;
            while (*p && *p != ' ') p++;
            if (*p) *p = '\0';
            for (int i = 0; i < m_count; i++) {
                if (strcmp(mails[i].subject, subject) == 0) {
                    free(mails[i].sender);
                    free(mails[i].subject);
                    free(mails[i].body);
                    for (int j = 0; j < mails[i].tag_count; j++) {
                        free(mails[i].tags[j]);
                    }
                    free(mails[i].tags);
                    for (int j = i; j < m_count - 1; j++) {
                        mails[j] = mails[j + 1];
                    }
                    m_count--;
                    break;
                }
            }
        } else if (strcmp(cmd, "SEARCH") == 0) {
            char *tag = p;
            while (*p && *p != ' ') p++;
            if (*p) *p = '\0';
            for (int i = 0; i < m_count; i++) {
                int has_tag = 0;
                for (int j = 0; j < mails[i].tag_count; j++) {
                    if (strcmp(mails[i].tags[j], tag) == 0) {
                        has_tag = 1;
                        break;
                    }
                }
                if (has_tag) {
                    printf("%s\n", mails[i].subject);
                }
            }
        }
        free(line);
    }
    for (int i = 0; i < m_count; i++) {
        free(mails[i].sender);
        free(mails[i].subject);
        free(mails[i].body);
        for (int j = 0; j < mails[i].tag_count; j++) {
            free(mails[i].tags[j]);
        }
        free(mails[i].tags);
    }
    free(mails);
    return 0;
}
