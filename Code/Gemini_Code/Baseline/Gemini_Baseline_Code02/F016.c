// F016.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *read_line(FILE *f) {
    size_t size = 16;
    size_t len = 0;
    char *str = malloc(size);
    if (!str) return NULL;
    int c;
    while ((c = fgetc(f)) != EOF && c != '\n') {
        if (len + 1 >= size) {
            size *= 2;
            char *new_str = realloc(str, size);
            if (!new_str) { free(str); return NULL; }
            str = new_str;
        }
        str[len++] = c;
    }
    if (len == 0 && c == EOF) { free(str); return NULL; }
    str[len] = '\0';
    return str;
}

typedef struct {
    char *name;
    char *email;
    char *phone;
} Contact;

int main(int argc, char **argv) {
    Contact *contacts = NULL;
    int count = 0;
    if (argc > 1) {
        FILE *f = fopen(argv[1], "r");
        if (f) {
            while (1) {
                char *line = read_line(f);
                if (!line) break;
                char *c1 = strchr(line, ',');
                if (c1) {
                    char *c2 = strchr(c1 + 1, ',');
                    if (c2) {
                        *c1 = '\0';
                        *c2 = '\0';
                        contacts = realloc(contacts, sizeof(Contact) * (count + 1));
                        contacts[count].name = strdup(line);
                        contacts[count].email = strdup(c1 + 1);
                        contacts[count].phone = strdup(c2 + 1);
                        count++;
                    }
                }
                free(line);
            }
            fclose(f);
        }
    }
    while (1) {
        char *line = read_line(stdin);
        if (!line) break;
        if (strcmp(line, "END") == 0) { free(line); break; }
        char *cmd = line;
        char *p1 = strchr(cmd, ' ');
        if (!p1) {
            if (strcmp(cmd, "REPORT") == 0) {
                for (int i = 0; i < count; i++) {
                    printf("%s,%s,%s\n", contacts[i].name, contacts[i].email, contacts[i].phone);
                }
            }
            free(line);
            continue;
        }
        *p1 = '\0';
        char *args = p1 + 1;
        if (strcmp(cmd, "ADD") == 0) {
            char *c1 = strchr(args, ',');
            if (c1) {
                char *c2 = strchr(c1 + 1, ',');
                if (c2) {
                    *c1 = '\0';
                    *c2 = '\0';
                    contacts = realloc(contacts, sizeof(Contact) * (count + 1));
                    contacts[count].name = strdup(args);
                    contacts[count].email = strdup(c1 + 1);
                    contacts[count].phone = strdup(c2 + 1);
                    count++;
                }
            }
        } else if (strcmp(cmd, "DELETE") == 0) {
            char *name = args;
            for (int i = 0; i < count; i++) {
                if (strcmp(contacts[i].name, name) == 0) {
                    free(contacts[i].name);
                    free(contacts[i].email);
                    free(contacts[i].phone);
                    for (int j = i; j < count - 1; j++) {
                        contacts[j] = contacts[j + 1];
                    }
                    count--;
                    if (count == 0) { free(contacts); contacts = NULL; }
                    else { contacts = realloc(contacts, sizeof(Contact) * count); }
                    break;
                }
            }
        } else if (strcmp(cmd, "FIND") == 0) {
            char *name = args;
            for (int i = 0; i < count; i++) {
                if (strcmp(contacts[i].name, name) == 0) {
                    printf("%s,%s,%s\n", contacts[i].name, contacts[i].email, contacts[i].phone);
                    break;
                }
            }
        }
        free(line);
    }
    for (int i = 0; i < count; i++) {
        free(contacts[i].name);
        free(contacts[i].email);
        free(contacts[i].phone);
    }
    free(contacts);
    return 0;
}
