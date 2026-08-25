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
    if (len > 0 && buf[len - 1] == '\n') {
        buf[len - 1] = '\0';
    } else {
        buf[len] = '\0';
    }
    return buf;
}

char *safe_dup(const char *s) {
    size_t l = strlen(s);
    char *d = malloc(l + 1);
    if (d) {
        strcpy(d, s);
    }
    return d;
}

typedef struct {
    char *sender;
    char *subject;
    char *body;
    char **tags;
    size_t t_count;
    size_t t_cap;
} MailMessage;

int main(void) {
    MailMessage *box = NULL;
    size_t m_count = 0;
    size_t m_cap = 0;
    while (1) {
        char *line = read_line(stdin);
        if (!line) break;
        if (strcmp(line, "END") == 0) {
            free(line);
            break;
        }
        if (strncmp(line, "MAIL ", 5) == 0) {
            char *p = line + 5;
            char *sender = p;
            while (*p && *p != ' ') p++;
            if (*p == ' ') {
                *p = '\0';
                p++;
            }
            char *subject = p;
            while (*p && *p != ' ') p++;
            char *body = "";
            if (*p == ' ') {
                *p = '\0';
                body = p + 1;
            }
            if (m_count >= m_cap) {
                m_cap = m_cap == 0 ? 4 : m_cap * 2;
                MailMessage *nb = realloc(box, m_cap * sizeof(MailMessage));
                if (!nb) {
                    free(line);
                    break;
                }
                box = nb;
            }
            box[m_count].sender = safe_dup(sender);
            box[m_count].subject = safe_dup(subject);
            box[m_count].body = safe_dup(body);
            box[m_count].tags = NULL;
            box[m_count].t_count = 0;
            box[m_count].t_cap = 0;
            m_count++;
        } else if (strncmp(line, "TAG ", 4) == 0) {
            char *p = line + 4;
            char *subject = p;
            while (*p && *p != ' ') p++;
            char *tag = "";
            if (*p == ' ') {
                *p = '\0';
                tag = p + 1;
            }
            for (size_t i = 0; i < m_count; i++) {
                if (strcmp(box[i].subject, subject) == 0) {
                    MailMessage *m = &box[i];
                    if (m->t_count >= m->t_cap) {
                        m->t_cap = m->t_cap == 0 ? 4 : m->t_cap * 2;
                        char **nb = realloc(m->tags, m->t_cap * sizeof(char *));
                        if (!nb) break;
                        m->tags = nb;
                    }
                    m->tags[m->t_count] = safe_dup(tag);
                    m->t_count++;
                    break;
                }
            }
        } else if (strncmp(line, "UNTAG ", 6) == 0) {
            char *p = line + 6;
            char *subject = p;
            while (*p && *p != ' ') p++;
            char *tag = "";
            if (*p == ' ') {
                *p = '\0';
                tag = p + 1;
            }
            for (size_t i = 0; i < m_count; i++) {
                if (strcmp(box[i].subject, subject) == 0) {
                    MailMessage *m = &box[i];
                    for (size_t j = 0; j < m->t_count; j++) {
                        if (strcmp(m->tags[j], tag) == 0) {
                            free(m->tags[j]);
                            for (size_t k = j; k < m->t_count - 1; k++) {
                                m->tags[k] = m->tags[k + 1];
                            }
                            m->t_count--;
                            break;
                        }
                    }
                    break;
                }
            }
        } else if (strncmp(line, "DELETE ", 7) == 0) {
            char *subject = line + 7;
            for (size_t i = 0; i < m_count; i++) {
                if (strcmp(box[i].subject, subject) == 0) {
                    free(box[i].sender);
                    free(box[i].subject);
                    free(box[i].body);
                    for (size_t j = 0; j < box[i].t_count; j++) {
                        free(box[i].tags[j]);
                    }
                    free(box[i].tags);
                    for (size_t j = i; j < m_count - 1; j++) {
                        box[j] = box[j + 1];
                    }
                    m_count--;
                    break;
                }
            }
        } else if (strncmp(line, "SEARCH ", 7) == 0) {
            char *tag = line + 7;
            for (size_t i = 0; i < m_count; i++) {
                int found = 0;
                for (size_t j = 0; j < box[i].t_count; j++) {
                    if (strcmp(box[i].tags[j], tag) == 0) {
                        found = 1;
                        break;
                    }
                }
                if (found) {
                    printf("%s\n", box[i].subject);
                }
            }
        } else if (strcmp(line, "REPORT") == 0) {
            for (size_t i = 0; i < m_count; i++) {
                printf("From: %s Subject: %s Body: %s\n", box[i].sender, box[i].subject, box[i].body);
            }
        }
        free(line);
    }
    for (size_t i = 0; i < m_count; i++) {
        free(box[i].sender);
        free(box[i].subject);
        free(box[i].body);
        for (size_t j = 0; j < box[i].t_count; j++) {
            free(box[i].tags[j]);
        }
        free(box[i].tags);
    }
    free(box);
    return 0;
}
