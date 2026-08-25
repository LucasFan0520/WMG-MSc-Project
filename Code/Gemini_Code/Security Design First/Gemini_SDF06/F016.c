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
    if (argc < 2) {
        return 1;
    }

    Contact *contacts = NULL;
    size_t count = 0;
    size_t capacity = 0;

    FILE *fp = fopen(argv[1], "r");
    if (!fp) {
        return 1;
    }

    char *line = NULL;
    size_t len = 0;
    ssize_t read;

    while ((read = getline(&line, &len, fp)) != -1) {
        line[strcspn(line, "\r\n")] = '\0';
        char *comma1 = strchr(line, ',');
        if (!comma1) continue;
        char *comma2 = strchr(comma1 + 1, ',');
        if (!comma2) continue;

        *comma1 = '\0';
        *comma2 = '\0';

        char *name = line;
        char *email = comma1 + 1;
        char *phone = comma2 + 1;

        if (count >= capacity) {
            size_t new_cap = capacity == 0 ? 4 : capacity * 2;
            Contact *new_c = realloc(contacts, new_cap * sizeof(Contact));
            if (!new_c) continue;
            contacts = new_c;
            capacity = new_cap;
        }

        char *n = strdup(name);
        char *e = strdup(email);
        char *ph = strdup(phone);
        if (n && e && ph) {
            contacts[count].name = n;
            contacts[count].email = e;
            contacts[count].phone = ph;
            count++;
        } else {
            free(n);
            free(e);
            free(ph);
        }
    }
    fclose(fp);

    while ((read = getline(&line, &len, stdin)) != -1) {
        line[strcspn(line, "\r\n")] = '\0';
        char *p = line;
        while (*p == ' ' || *p == '\t') p++;
        if (*p == '\0') continue;
        char *cmd = p;
        while (*p && *p != ' ' && *p != '\t') p++;
        if (*p) {
            *p = '\0';
            p++;
        }
        if (strcmp(cmd, "END") == 0) {
            break;
        }
        if (strcmp(cmd, "ADD") == 0) {
            while (*p == ' ' || *p == '\t') p++;
            char *args = p;
            char *comma1 = strchr(args, ',');
            if (!comma1) continue;
            char *comma2 = strchr(comma1 + 1, ',');
            if (!comma2) continue;

            *comma1 = '\0';
            *comma2 = '\0';

            char *name = args;
            char *email = comma1 + 1;
            char *phone = comma2 + 1;

            if (count >= capacity) {
                size_t new_cap = capacity == 0 ? 4 : capacity * 2;
                Contact *new_c = realloc(contacts, new_cap * sizeof(Contact));
                if (!new_c) continue;
                contacts = new_c;
                capacity = new_cap;
            }

            char *n = strdup(name);
            char *e = strdup(email);
            char *ph = strdup(phone);
            if (n && e && ph) {
                contacts[count].name = n;
                contacts[count].email = e;
                contacts[count].phone = ph;
                count++;
            } else {
                free(n);
                free(e);
                free(ph);
            }
        } else if (strcmp(cmd, "DELETE") == 0) {
            while (*p == ' ' || *p == '\t') p++;
            char *name = p;
            while (*p && *p != ' ' && *p != '\t') p++;
            if (*p) *p = '\0';
            if (*name == '\0') continue;

            for (size_t i = 0; i < count; i++) {
                if (strcmp(contacts[i].name, name) == 0) {
                    free(contacts[i].name);
                    free(contacts[i].email);
                    free(contacts[i].phone);
                    for (size_t j = i; j < count - 1; j++) {
                        contacts[j] = contacts[j + 1];
                    }
                    count--;
                    break;
                }
            }
        } else if (strcmp(cmd, "FIND") == 0) {
            while (*p == ' ' || *p == '\t') p++;
            char *name = p;
            while (*p && *p != ' ' && *p != '\t') p++;
            if (*p) *p = '\0';
            if (*name == '\0') continue;

            for (size_t i = 0; i < count; i++) {
                if (strcmp(contacts[i].name, name) == 0) {
                    printf("%s,%s,%s\n", contacts[i].name, contacts[i].email, contacts[i].phone);
                    break;
                }
            }
        } else if (strcmp(cmd, "REPORT") == 0) {
            for (size_t i = 0; i < count; i++) {
                printf("%s,%s,%s\n", contacts[i].name, contacts[i].email, contacts[i].phone);
            }
        }
    }

    for (size_t i = 0; i < count; i++) {
        free(contacts[i].name);
        free(contacts[i].email);
        free(contacts[i].phone);
    }
    free(contacts);
    free(line);
    return 0;
}
