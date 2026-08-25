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
} MailMessage;

int main() {
    MailMessage *box = NULL;
    int count = 0;
    char *line = NULL;
    size_t len = 0;
    ssize_t read;

    while ((read = getline(&line, &len, stdin)) != -1) {
        line[strcspn(line, "\r\n")] = '\0';
        if (strcmp(line, "END") == 0) {
            break;
        }

        char *cmd = line;
        char *space1 = strchr(line, ' ');
        char *args = NULL;
        if (space1) {
            *space1 = '\0';
            args = space1 + 1;
            while (*args == ' ') args++;
        }

        if (strcmp(cmd, "MAIL") == 0 && args) {
            char *sender = args;
            char *space2 = strchr(args, ' ');
            if (space2) {
                *space2 = '\0';
                char *subject = space2 + 1;
                while (*subject == ' ') subject++;
                char *space3 = strchr(subject, ' ');
                char *body = "";
                if (space3) {
                    *space3 = '\0';
                    body = space3 + 1;
                    while (*body == ' ') body++;
                }
                box = realloc(box, (count + 1) * sizeof(MailMessage));
                if (!box) return 1;
                box[count].sender = strdup(sender);
                box[count].subject = strdup(subject);
                box[count].body = strdup(body);
                box[count].tags = NULL;
                box[count].tag_count = 0;
                count++;
            }
        } else if (strcmp(cmd, "TAG") == 0 && args) {
            char *subject = args;
            char *space2 = strchr(args, ' ');
            if (space2) {
                *space2 = '\0';
                char *tag = space2 + 1;
                while (*tag == ' ') tag++;
                for (int i = 0; i < count; i++) {
                    if (strcmp(box[i].subject, subject) == 0) {
                        int tc = box[i].tag_count;
                        box[i].tags = realloc(box[i].tags, (tc + 1) * sizeof(char *));
                        if (!box[i].tags) return 1;
                        box[i].tags[tc] = strdup(tag);
                        box[i].tag_count++;
                        break;
                    }
                }
            }
        } else if (strcmp(cmd, "UNTAG") == 0 && args) {
            char *subject = args;
            char *space2 = strchr(args, ' ');
            if (space2) {
                *space2 = '\0';
                char *tag = space2 + 1;
                while (*tag == ' ') tag++;
                for (int i = 0; i < count; i++) {
                    if (strcmp(box[i].subject, subject) == 0) {
                        for (int j = 0; j < box[i].tag_count; j++) {
                            if (strcmp(box[i].tags[j], tag) == 0) {
                                free(box[i].tags[j]);
                                for (int k = j; k < box[i].tag_count - 1; k++) {
                                    box[i].tags[k] = box[i].tags[k + 1];
                                }
                                box[i].tag_count--;
                                box[i].tags = realloc(box[i].tags, box[i].tag_count * sizeof(char *));
                                if (box[i].tag_count > 0 && !box[i].tags) return 1;
                                break;
                            }
                        }
                        break;
                    }
                }
            }
        } else if (strcmp(cmd, "DELETE") == 0 && args) {
            for (int i = 0; i < count; i++) {
                if (strcmp(box[i].subject, args) == 0) {
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
                    box = realloc(box, count * sizeof(MailMessage));
                    if (count > 0 && !box) return 1;
                    break;
                }
            }
        } else if (strcmp(cmd, "SEARCH") == 0 && args) {
            for (int i = 0; i < count; i++) {
                int has_tag = 0;
                for (int j = 0; j < box[i].tag_count; j++) {
                    if (strcmp(box[i].tags[j], args) == 0) {
                        has_tag = 1;
                        break;
                    }
                }
                if (has_tag) {
                    printf("%s\n", box[i].subject);
                }
            }
        } else if (strcmp(cmd, "REPORT") == 0) {
            for (int i = 0; i < count; i++) {
                printf("%s %s %s", box[i].sender, box[i].subject, box[i].body);
                for (int j = 0; j < box[i].tag_count; j++) {
                    printf(" #%s", box[i].tags[j]);
                }
                putchar('\n');
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
