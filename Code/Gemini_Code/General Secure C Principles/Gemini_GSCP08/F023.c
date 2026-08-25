// F023.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *read_line(FILE *fp) {
    size_t cap = 128;
    size_t len = 0;
    char *buf = malloc(cap);
    if (!buf) return NULL;
    int c;
    while ((c = fgetc(fp)) != EOF && c != '\n') {
        if (len + 1 >= cap) {
            size_t ncap = cap * 2;
            char *nb = realloc(buf, ncap);
            if (!nb) {
                free(buf);
                return NULL;
            }
            buf = nb;
            cap = ncap;
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
    size_t tag_count;
    size_t tag_cap;
} Mail;

int main(void) {
    Mail *arr = NULL;
    size_t count = 0;
    size_t cap = 0;
    while (1) {
        char *line = read_line(stdin);
        if (!line) break;
        if (strcmp(line, "END") == 0) {
            free(line);
            break;
        }
        char *cmd = line;
        char *p1 = strchr(cmd, ' ');
        if (!p1) {
            if (strcmp(cmd, "REPORT") == 0) {
                for (size_t i = 0; i < count; i++) {
                    printf("%s %s %s\n", arr[i].sender, arr[i].subject, arr[i].body);
                }
            }
            free(line);
            continue;
        }
        *p1 = '\0';
        char *args = p1 + 1;
        if (strcmp(cmd, "MAIL") == 0) {
            char *sender = args;
            char *p2 = strchr(sender, ' ');
            if (p2) {
                *p2 = '\0';
                char *subj = p2 + 1;
                char *p3 = strchr(subj, ' ');
                if (p3) {
                    *p3 = '\0';
                    char *body = p3 + 1;
                    if (count >= cap) {
                        size_t ncap = cap == 0 ? 4 : cap * 2;
                        Mail *narr = realloc(arr, ncap * sizeof(Mail));
                        if (narr) {
                            arr = narr;
                            cap = ncap;
                        }
                    }
                    if (count < cap) {
                        arr[count].sender = strdup(sender);
                        arr[count].subject = strdup(subj);
                        arr[count].body = strdup(body);
                        arr[count].tags = NULL;
                        arr[count].tag_count = 0;
                        arr[count].tag_cap = 0;
                        count++;
                    }
                }
            }
        } else if (strcmp(cmd, "TAG") == 0) {
            char *subj = args;
            char *p2 = strchr(subj, ' ');
            if (p2) {
                *p2 = '\0';
                char *tag = p2 + 1;
                for (size_t i = 0; i < count; i++) {
                    if (strcmp(arr[i].subject, subj) == 0) {
                        Mail *m = &arr[i];
                        int t_found = 0;
                        for (size_t j = 0; j < m->tag_count; j++) {
                            if (strcmp(m->tags[j], tag) == 0) {
                                t_found = 1;
                                break;
                            }
                        }
                        if (!t_found) {
                            if (m->tag_count >= m->tag_cap) {
                                size_t ncap = m->tag_cap == 0 ? 4 : m->tag_cap * 2;
                                char **ntags = realloc(m->tags, ncap * sizeof(char *));
                                if (ntags) {
                                    m->tags = ntags;
                                    m->tag_cap = ncap;
                                }
                            }
                            if (m->tag_count < m->tag_cap) {
                                m->tags[m->tag_count] = strdup(tag);
                                m->tag_count++;
                            }
                        }
                        break;
                    }
                }
            }
        } else if (strcmp(cmd, "UNTAG") == 0) {
            char *subj = args;
            char *p2 = strchr(subj, ' ');
            if (p2) {
                *p2 = '\0';
                char *tag = p2 + 1;
                for (size_t i = 0; i < count; i++) {
                    if (strcmp(arr[i].subject, subj) == 0) {
                        Mail *m = &arr[i];
                        for (size_t j = 0; j < m->tag_count; j++) {
                            if (strcmp(m->tags[j], tag) == 0) {
                                free(m->tags[j]);
                                for (size_t k = j; k < m->tag_count - 1; k++) {
                                    m->tags[k] = m->tags[k + 1];
                                }
                                m->tag_count--;
                                break;
                            }
                        }
                        break;
                    }
                }
            }
        } else if (strcmp(cmd, "DELETE") == 0) {
            char *subj = args;
            for (size_t i = 0; i < count; i++) {
                if (strcmp(arr[i].subject, subj) == 0) {
                    free(arr[i].sender);
                    free(arr[i].subject);
                    free(arr[i].body);
                    for (size_t j = 0; j < arr[i].tag_count; j++) {
                        free(arr[i].tags[j]);
                    }
                    free(arr[i].tags);
                    for (size_t j = i; j < count - 1; j++) {
                        arr[j] = arr[j + 1];
                    }
                    count--;
                    break;
                }
            }
        } else if (strcmp(cmd, "SEARCH") == 0) {
            char *tag = args;
            for (size_t i = 0; i < count; i++) {
                for (size_t j = 0; j < arr[i].tag_count; j++) {
                    if (strcmp(arr[i].tags[j], tag) == 0) {
                        printf("%s\n", arr[i].subject);
                        break;
                    }
                }
            }
        }
        free(line);
    }
    for (size_t i = 0; i < count; i++) {
        free(arr[i].sender);
        free(arr[i].subject);
        free(arr[i].body);
        for (size_t j = 0; j < arr[i].tag_count; j++) {
            free(arr[i].tags[j]);
        }
        free(arr[i].tags);
    }
    free(arr);
    return 0;
}
