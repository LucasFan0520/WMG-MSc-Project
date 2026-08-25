// F023.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *sender;
    char *subject;
    char *body;
    char **tags;
    int tcount;
} Message;

int main(void) {
    Message *messages = NULL;
    int mcount = 0;
    char *line = NULL;
    size_t cap = 0;
    while (getline(&line, &cap, stdin) != -1) {
        line[strcspn(line, "\r\n")] = 0;
        if (strcmp(line, "END") == 0) {
            break;
        }
        if (strncmp(line, "MAIL ", 5) == 0) {
            char snd[256], subj[256];
            char *p = line + 5;
            while (*p == ' ') p++;
            if (sscanf(p, "%255s %255s", snd, subj) == 2) {
                char *body = strchr(p, ' ');
                if (body) {
                    body = strchr(body + 1, ' ');
                    if (body) {
                        while (*body == ' ') body++;
                    }
                }
                if (!body) body = "";
                Message *tmp = realloc(messages, (mcount + 1) * sizeof(Message));
                if (tmp) {
                    messages = tmp;
                    messages[mcount].sender = strdup(snd);
                    messages[mcount].subject = strdup(subj);
                    messages[mcount].body = strdup(body);
                    messages[mcount].tags = NULL;
                    messages[mcount].tcount = 0;
                    mcount++;
                }
            }
        } else if (strncmp(line, "TAG ", 4) == 0) {
            char subj[256], tag[256];
            if (sscanf(line + 4, "%255s %255s", subj, tag) == 2) {
                for (int i = 0; i < mcount; i++) {
                    if (strcmp(messages[i].subject, subj) == 0) {
                        char **ttmp = realloc(messages[i].tags, (messages[i].tcount + 1) * sizeof(char *));
                        if (ttmp) {
                            messages[i].tags = ttmp;
                            messages[i].tags[messages[i].tcount] = strdup(tag);
                            messages[i].tcount++;
                        }
                        break;
                    }
                }
            }
        } else if (strncmp(line, "UNTAG ", 6) == 0) {
            char subj[256], tag[256];
            if (sscanf(line + 6, "%255s %255s", subj, tag) == 2) {
                for (int i = 0; i < mcount; i++) {
                    if (strcmp(messages[i].subject, subj) == 0) {
                        for (int j = 0; j < messages[i].tcount; j++) {
                            if (strcmp(messages[i].tags[j], tag) == 0) {
                                free(messages[i].tags[j]);
                                for (int k = j; k < messages[i].tcount - 1; k++) {
                                    messages[i].tags[k] = messages[i].tags[k + 1];
                                }
                                messages[i].tcount--;
                                break;
                            }
                        }
                        break;
                    }
                }
            }
        } else if (strncmp(line, "DELETE ", 7) == 0) {
            char *subj = line + 7;
            while (*subj == ' ') subj++;
            for (int i = 0; i < mcount; i++) {
                if (strcmp(messages[i].subject, subj) == 0) {
                    free(messages[i].sender);
                    free(messages[i].subject);
                    free(messages[i].body);
                    for (int j = 0; j < messages[i].tcount; j++) {
                        free(messages[i].tags[j]);
                    }
                    free(messages[i].tags);
                    for (int j = i; j < mcount - 1; j++) {
                        messages[j] = messages[j + 1];
                    }
                    mcount--;
                    break;
                }
            }
        } else if (strncmp(line, "SEARCH ", 7) == 0) {
            char *tag = line + 7;
            while (*tag == ' ') tag++;
            for (int i = 0; i < mcount; i++) {
                int has_tag = 0;
                for (int j = 0; j < messages[i].tcount; j++) {
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
            for (int i = 0; i < mcount; i++) {
                printf("From: %s Subj: %s Body: %s Tags:", messages[i].sender, messages[i].subject, messages[i].body);
                for (int j = 0; j < messages[i].tcount; j++) {
                    printf(" %s", messages[i].tags[j]);
                }
                printf("\n");
            }
        }
    }
    free(line);
    for (int i = 0; i < mcount; i++) {
        free(messages[i].sender);
        free(messages[i].subject);
        free(messages[i].body);
        for (int j = 0; j < messages[i].tcount; j++) {
            free(messages[i].tags[j]);
        }
        free(messages[i].tags);
    }
    free(messages);
    return 0;
}
