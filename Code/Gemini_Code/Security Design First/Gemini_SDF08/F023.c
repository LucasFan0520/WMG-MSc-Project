// F023.c
#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *sender;
    char *subject;
    char *body;
    char **tags;
    int tag_count;
} Message;

int main(void) {
    Message *messages = NULL;
    int count = 0;
    char *line = NULL;
    size_t len = 0;
    ssize_t read;

    while ((read = getline(&line, &len, stdin)) != -1) {
        if (read > 0 && line[read - 1] == '\n') {
            line[read - 1] = '\0';
        }
        if (strcmp(line, "END") == 0) {
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
                if (s2) {
                    *s2 = '\0';
                    char *subject = p2;
                    char *body = s2 + 1;
                    Message *tmp = realloc(messages, sizeof(Message) * (count + 1));
                    if (tmp) {
                        messages = tmp;
                        messages[count].sender = strdup(sender);
                        messages[count].subject = strdup(subject);
                        messages[count].body = strdup(body);
                        messages[count].tags = NULL;
                        messages[count].tag_count = 0;
                        if (messages[count].sender && messages[count].subject && messages[count].body) {
                            count++;
                        }
                    }
                }
            }
        } else if (strncmp(line, "TAG ", 4) == 0) {
            char *p = line + 4;
            char *s1 = strchr(p, ' ');
            if (s1) {
                *s1 = '\0';
                char *subject = p;
                char *tag = s1 + 1;
                for (int i = 0; i < count; i++) {
                    if (strcmp(messages[i].subject, subject) == 0) {
                        char **ttmp = realloc(messages[i].tags, sizeof(char *) * (messages[i].tag_count + 1));
                        if (ttmp) {
                            messages[i].tags = ttmp;
                            messages[i].tags[messages[i].tag_count] = strdup(tag);
                            if (messages[i].tags[messages[i].tag_count]) {
                                messages[i].tag_count++;
                            }
                        }
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
                for (int i = 0; i < count; i++) {
                    if (strcmp(messages[i].subject, subject) == 0) {
                        int t_idx = -1;
                        for (int j = 0; j < messages[i].tag_count; j++) {
                            if (strcmp(messages[i].tags[j], tag) == 0) {
                                t_idx = j;
                                break;
                            }
                        }
                        if (t_idx != -1) {
                            free(messages[i].tags[t_idx]);
                            for (int j = t_idx; j < messages[i].tag_count - 1; j++) {
                                messages[i].tags[j] = messages[i].tags[j + 1];
                            }
                            messages[i].tag_count--;
                        }
                        break;
                    }
                }
            }
        } else if (strncmp(line, "DELETE ", 7) == 0) {
            char *subject = line + 7;
            int idx = -1;
            for (int i = 0; i < count; i++) {
                if (strcmp(messages[i].subject, subject) == 0) {
                    idx = i;
                    break;
                }
            }
            if (idx != -1) {
                free(messages[idx].sender);
                free(messages[idx].subject);
                free(messages[idx].body);
                for (int j = 0; j < messages[idx].tag_count; j++) {
                    free(messages[idx].tags[j]);
                }
                free(messages[idx].tags);
                for (int i = idx; i < count - 1; i++) {
                    messages[i] = messages[i + 1];
                }
                count--;
            }
        } else if (strncmp(line, "SEARCH ", 7) == 0) {
            char *tag = line + 7;
            for (int i = 0; i < count; i++) {
                int has_tag = 0;
                for (int j = 0; j < messages[i].tag_count; j++) {
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
            for (int i = 0; i < count; i++) {
                printf("From: %s Subj: %s Body: %s\n", messages[i].sender, messages[i].subject, messages[i].body);
            }
        }
    }
    for (int i = 0; i < count; i++) {
        free(messages[i].sender);
        free(messages[i].subject);
        free(messages[i].body);
        for (int j = 0; j < messages[i].tag_count; j++) {
            free(messages[i].tags[j]);
        }
        free(messages[i].tags);
    }
    free(messages);
    free(line);
    return 0;
}
