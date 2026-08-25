// F016.c
#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *name;
    char *email;
    char *phone;
} CSVContact;

CSVContact *contacts = NULL;
int count = 0;

void add_contact(const char *name, const char *email, const char *phone) {
    contacts = realloc(contacts, (count + 1) * sizeof(CSVContact));
    if (!contacts) exit(1);
    contacts[count].name = strdup(name);
    contacts[count].email = strdup(email);
    contacts[count].phone = strdup(phone);
    count++;
}

int parse_csv_line(char *line, char **name, char **email, char **phone) {
    char *c1 = strchr(line, ',');
    if (!c1) return 0;
    *c1 = '\0';
    char *c2 = strchr(c1 + 1, ',');
    if (!c2) return 0;
    *c2 = '\0';
    *name = line;
    *email = c1 + 1;
    *phone = c2 + 1;
    return 1;
}

int main(int argc, char **argv) {
    if (argc < 2) return 1;
    FILE *fp = fopen(argv[1], "r");
    if (fp) {
        char *fline = NULL;
        size_t flen = 0;
        ssize_t fread;
        while ((fread = getline(&fline, &flen, fp)) != -1) {
            fline[strcspn(fline, "\r\n")] = '\0';
            char *n, *e, *p;
            if (parse_csv_line(fline, &n, &e, &p)) {
                add_contact(n, e, p);
            }
        }
        free(fline);
        fclose(fp);
    }

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
            char *n, *e, *p;
            if (parse_csv_line(args, &n, &e, &p)) {
                add_contact(n, e, p);
            }
        } else if (strcmp(cmd, "DELETE") == 0 && args) {
            for (int i = 0; i < count; i++) {
                if (strcmp(contacts[i].name, args) == 0) {
                    free(contacts[i].name);
                    free(contacts[i].email);
                    free(contacts[i].phone);
                    for (int j = i; j < count - 1; j++) {
                        contacts[j] = contacts[j + 1];
                    }
                    count--;
                    contacts = realloc(contacts, count * sizeof(CSVContact));
                    if (count > 0 && !contacts) return 1;
                    break;
                }
            }
        } else if (strcmp(cmd, "FIND") == 0 && args) {
            for (int i = 0; i < count; i++) {
                if (strcmp(contacts[i].name, args) == 0) {
                    printf("%s,%s,%s\n", contacts[i].name, contacts[i].email, contacts[i].phone);
                    break;
                }
            }
        } else if (strcmp(cmd, "REPORT") == 0) {
            for (int i = 0; i < count; i++) {
                printf("%s,%s,%s\n", contacts[i].name, contacts[i].email, contacts[i].phone);
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
