// F015.c
#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *name;
    char *email;
    char *phone;
    char *note;
} Contact;

int main() {
    Contact *list = NULL;
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

        if (strcmp(cmd, "ADD") == 0 && args) {
            char *name = args;
            char *space2 = strchr(args, ' ');
            if (space2) {
                *space2 = '\0';
                char *email = space2 + 1;
                while (*email == ' ') email++;
                char *space3 = strchr(email, ' ');
                if (space3) {
                    *space3 = '\0';
                    char *phone = space3 + 1;
                    while (*phone == ' ') phone++;
                    list = realloc(list, (count + 1) * sizeof(Contact));
                    if (!list) return 1;
                    list[count].name = strdup(name);
                    list[count].email = strdup(email);
                    list[count].phone = strdup(phone);
                    list[count].note = strdup("");
                    count++;
                }
            }
        } else if (strcmp(cmd, "NOTE") == 0 && args) {
            char *name = args;
            char *space2 = strchr(args, ' ');
            char *text = "";
            if (space2) {
                *space2 = '\0';
                text = space2 + 1;
                while (*text == ' ') text++;
            }
            for (int i = 0; i < count; i++) {
                if (strcmp(list[i].name, name) == 0) {
                    free(list[i].note);
                    list[i].note = strdup(text);
                    break;
                }
            }
        } else if (strcmp(cmd, "MERGE") == 0 && args) {
            char *keep = args;
            char *space2 = strchr(args, ' ');
            if (space2) {
                *space2 = '\0';
                char *remove = space2 + 1;
                while (*remove == ' ') remove++;

                int k_idx = -1, r_idx = -1;
                for (int i = 0; i < count; i++) {
                    if (strcmp(list[i].name, keep) == 0) k_idx = i;
                    if (strcmp(list[i].name, remove) == 0) r_idx = i;
                }

                if (k_idx != -1 && r_idx != -1) {
                    size_t nlen = strlen(list[k_idx].note) + strlen(list[r_idx].note) + 2;
                    char *new_note = malloc(nlen);
                    if (!new_note) return 1;
                    strcpy(new_note, list[k_idx].note);
                    if (strlen(list[k_idx].note) > 0 && strlen(list[r_idx].note) > 0) {
                        strcat(new_note, " ");
                    }
                    strcat(new_note, list[r_idx].note);
                    free(list[k_idx].note);
                    list[k_idx].note = new_note;

                    free(list[r_idx].name);
                    free(list[r_idx].email);
                    free(list[r_idx].phone);
                    free(list[r_idx].note);
                    for (int j = r_idx; j < count - 1; j++) {
                        list[j] = list[j + 1];
                    }
                    count--;
                    list = realloc(list, count * sizeof(Contact));
                    if (count > 0 && !list) return 1;
                }
            }
        } else if (strcmp(cmd, "DELETE") == 0 && args) {
            for (int i = 0; i < count; i++) {
                if (strcmp(list[i].name, args) == 0) {
                    free(list[i].name);
                    free(list[i].email);
                    free(list[i].phone);
                    free(list[i].note);
                    for (int j = i; j < count - 1; j++) {
                        list[j] = list[j + 1];
                    }
                    count--;
                    list = realloc(list, count * sizeof(Contact));
                    if (count > 0 && !list) return 1;
                    break;
                }
            }
        } else if (strcmp(cmd, "FIND") == 0 && args) {
            for (int i = 0; i < count; i++) {
                if (strcmp(list[i].name, args) == 0) {
                    printf("%s %s %s %s\n", list[i].name, list[i].email, list[i].phone, list[i].note);
                    break;
                }
            }
        } else if (strcmp(cmd, "LIST") == 0) {
            for (int i = 0; i < count; i++) {
                printf("%s %s %s %s\n", list[i].name, list[i].email, list[i].phone, list[i].note);
            }
        }
    }

    free(line);
    for (int i = 0; i < count; i++) {
        free(list[i].name);
        free(list[i].email);
        free(list[i].phone);
        free(list[i].note);
    }
    free(list);
    return 0;
}
