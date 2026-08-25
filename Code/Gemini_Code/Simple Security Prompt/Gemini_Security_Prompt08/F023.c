// F023.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *read_line(FILE *f) {
    size_t cap = 16;
    size_t len = 0;
    char *buf = malloc(cap);
    if (!buf) return NULL;
    int c;
    while ((c = fgetc(f)) != EOF && c != '\n') {
        if (len + 1 >= cap) {
            cap *= 2;
            char *nb = realloc(buf, cap);
            if (!nb) { free(buf); return NULL; }
            buf = nb;
        }
        buf[len++] = c;
    }
    if (len == 0 && c == EOF) { free(buf); return NULL; }
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
} Message;

int main(void) {
    Message *list = NULL;
    size_t count = 0;
    size_t cap = 0;
    while (1) {
        char *line = read_line(stdin);
        if (!line) break;
        char *p = line;
        while (*p == ' ') p++;
        if (*p == '\0') { free(line); continue; }
        char *cmd = p;
        while (*p && *p != ' ') p++;
        if (*p) { *p = '\0'; p++; }
        if (strcmp(cmd, "END") == 0) { free(line); break; }
        while (*p == ' ') p++;
        char *arg1 = p;
        if (strcmp(cmd, "MAIL") == 0) {
            char *sender = arg1;
            p = arg1;
            while (*p && *p != ' ') p++;
            if (*p) { *p = '\0'; p++; }
            while (*p == ' ') p++;
            char *subject = p;
            while (*p && *p != ' ') p++;
            if (*p) { *p = '\0'; p++; }
            while (*p == ' ') p++;
            char *body = p;
            if (count >= cap) {
                cap = cap == 0 ? 4 : cap * 2;
                Message *nb = realloc(list, cap * sizeof(Message));
                if (nb) list = nb;
            }
            if (count < cap) {
                list[count].sender = strdup(sender);
                list[count].subject = strdup(subject);
                list[count].body = strdup(body);
                list[count].tags = NULL;
                list[count].tag_count = 0;
                list[count].tag_cap = 0;
                count++;
            }
        } else if (strcmp(cmd, "TAG") == 0 || strcmp(cmd, "UNTAG") == 0) {
            char *subject = arg1;
            p = arg1;
            while (*p && *p != ' ') p++;
            if (*p) { *p = '\0'; p++; }
            while (*p == ' ') p++;
            char *tag = p;
            while (*p && *p != ' ') p++;
            if (*p) *p = '\0';
            for (size_t i = 0; i < count; i++) {
                if (strcmp(list[i].subject, subject) == 0) {
                    if (strcmp(cmd, "TAG") == 0) {
                        if (list[i].tag_count >= list[i].tag_cap) {
                            list[i].tag_cap = list[i].tag_cap == 0 ? 4 : list[i].tag_cap * 2;
                            char **nt = realloc(list[i].tags, list[i].tag_cap * sizeof(char *));
                            if (nt) list[i].tags = nt;
                        }
                        if (list[i].tag_count < list[i].tag_cap) {
                            list[i].tags[list[i].tag_count++] = strdup(tag);
                        }
                    } else {
                        for (size_t j = 0; j < list[i].tag_count; j++) {
                            if (strcmp(list[i].tags[j], tag) == 0) {
                                free(list[i].tags[j]);
                                for (size_t k = j; k < list[i].tag_count - 1; k++) {
                                    list[i].tags[k] = list[i].tags[k + 1];
                                }
                                list[i].tag_count--;
                                break;
                            }
                        }
                    }
                    break;
                }
            }
        } else if (strcmp(cmd, "DELETE") == 0) {
            while (*p && *p != ' ') p++;
            if (*p) *p = '\0';
            for (size_t i = 0; i < count; i++) {
                if (strcmp(list[i].subject, arg1) == 0) {
                    free(list[i].sender);
                    free(list[i].subject);
                    free(list[i].body);
                    for (size_t j = 0; j < list[i].tag_count; j++) {
                        free(list[i].tags[j]);
                    }
                    free(list[i].tags);
                    for (size_t j = i; j < count - 1; j++) {
                        list[j] = list[j + 1];
                    }
                    count--;
                    break;
                }
            }
        } else if (strcmp(cmd, "SEARCH") == 0) {
            while (*p && *p != ' ') p++;
            if (*p) *p = '\0';
            for (size_t i = 0; i < count; i++) {
                int found = 0;
                for (size_t j = 0; j < list[i].tag_count; j++) {
                    if (strcmp(list[i].tags[j], arg1) == 0) {
                        found = 1;
                        break;
                    }
                }
                if (found) {
                    printf("%s\n", list[i].subject);
                }
            }
        } else if (strcmp(cmd, "REPORT") == 0) {
            for (size_t i = 0; i < count; i++) {
                printf("From: %s Subject: %s Body: %s\n", list[i].sender, list[i].subject, list[i].body);
            }
        }
        free(line);
    }
    for (size_t i = 0; i < count; i++) {
        free(list[i].sender);
        free(list[i].subject);
        free(list[i].body);
        for (size_t j = 0; j < list[i].tag_count; j++) {
            free(list[i].tags[j]);
        }
        free(list[i].tags);
    }
    free(list);
    return 0;
}
