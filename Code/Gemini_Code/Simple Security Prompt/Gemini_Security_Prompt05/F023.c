// F023.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *read_line(FILE *f) {
    int cap = 16;
    int len = 0;
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

char *get_token(char **p) {
    while (**p == ' ') (*p)++;
    if (**p == '\0') return NULL;
    char *start = *p;
    while (**p && **p != ' ') (*p)++;
    if (**p) {
        **p = '\0';
        (*p)++;
    }
    return start;
}

char *get_rest(char **p) {
    while (**p == ' ') (*p)++;
    return **p ? *p : "";
}

typedef struct {
    char *sender;
    char *subject;
    char *body;
    char **tags;
    int tcount;
    int tcap;
} Mail;

int main() {
    Mail *arr = NULL;
    int count = 0;
    int cap = 0;
    while (1) {
        char *line = read_line(stdin);
        if (!line) break;
        char *p = line;
        char *cmd = get_token(&p);
        if (!cmd || strcmp(cmd, "END") == 0) {
            free(line);
            break;
        }
        if (strcmp(cmd, "MAIL") == 0) {
            char *sender = get_token(&p);
            char *subj = get_token(&p);
            char *body = get_rest(&p);
            if (sender && subj) {
                if (count >= cap) {
                    cap = cap == 0 ? 4 : cap * 2;
                    Mail *narr = realloc(arr, cap * sizeof(Mail));
                    if (narr) arr = narr;
                }
                arr[count].sender = strdup(sender);
                arr[count].subject = strdup(subj);
                arr[count].body = strdup(body);
                arr[count].tags = NULL;
                arr[count].tcount = 0;
                arr[count].tcap = 0;
                count++;
            }
        } else if (strcmp(cmd, "TAG") == 0) {
            char *subj = get_token(&p);
            char *tag = get_token(&p);
            if (subj && tag) {
                for (int i = 0; i < count; i++) {
                    if (strcmp(arr[i].subject, subj) == 0) {
                        Mail *m = &arr[i];
                        if (m->tcount >= m->tcap) {
                            m->tcap = m->tcap == 0 ? 4 : m->tcap * 2;
                            char **ntags = realloc(m->tags, m->tcap * sizeof(char *));
                            if (ntags) m->tags = ntags;
                        }
                        m->tags[m->tcount] = strdup(tag);
                        m->tcount++;
                        break;
                    }
                }
            }
        } else if (strcmp(cmd, "UNTAG") == 0) {
            char *subj = get_token(&p);
            char *tag = get_token(&p);
            if (subj && tag) {
                for (int i = 0; i < count; i++) {
                    if (strcmp(arr[i].subject, subj) == 0) {
                        for (int j = 0; j < arr[i].tcount; j++) {
                            if (strcmp(arr[i].tags[j], tag) == 0) {
                                free(arr[i].tags[j]);
                                for (int k = j; k < arr[i].tcount - 1; k++) {
                                    arr[i].tags[k] = arr[i].tags[k + 1];
                                }
                                arr[i].tcount--;
                                break;
                            }
                        }
                        break;
                    }
                }
            }
        } else if (strcmp(cmd, "DELETE") == 0) {
            char *subj = get_token(&p);
            if (subj) {
                for (int i = 0; i < count; i++) {
                    if (strcmp(arr[i].subject, subj) == 0) {
                        free(arr[i].sender);
                        free(arr[i].subject);
                        free(arr[i].body);
                        for (int j = 0; j < arr[i].tcount; j++) {
                            free(arr[i].tags[j]);
                        }
                        free(arr[i].tags);
                        for (int j = i; j < count - 1; j++) {
                            arr[j] = arr[j + 1];
                        }
                        count--;
                        break;
                    }
                }
            }
        } else if (strcmp(cmd, "SEARCH") == 0) {
            char *tag = get_token(&p);
            if (tag) {
                for (int i = 0; i < count; i++) {
                    int match = 0;
                    for (int j = 0; j < arr[i].tcount; j++) {
                        if (strcmp(arr[i].tags[j], tag) == 0) {
                            match = 1;
                            break;
                        }
                    }
                    if (match) {
                        printf("%s\n", arr[i].subject);
                    }
                }
            }
        } else if (strcmp(cmd, "REPORT") == 0) {
            for (int i = 0; i < count; i++) {
                printf("%s %s %s\n", arr[i].sender, arr[i].subject, arr[i].body);
            }
        }
        free(line);
    }
    for (int i = 0; i < count; i++) {
        free(arr[i].sender);
        free(arr[i].subject);
        free(arr[i].body);
        for (int j = 0; j < arr[i].tcount; j++) {
            free(arr[i].tags[j]);
        }
        free(arr[i].tags);
    }
    free(arr);
    return 0;
}
