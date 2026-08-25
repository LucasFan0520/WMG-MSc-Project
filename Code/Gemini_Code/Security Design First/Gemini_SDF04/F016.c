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

Contact *contacts = NULL;
int count = 0;
int capacity = 0;

void add_contact(const char *name, const char *email, const char *phone) {
    if (count >= capacity) {
        capacity = capacity == 0 ? 4 : capacity * 2;
        Contact *tmp = realloc(contacts, capacity * sizeof(Contact));
        if (!tmp) return;
        contacts = tmp;
    }
    contacts[count].name = strdup(name);
    contacts[count].email = strdup(email);
    contacts[count].phone = strdup(phone);
    if (contacts[count].name && contacts[count].email && contacts[count].phone) {
        count++;
    }
}

int main(int argc, char **argv) {
    if (argc < 2) {
        return 1;
    }
    FILE *f = fopen(argv[1], "r");
    if (f) {
        char *fline = NULL;
        size_t flen = 0;
        ssize_t nread;
        while ((nread = getline(&fline, &flen, f)) != -1) {
            size_t l = strlen(fline);
            if (l > 0 && fline[l - 1] == '\n') fline[l - 1] = '\0';
            char *comma1 = strchr(fline, ',');
            if (comma1) {
                *comma1 = '\0';
                char *email = comma1 + 1;
                char *comma2 = strchr(email, ',');
                if (comma2) {
                    *comma2 = '\0';
                    char *phone = comma2 + 1;
                    add_contact(fline, email, phone);
                }
            }
        }
        free(fline);
        fclose(f);
    }
    char *line = NULL;
    size_t len = 0;
    ssize_t nread;
    while ((nread = getline(&line, &len, stdin)) != -1) {
        size_t l = strlen(line);
        if (l > 0 && line[l - 1] == '\n') line[l - 1] = '\0';
        char *cmd = line;
        char *space1 = strchr(cmd, ' ');
        if (space1) {
            *space1 = '\0';
            char *arg1 = space1 + 1;
            if (strcmp(cmd, "ADD") == 0) {
                char *comma1 = strchr(arg1, ',');
                if (comma1) {
                    *comma1 = '\0';
                    char *email = comma1 + 1;
                    char *comma2 = strchr(email, ',');
                    if (comma2) {
                        *comma2 = '\0';
                        char *phone = comma2 + 1;
                        add_contact(arg1, email, phone);
                    }
                }
            } else if (strcmp(cmd, "DELETE") == 0) {
                for (int i = 0; i < count; ) {
                    if (strcmp(contacts[i].name, arg1) == 0) {
                        free(contacts[i].name);
                        free(contacts[i].email);
                        free(contacts[i].phone);
                        for (int j = i; j < count - 1; j++) {
                            contacts[j] = contacts[j + 1];
                        }
                        count--;
                    } else {
                        i++;
                    }
                }
            } else if (strcmp(cmd, "FIND") == 0) {
                for (int i = 0; i < count; i++) {
                    if (strcmp(contacts[i].name, arg1) == 0) {
                        printf("%s,%s,%s\n", contacts[i].name, contacts[i].email, contacts[i].phone);
                    }
                }
            }
        } else {
            if (strcmp(cmd, "REPORT") == 0) {
                for (int i = 0; i < count; i++) {
                    printf("%s,%s,%s\n", contacts[i].name, contacts[i].email, contacts[i].phone);
                }
            } else if (strcmp(cmd, "END") == 0) {
                break;
            }
        }
    }
    free(line);
    for (int i = 0; i < count; i++) {
        free(contacts[i].name);
        free(contacts[i].email);
        free(contacts[i].phone);
    }
    free(contacts);
    return 0;
}