// F023.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *read_line(FILE *f) {
    size_t size = 16;
    size_t len = 0;
    char *buf = malloc(size);
    if (!buf) return NULL;
    int c;
    while ((c = fgetc(f)) != EOF && c != '\n') {
        if (len + 1 >= size) {
            size *= 2;
            char *new_buf = realloc(buf, size);
            if (!new_buf) {
                free(buf);
                return NULL;
            }
            buf = new_buf;
        }
        buf[len++] = c;
    }
    if (len == 0 && c == EOF) {
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
    size_t tcount;
    size_t tcap;
} Message;

int main() {
    Message *messages = NULL;
    size_t mcount = 0;
    size_t mcap = 0;
    while (1) {
        char *line = read_line(stdin);
        if (!line) break;
        if (strcmp(line, "END") == 0) {
            free(line);
            break;
        }
        if (strncmp(line, "MAIL ", 5) == 0) {
            char *p = line + 5;
            char *s1 = strchr(p, ' ');
            if (s1) {
                *s1 = '\0';
                char *sender = p;
                char *p2 = s1 + 1;
                char *s2 = strchr(p2, ' ');
                char *subject = p2;
                char *body = "";
                if (s2) {
                    *s2 = '\0';
                    body = s2 + 1;
                }
                if (mcount >= mcap) {
                    mcap = mcap == 0 ? 4 : mcap * 2;
                    messages = realloc(messages, mcap * sizeof(Message));
                }
                messages[mcount].sender = strdup(sender);
                messages[mcount].subject = strdup(subject);
                messages[mcount].body = strdup(body);
                messages[mcount].tags = NULL;
                messages[mcount].tcount = 0;
                messages[mcount].tcap = 0;
                mcount++;
            }
        } else if (strncmp(line, "TAG ", 4) == 0) {
            char *p = line + 4;
            char *s1 = strchr(p, ' ');
            if (s1) {
                *s1 = '\0';
                char *subject = p;
                char *tag = s1 + 1;
                for (size_t i = 0; i < mcount; i++) {
                    if (strcmp(messages[i].subject, subject) == 0) {
                        Message *m = &messages[i];
                        if (m->tcount >= m->tcap) {
                            m->tcap = m->tcap == 0 ? 4 : m->tcap * 2;
                            m->tags = realloc(m->tags, m->tcap * sizeof(char *));
                        }
                        m->tags[m->tcount++] = strdup(tag);
                        break;
                    }
                }
            }
        } else if (strncmp(line, "UNTAG ", 6) == 0) {
            char *p = line + 6;
            char *s1 = strchr(p, ' ');
            if (s1) {
                *s1 = '\0';
                char *subject = p;
                char *tag = s1 + 1;
                for (size_t i = 0; i < mcount; i++) {
                    if (strcmp(messages[i].subject, subject) == 0) {
                        Message *m = &messages[i];
                        for (size_t j = 0; j < m->tcount; j++) {
                            if (strcmp(m->tags[j], tag) == 0) {
                                free(m->tags[j]);
                                for (size_t k = j; k < m->tcount - 1; k++) {
                                    m->tags[k] = m->tags[k + 1];
                                }
                                m->tcount--;
                                break;
                            }
                        }
                        break;
                    }
                }
            }
        } else if (strncmp(line, "DELETE ", 7) == 0) {
            char *subject = line + 7;
            for (size_t i = 0; i < mcount; i++) {
                if (strcmp(messages[i].subject, subject) == 0) {
                    free(messages[i].sender);
                    free(messages[i].subject);
                    free(messages[i].body);
                    for (size_t j = 0; j < messages[i].tcount; j++) {
                        free(messages[i].tags[j]);
                    }
                    free(messages[i].tags);
                    for (size_t j = i; j < mcount - 1; j++) {
                        messages[j] = messages[j + 1];
                    }
                    mcount--;
                    break;
                }
            }
        } else if (strncmp(line, "SEARCH ", 7) == 0) {
            char *tag = line + 7;
            for (size_t i = 0; i < mcount; i++) {
                int has_tag = 0;
                for (size_t j = 0; j < messages[i].tcount; j++) {
                    if (strcmp(messages[i].tags[j], tag) == 0) {
                        has_tag = 1;
                        break;
                    }
                }
                if (has_tag) {
                    printf("%s\n", messages[i].subject);
                }
            }
        } else if (strcmp(line, "REPORT") == 0) {
            for (size_t i = 0; i < mcount; i++) {
                printf("%s %s %s\n", messages[i].sender, messages[i].subject, messages[i].body);
            }
        }
        free(line);
    }
    for (size_t i = 0; i < mcount; i++) {
        free(messages[i].sender);
        free(messages[i].subject);
        free(messages[i].body);
        for (size_t j = 0; j < messages[i].tcount; j++) {
            free(messages[i].tags[j]);
        }
        free(messages[i].tags);
    }
    free(messages);
    return 0;
}
