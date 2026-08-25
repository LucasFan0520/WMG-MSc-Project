// F023.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *read_line(FILE *f) {
    size_t size = 16;
    size_t len = 0;
    char *str = malloc(size);
    if (!str) return NULL;
    int c;
    while ((c = fgetc(f)) != EOF && c != '\n') {
        if (len + 1 >= size) {
            size *= 2;
            char *new_str = realloc(str, size);
            if (!new_str) { free(str); return NULL; }
            str = new_str;
        }
        str[len++] = c;
    }
    if (len == 0 && c == EOF) { free(str); return NULL; }
    str[len] = '\0';
    return str;
}

typedef struct {
    char *sender;
    char *subject;
    char *body;
    char **tags;
    int tag_count;
} Mail;

int main() {
    Mail *mails = NULL;
    int count = 0;
    while (1) {
        char *line = read_line(stdin);
        if (!line) break;
        if (strcmp(line, "END") == 0) { free(line); break; }
        char *cmd = line;
        char *p1 = strchr(cmd, ' ');
        if (!p1) {
            if (strcmp(cmd, "REPORT") == 0) {
                for (int i = 0; i < count; i++) {
                    printf("From: %s, Subject: %s, Body: %s\n", mails[i].sender, mails[i].subject, mails[i].body);
                    if (mails[i].tag_count > 0) {
                        printf("  Tags: ");
                        for (int j = 0; j < mails[i].tag_count; j++) {
                            printf("%s%s", mails[i].tags[j], (j == mails[i].tag_count - 1) ? "" : ", ");
                        }
                        printf("\n");
                    }
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
                char *subject = p2 + 1;
                char *p3 = strchr(subject, ' ');
                if (p3) {
                    *p3 = '\0';
                    char *body = p3 + 1;
                    mails = realloc(mails, sizeof(Mail) * (count + 1));
                    mails[count].sender = strdup(sender);
                    mails[count].subject = strdup(subject);
                    mails[count].body = strdup(body);
                    mails[count].tags = NULL;
                    mails[count].tag_count = 0;
                    count++;
                }
            }
        } else if (strcmp(cmd, "TAG") == 0) {
            char *subject = args;
            char *p2 = strchr(subject, ' ');
            if (p2) {
                *p2 = '\0';
                char *tag = p2 + 1;
                for (int i = 0; i < count; i++) {
                    if (strcmp(mails[i].subject, subject) == 0) {
                        int tc = mails[i].tag_count;
                        mails[i].tags = realloc(mails[i].tags, sizeof(char*) * (tc + 1));
                        mails[i].tags[tc] = strdup(tag);
                        mails[i].tag_count++;
                        break;
                    }
                }
            }
        } else if (strcmp(cmd, "UNTAG") == 0) {
            char *subject = args;
            char *p2 = strchr(subject, ' ');
            if (p2) {
                *p2 = '\0';
                char *tag = p2 + 1;
                for (int i = 0; i < count; i++) {
                    if (strcmp(mails[i].subject, subject) == 0) {
                        for (int j = 0; j < mails[i].tag_count; j++) {
                            if (strcmp(mails[i].tags[j], tag) == 0) {
                                free(mails[i].tags[j]);
                                for (int k = j; k < mails[i].tag_count - 1; k++) {
                                    mails[i].tags[k] = mails[i].tags[k + 1];
                                }
                                mails[i].tag_count--;
                                if (mails[i].tag_count == 0) {
                                    free(mails[i].tags);
                                    mails[i].tags = NULL;
                                } else {
                                    mails[i].tags = realloc(mails[i].tags, sizeof(char*) * mails[i].tag_count);
                                }
                                break;
                            }
                        }
                        break;
                    }
                }
            }
        } else if (strcmp(cmd, "DELETE") == 0) {
            for (int i = 0; i < count; i++) {
                if (strcmp(mails[i].subject, args) == 0) {
                    free(mails[i].sender);
                    free(mails[i].subject);
                    free(mails[i].body);
                    for (int j = 0; j < mails[i].tag_count; j++) free(mails[i].tags[j]);
                    free(mails[i].tags);
                    for (int j = i; j < count - 1; j++) {
                        mails[j] = mails[j + 1];
                    }
                    count--;
                    if (count == 0) { free(mails); mails = NULL; }
                    else { mails = realloc(mails, sizeof(Mail) * count); }
                    break;
                }
            }
        } else if (strcmp(cmd, "SEARCH") == 0) {
            for (int i = 0; i < count; i++) {
                for (int j = 0; j < mails[i].tag_count; j++) {
                    if (strcmp(mails[i].tags[j], args) == 0) {
                        printf("%s\n", mails[i].subject);
                        break;
                    }
                }
            }
        }
        free(line);
    }
    for (int i = 0; i < count; i++) {
        free(mails[i].sender);
        free(mails[i].subject);
        free(mails[i].body);
        for (int j = 0; j < mails[i].tag_count; j++) free(mails[i].tags[j]);
        free(mails[i].tags);
    }
    free(mails);
    return 0;
}
