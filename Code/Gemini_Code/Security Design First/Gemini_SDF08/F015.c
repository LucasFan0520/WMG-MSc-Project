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

int main(void) {
    Contact *list = NULL;
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
        if (strncmp(line, "ADD ", 4) == 0) {
            char *p = line + 4;
            char *s1 = strchr(p, ' ');
            if (s1) {
                *s1 = '\0';
                char *name = p;
                char *p2 = s1 + 1;
                char *s2 = strchr(p2, ' ');
                if (s2) {
                    *s2 = '\0';
                    char *email = p2;
                    char *phone = s2 + 1;
                    Contact *tmp = realloc(list, sizeof(Contact) * (count + 1));
                    if (tmp) {
                        list = tmp;
                        list[count].name = strdup(name);
                        list[count].email = strdup(email);
                        list[count].phone = strdup(phone);
                        list[count].note = strdup("");
                        if (list[count].name && list[count].email && list[count].phone && list[count].note) {
                            count++;
                        }
                    }
                }
            }
        } else if (strncmp(line, "NOTE ", 5) == 0) {
            char *p = line + 5;
            char *space = strchr(p, ' ');
            if (space) {
                *space = '\0';
                char *name = p;
                char *text = space + 1;
                for (int i = 0; i < count; i++) {
                    if (strcmp(list[i].name, name) == 0) {
                        char *nt = strdup(text);
                        if (nt) {
                            free(list[i].note);
                            list[i].note = nt;
                        }
                        break;
                    }
                }
            }
        } else if (strncmp(line, "MERGE ", 6) == 0) {
            char *p = line + 6;
            char *space = strchr(p, ' ');
            if (space) {
                *space = '\0';
                char *keep = p;
                char *remove = space + 1;
                int k_idx = -1, r_idx = -1;
                for (int i = 0; i < count; i++) {
                    if (strcmp(list[i].name, keep) == 0) k_idx = i;
                    if (strcmp(list[i].name, remove) == 0) r_idx = i;
                }
                if (k_idx != -1 && r_idx != -1) {
                    size_t l1 = strlen(list[k_idx].note);
                    size_t l2 = strlen(list[r_idx].note);
                    char *new_note = malloc(l1 + l2 + 2);
                    if (new_note) {
                        strcpy(new_note, list[k_idx].note);
                        if (l1 > 0 && l2 > 0) strcat(new_note, " ");
                        strcat(new_note, list[r_idx].note);
                        free(list[k_idx].note);
                        list[k_idx].note = new_note;
                    }
                    free(list[r_idx].name);
                    free(list[r_idx].email);
                    free(list[r_idx].phone);
                    free(list[r_idx].note);
                    for (int i = r_idx; i < count - 1; i++) {
                        list[i] = list[i + 1];
                    }
                    count--;
                }
            }
        } else if (strncmp(line, "DELETE ", 7) == 0) {
            char *name = line + 7;
            int found = -1;
            for (int i = 0; i < count; i++) {
                if (strcmp(list[i].name, name) == 0) {
                    found = i;
                    break;
                }
            }
            if (found != -1) {
                free(list[found].name);
                free(list[found].email);
                free(list[found].phone);
                free(list[found].note);
                for (int i = found; i < count - 1; i++) {
                    list[i] = list[i + 1];
                }
                count--;
            }
        } else if (strncmp(line, "FIND ", 5) == 0) {
            char *name = line + 5;
            for (int i = 0; i < count; i++) {
                if (strcmp(list[i].name, name) == 0) {
                    printf("%s %s %s %s\n", list[i].name, list[i].email, list[i].phone, list[i].note);
                    break;
                }
            }
        } else if (strcmp(line, "LIST") == 0) {
            for (int i = 0; i < count; i++) {
                printf("%s %s %s %s\n", list[i].name, list[i].email, list[i].phone, list[i].note);
            }
        }
    }
    for (int i = 0; i < count; i++) {
        free(list[i].name);
        free(list[i].email);
        free(list[i].phone);
        free(list[i].note);
    }
    free(list);
    free(line);
    return 0;
}
