// F023.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *read_line(FILE *fp) {
    size_t size = 1024;
    size_t len = 0;
    char *buf = malloc(size);
    if (!buf) return NULL;
    int c;
    while ((c = fgetc(fp)) != EOF && c != '\n') {
        if (len + 1 >= size) {
            size *= 2;
            char *new_buf = realloc(buf, size);
            if (!new_buf) { free(buf); return NULL; }
            buf = new_buf;
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
    int tag_count;
} Message;

int main() {
    Message *mailbox = NULL;
    int count = 0;
    int capacity = 0;
    while (1) {
        char *line = read_line(stdin);
        if (!line) break;
        char cmd[64];
        int n = 0;
        if (sscanf(line, "%63s%n", cmd, &n) < 1) {
            free(line);
            continue;
        }
        if (strcmp(cmd, "END") == 0) {
            free(line);
            break;
        }
        if (strcmp(cmd, "REPORT") == 0) {
            for (int i = 0; i < count; i++) {
                printf("%s | %s | %s", mailbox[i].sender, mailbox[i].subject, mailbox[i].body);
                if (mailbox[i].tag_count > 0) {
                    printf(" [");
                    for (int j = 0; j < mailbox[i].tag_count; j++) {
                        printf("%s%s", mailbox[i].tags[j], (j == mailbox[i].tag_count - 1) ? "" : ",");
                    }
                    printf("]");
                }
                printf("\n");
            }
            free(line);
            continue;
        }
        char *p = line + n;
        while (*p == ' ' || *p == '\t') p++;
        if (strcmp(cmd, "MAIL") == 0) {
            char sender[256], subject[256];
            int n2 = 0;
            if (sscanf(p, "%255s %255s%n", sender, subject, &n2) == 2) {
                p += n2;
                while (*p == ' ' || *p == '\t') p++;
                if (count >= capacity) {
                    capacity = capacity == 0 ? 4 : capacity * 2;
                    mailbox = realloc(mailbox, capacity * sizeof(Message));
                }
                mailbox[count].sender = strdup(sender);
                mailbox[count].subject = strdup(subject);
                mailbox[count].body = strdup(p);
                mailbox[count].tags = NULL;
                mailbox[count].tag_count = 0;
                count++;
            }
        } else if (strcmp(cmd, "TAG") == 0) {
            char subj[256], tag[256];
            if (sscanf(p, "%255s %255s", subj, tag) == 2) {
                for (int i = 0; i < count; i++) {
                    if (strcmp(mailbox[i].subject, subj) == 0) {
                        mailbox[i].tags = realloc(mailbox[i].tags, (mailbox[i].tag_count + 1) * sizeof(char *));
                        mailbox[i].tags[mailbox[i].tag_count++] = strdup(tag);
                        break;
                    }
                }
            }
        } else if (strcmp(cmd, "UNTAG") == 0) {
            char subj[256], tag[256];
            if (sscanf(p, "%255s %255s", subj, tag) == 2) {
                for (int i = 0; i < count; i++) {
                    if (strcmp(mailbox[i].subject, subj) == 0) {
                        int found_tag = -1;
                        for (int j = 0; j < mailbox[i].tag_count; j++) {
                            if (strcmp(mailbox[i].tags[j], tag) == 0) {
                                found_tag = j;
                                break;
                            }
                        }
                        if (found_tag != -1) {
                            free(mailbox[i].tags[found_tag]);
                            for (int j = found_tag; j < mailbox[i].tag_count - 1; j++) {
                                mailbox[i].tags[j] = mailbox[i].tags[j + 1];
                            }
                            mailbox[i].tag_count--;
                        }
                        break;
                    }
                }
            }
        } else if (strcmp(cmd, "DELETE") == 0) {
            char subj[256];
            if (sscanf(p, "%255s", subj) == 1) {
                int found = -1;
                for (int i = 0; i < count; i++) {
                    if (strcmp(mailbox[i].subject, subj) == 0) {
                        found = i;
                        break;
                    }
                }
                if (found != -1) {
                    free(mailbox[found].sender);
                    free(mailbox[found].subject);
                    free(mailbox[found].body);
                    for (int j = 0; j < mailbox[found].tag_count; j++) {
                        free(mailbox[found].tags[j]);
                    }
                    free(mailbox[found].tags);
                    for (int i = found; i < count - 1; i++) {
                        mailbox[i] = mailbox[i + 1];
                    }
                    count--;
                }
            }
        } else if (strcmp(cmd, "SEARCH") == 0) {
            char tag[256];
            if (sscanf(p, "%255s", tag) == 1) {
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
