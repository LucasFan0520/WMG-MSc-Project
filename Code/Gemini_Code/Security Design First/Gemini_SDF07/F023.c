/* F023.c */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *sender;
    char *subject;
    char *body;
    char **tags;
    int tag_count;
    int tag_cap;
} MailMessage;

int main(void) {
    MailMessage *box = NULL;
    int count = 0;
    int capacity = 0;
    char *line = NULL;
    size_t len = 0;
    ssize_t read;

    while ((read = getline(&line, &len, stdin)) != -1) {
        line[strcspn(line, "\r\n")] = '\0';
        if (strcmp(line, "END") == 0) {
            break;
        }
        if (strncmp(line, "MAIL ", 5) == 0) {
            char *args = line + 5;
            char *space1 = strchr(args, ' ');
            if (space1) {
                *space1 = '\0';
                char *snd = args;
                char *rest = space1 + 1;
                char *space2 = strchr(rest, ' ');
                char *sbj = NULL;
                char *bdy = NULL;
                if (space2) {
                    *space2 = '\0';
                    sbj = rest;
                    bdy = space2 + 1;
                } else {
                    sbj = rest;
                    bdy = "";
                }
                char *n_snd = strdup(snd);
                char *n_sbj = strdup(sbj);
                char *n_bdy = strdup(bdy);
                if (n_snd && n_sbj && n_bdy) {
                    if (count >= capacity) {
                        int new_cap = capacity == 0 ? 4 : capacity * 2;
                        MailMessage *tmp = realloc(box, new_cap * sizeof(MailMessage));
                        if (tmp) {
                            box = tmp;
                            capacity = new_cap;
                        } else {
                            free(n_snd); free(n_sbj); free(n_bdy);
                            *space1 = ' '; if (space2) *space2 = ' ';
                            continue;
                        }
                    }
                    box[count].sender = n_snd;
                    box[count].subject = n_sbj;
                    box[count].body = n_bdy;
                    box[count].tags = NULL;
                    box[count].tag_count = 0;
                    box[count].tag_cap = 0;
                    count++;
                } else {
                    free(n_snd); free(n_sbj); free(n_bdy);
                }
                *space1 = ' ';
                if (space2) *space2 = ' ';
            }
        } else if (strncmp(line, "TAG ", 4) == 0) {
            char *args = line + 4;
            char *space = strchr(args, ' ');
            if (space) {
                *space = '\0';
                char *sbj = args;
                char *tg = space + 1;
                for (int i = 0; i < count; i++) {
                    if (strcmp(box[i].subject, sbj) == 0) {
                        char *n_tg = strdup(tg);
                        if (n_tg) {
                            if (box[i].tag_count >= box[i].tag_cap) {
                                int new_tcap = box[i].tag_cap == 0 ? 4 : box[i].tag_cap * 2;
                                char **tmp = realloc(box[i].tags, new_tcap * sizeof(char *));
                                if (tmp) {
                                    box[i].tags = tmp;
                                    box[i].tag_cap = new_tcap;
                                } else {
                                    free(n_tg);
                                    break;
                                }
                            }
                            box[i].tags[box[i].tag_count] = n_tg;
                            box[i].tag_count++;
                        }
                        break;
                    }
                }
                *space = ' ';
            }
        } else if (strncmp(line, "UNTAG ", 6) == 0) {
            char *args = line + 6;
            char *space = strchr(args, ' ');
            if (space) {
                *space = '\0';
                char *sbj = args;
                char *tg = space + 1;
                for (int i = 0; i < count; i++) {
                    if (strcmp(box[i].subject, sbj) == 0) {
                        for (int j = 0; j < box[i].tag_count; j++) {
                            if (strcmp(box[i].tags[j], tg) == 0) {
                                free(box[i].tags[j]);
                                for (int k = j; k < box[i].tag_count - 1; k++) {
                                    box[i].tags[k] = box[i].tags[k + 1];
                                }
                                box[i].tag_count--;
                                break;
                            }
                        }
                        break;
                    }
                }
                *space = ' ';
            }
        } else if (strncmp(line, "DELETE ", 7) == 0) {
            char *sbj = line + 7;
            for (int i = 0; i < count; i++) {
                if (strcmp(box[i].subject, sbj) == 0) {
                    free(box[i].sender);
                    free(box[i].subject);
                    free(box[i].body);
                    for (int j = 0; j < box[i].tag_count; j++) {
                        free(box[i].tags[j]);
                    }
                    free(box[i].tags);
                    for (int j = i; j < count - 1; j++) {
                        box[j] = box[j + 1];
                    }
                    count--;
                    break;
                }
            }
        } else if (strncmp(line, "SEARCH ", 7) == 0) {
            char *tg = line + 7;
            for (int i = 0; i < count; i++) {
                int found = 0;
                for (int j = 0; j < box[i].tag_count; j++) {
                    if (strcmp(box[i].tags[j], tg) == 0) {
                        found = 1;
                        break;
                    }
                }
                if (found) {
                    printf("%s\n", box[i].subject);
                }
            }
        } else if (strcmp(line, "REPORT") == 0) {
            for (int i = 0; i < count; i++) {
                printf("%s %s %s\n", box[i].sender, box[i].subject, box[i].body);
            }
        }
    }
    free(line);
    for (int i = 0; i < count; i++) {
        free(box[i].sender);
        free(box[i].subject);
        free(box[i].body);
        for (int j = 0; j < box[i].tag_count; j++) {
            free(box[i].tags[j]);
        }
        free(box[i].tags);
    }
    free(box);
    return 0;
}
