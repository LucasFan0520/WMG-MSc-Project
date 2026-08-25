// F016.c
#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *name;
    char *email;
    char *phone;
} Contact;

int main(int argc, char **argv) {
    Contact *list = NULL;
    int count = 0;
    char *line = NULL;
    size_t len = 0;
    ssize_t read;

    if (argc > 1) {
        FILE *f = fopen(argv[1], "r");
        if (f) {
            while ((read = getline(&line, &len, f)) != -1) {
                if (read > 0 && line[read - 1] == '\n') {
                    line[read - 1] = '\0';
                }
                char *c1 = strchr(line, ',');
                if (c1) {
                    char *c2 = strchr(c1 + 1, ',');
                    if (c2) {
                        *c1 = '\0';
                        *c2 = '\0';
                        char *name = line;
                        char *email = c1 + 1;
                        char *phone = c2 + 1;
                        Contact *tmp = realloc(list, sizeof(Contact) * (count + 1));
                        if (tmp) {
                            list = tmp;
                            list[count].name = strdup(name);
                            list[count].email = strdup(email);
                            list[count].phone = strdup(phone);
                            if (list[count].name && list[count].email && list[count].phone) {
                                count++;
                            }
                        }
                    }
                }
            }
            fclose(f);
        }
    }

    while ((read = getline(&line, &len, stdin)) != -1) {
        if (read > 0 && line[read - 1] == '\n') {
            line[read - 1] = '\0';
        }
        if (strcmp(line, "END") == 0) {
            break;
        }
        if (strncmp(line, "ADD ", 4) == 0) {
            char *p = line + 4;
            char *c1 = strchr(p, ',');
            if (c1) {
                char *c2 = strchr(c1 + 1, ',');
                if (c2) {
                    *c1 = '\0';
                    *c2 = '\0';
                    char *name = p;
                    char *email = c1 + 1;
                    char *phone = c2 + 1;
                    Contact *tmp = realloc(list, sizeof(Contact) * (count + 1));
                    if (tmp) {
                        list = tmp;
                        list[count].name = strdup(name);
                        list[count].email = strdup(email);
                        list[count].phone = strdup(phone);
                        if (list[count].name && list[count].email && list[count].phone) {
                            count++;
                        }
                    }
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
                for (int i = found; i < count - 1; i++) {
                    list[i] = list[i + 1];
                }
                count--;
            }
        } else if (strncmp(line, "FIND ", 5) == 0) {
            char *name = line + 5;
            for (int i = 0; i < count; i++) {
                if (strcmp(list[i].name, name) == 0) {
                    printf("%s,%s,%s\n", list[i].name, list[i].email, list[i].phone);
                    break;
                }
            }
        } else if (strcmp(line, "REPORT") == 0) {
            for (int i = 0; i < count; i++) {
                printf("%s,%s,%s\n", list[i].name, list[i].email, list[i].phone);
            }
        }
    }
    for (int i = 0; i < count; i++) {
        free(list[i].name);
        free(list[i].email);
        free(list[i].phone);
    }
    free(list);
    free(line);
    return 0;
}
