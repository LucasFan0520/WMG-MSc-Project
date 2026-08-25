// F023.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *sender;
    char *subject;
    char *body;
    char **tags;
    int t_count;
} Message;

char *read_line(FILE *f) {
    size_t size = 32;
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
    if (c == EOF && len == 0) {
        free(buf);
        return NULL;
    }
    buf[len] = '\0';
    return buf;
}

int main() {
    Message *list = NULL;
    int count = 0;
    while (1) {
        char *line = read_line(stdin);
        if (!line) break;
        if (strcmp(line, "END") == 0) {
            free(line);
            break;
        }
        char *p1 = strchr(line, ' ');
        if (!p1) {
            if (strcmp(line, "REPORT") == 0) {
                for (int i = 0; i < count; i++) {
                    printf("From: %s Subject: %s Body: %s Tags:", list[i].sender, list[i].subject, list[i].body);
                    for (int j = 0; j < list[i].t_count; j++) {
                        printf(" %s", list[i].tags[j]);
                    }
                    printf("\n");
                }
            }
            free(line);
            continue;
        }
        *p1 = '\0';
        char *cmd = line;
        char *rest = p1 + 1;
        if (strcmp(cmd, "MAIL") == 0) {
            char *p2 = strchr(rest, ' ');
            if (p2) {
                *p2 = '\0';
                char *sender = rest;
                char *rest2 = p2 + 1;
                char *p3 = strchr(rest2, ' ');
                if (p3) {
                    *p3 = '\0';
                    char *subject = rest2;
                    char *body = p3 + 1;
                    list = realloc(list, (count + 1) * sizeof(Message));
                    list[count].sender = strdup(sender);
                    list[count].subject = strdup(subject);
                    list[count].body = strdup(body);
                    list[count].tags = NULL;
                    list[count].t_count = 0;
                    count++;
                }
            }
        } else if (strcmp(cmd, "TAG") == 0 || strcmp(cmd, "UNTAG") == 0) {
            char *p2 = strchr(rest, ' ');
            if (p2) {
                *p2 = '\0';
                char *subject = rest;
                char *tag = p2 + 1;
                int found = -1;
                for (int i = 0; i < count; i++) {
                    if (strcmp(list[i].subject, subject) == 0) {
                        found = i;
                        break;
                    }
                }
                if (found != -1) {
                    if (strcmp(cmd, "TAG") == 0) {
                        int tc = list[found].t_count;
                        list[found].tags = realloc(list[found].tags, (tc + 1) * sizeof(char *));
                        list[found].tags[tc] = strdup(tag);
                        list[found].t_count++;
                    } else {
                        int t_idx = -1;
                        for (int i = 0; i < list[found].t_count; i++) {
                            if (strcmp(list[found].tags[i], tag) == 0) {
                                t_idx = i;
                                break;
                            }
                        }
                        if (t_idx != -1) {
                            free(list[found].tags[t_idx]);
                            for (int i = t_idx; i < list[found].t_count - 1; i++) {
                                list[found].tags[i] = list[found].tags[i + 1];
                            }
                            list[found].t_count--;
                            if (list[found].t_count == 0) {
                                free(list[found].tags);
                                list[found].tags = NULL;
                            } else {
                                list[found].tags = realloc(list[found].tags, list[found].t_count * sizeof(char *));
                            }
                        }
                    }
                }
            }
        } else if (strcmp(cmd, "DELETE") == 0) {
            char *subject = rest;
            int found = -1;
            for (int i = 0; i < count; i++) {
                if (strcmp(list[i].subject, subject) == 0) {
                    found = i;
                    break;
                }
            }
            if (found != -1) {
                free(list[found].sender);
                free(list[found].subject);
                free(list[found].body);
                for (int j = 0; j < list[found].t_count; j++) {
                    free(list[found].tags[j]);
                }
                free(list[found].tags);
                for (int i = found; i < count - 1; i++) {
                    list[i] = list[i + 1];
                }
                count--;
                if (count == 0) {
                    free(list);
                    list = NULL;
                } else {
                    list = realloc(list, count * sizeof(Message));
                }
            }
        } else if (strcmp(cmd, "SEARCH") == 0) {
            char *tag = rest;
            for (int i = 0; i < count; i++) {
                for (int j = 0; j < list[i].t_count; j++) {
                    if (strcmp(list[i].tags[j], tag) == 0) {
                        printf("%s\n", list[i].subject);
                        break;
                    }
                }
            }
        }
        free(line);
    }
    for (int i = 0; i < count; i++) {
        free(list[i].sender);
        free(list[i].subject);
        free(list[i].body);
        for (int j = 0; j < list[i].t_count; j++) {
            free(list[i].tags[j]);
        }
        free(list[i].tags);
    }
    free(list);
    return 0;
}