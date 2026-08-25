// F023.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *read_line(FILE *fp) {
    size_t size = 128;
    size_t len = 0;
    char *buf = malloc(size);
    if (!buf) return NULL;
    int c;
    while ((c = fgetc(fp)) != EOF && c != '\n') {
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
    int tag_count;
} Message;

int main() {
    Message *mailbox = NULL;
    int count = 0;
    while (1) {
        char *line = read_line(stdin);
        if (!line) break;
        if (strcmp(line, "END") == 0) {
            free(line);
            break;
        }
        if (strncmp(line, "MAIL ", 5) == 0) {
            char snd[128], subj[128];
            if (sscanf(line + 5, "%127s %127s", snd, subj) == 2) {
                char *p = line + 5;
                while (*p && *p != ' ') p++;
                if (*p == ' ') p++;
                while (*p && *p != ' ') p++;
                if (*p == ' ') p++;
                mailbox = realloc(mailbox, (count + 1) * sizeof(Message));
                mailbox[count].sender = strdup(snd);
                mailbox[count].subject = strdup(subj);
                mailbox[count].body = strdup(p);
                mailbox[count].tags = NULL;
                mailbox[count].tag_count = 0;
                count++;
            }
        } else if (strncmp(line, "TAG ", 4) == 0) {
            char subj[128], tag[128];
            if (sscanf(line + 4, "%127s %127s", subj, tag) == 2) {
                for (int i = 0; i < count; i++) {
                    if (strcmp(mailbox[i].subject, subj) == 0) {
                        mailbox[i].tags = realloc(mailbox[i].tags, (mailbox[i].tag_count + 1) * sizeof(char *));
                        mailbox[i].tags[mailbox[i].tag_count] = strdup(tag);
                        mailbox[i].tag_count++;
                        break;
                    }
                }
            }
        } else if (strncmp(line, "UNTAG ", 6) == 0) {
            char subj[128], tag[128];
            if (sscanf(line + 6, "%127s %127s", subj, tag) == 2) {
                for (int i = 0; i < count; i++) {
                    if (strcmp(mailbox[i].subject, subj) == 0) {
                        for (int j = 0; j < mailbox[i].tag_count; j++) {
                            if (strcmp(mailbox[i].tags[j], tag) == 0) {
                                free(mailbox[i].tags[j]);
                                for (int m = j; m < mailbox[i].tag_count - 1; m++) {
                                    mailbox[i].tags[m] = mailbox[i].tags[m + 1];
                                }
                                mailbox[i].tag_count--;
                                mailbox[i].tags = realloc(mailbox[i].tags, mailbox[i].tag_count * sizeof(char *));
                                break;
                            }
                        }
                        break;
                    }
                }
            }
        } else if (strncmp(line, "DELETE ", 7) == 0) {
            char *subj = line + 7;
            for (int i = 0; i < count; i++) {
                if (strcmp(mailbox[i].subject, subj) == 0) {
                    free(mailbox[i].sender);
                    free(mailbox[i].subject);
                    free(mailbox[i].body);
                    for (int j = 0; j < mailbox[i].tag_count; j++) {
                        free(mailbox[i].tags[j]);
                    }
                    free(mailbox[i].tags);
                    for (int j = i; j < count - 1; j++) {
                        mailbox[j] = mailbox[j + 1];
                    }
                    count--;
                    mailbox = realloc(mailbox, count * sizeof(Message));
                    break;
                }
            }
        } else if (strncmp(line, "SEARCH ", 7) == 0) {
            char *tag = line + 7;
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
                printf("From: %s Subject: %s Body: %s Tags:", mailbox[i].sender, mailbox[i].subject, mailbox[i].body);
                for (int j = 0; j < mailbox[i].tag_count; j++) {
                    printf(" %s", mailbox[i].tags[j]);
                }
                printf("\n");
            }
        }
        free(line);
    }
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
