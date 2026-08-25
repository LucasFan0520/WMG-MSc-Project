// F016.c
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

int find_name(const char *name) {
    for (int i = 0; i < count; i++) {
        if (strcmp(contacts[i].name, name) == 0) {
            return i;
        }
    }
    return -1;
}

void load_file(const char *path) {
    FILE *f = fopen(path, "r");
    if (!f) {
        return;
    }
    char line[10000];
    while (fgets(line, sizeof(line), f)) {
        line[strcspn(line, "\n")] = 0;
        char *n = strtok(line, ",");
        char *e = strtok(NULL, ",");
        char *p = strtok(NULL, ",");
        if (n && e && p) {
            contacts = realloc(contacts, sizeof(Contact) * (count + 1));
            contacts[count].name = strdup(n);
            contacts[count].email = strdup(e);
            contacts[count].phone = strdup(p);
            count++;
        }
    }
    fclose(f);
}

int main(int argc, char **argv) {
    if (argc > 1) {
        load_file(argv[1]);
    }
    char cmd[20];
    char name[1000];
    char line[10000];

    while (scanf("%s", cmd) == 1 && strcmp(cmd, "END") != 0) {
        if (strcmp(cmd, "ADD") == 0) {
            scanf("%s", line);
            char *n = strtok(line, ",");
            char *e = strtok(NULL, ",");
            char *p = strtok(NULL, ",");
            if (n && e && p) {
                contacts = realloc(contacts, sizeof(Contact) * (count + 1));
                contacts[count].name = strdup(n);
                contacts[count].email = strdup(e);
                contacts[count].phone = strdup(p);
                count++;
            }
        } else if (strcmp(cmd, "DELETE") == 0) {
            scanf("%s", name);
            int idx = find_name(name);
            if (idx != -1) {
                free(contacts[idx].name);
                free(contacts[idx].email);
                free(contacts[idx].phone);
                for (int i = idx; i < count - 1; i++) {
                    contacts[i] = contacts[i + 1];
                }
                count--;
            }
        } else if (strcmp(cmd, "FIND") == 0) {
            scanf("%s", name);
            int idx = find_name(name);
            if (idx != -1) {
                printf("%s,%s,%s\n", contacts[idx].name, contacts[idx].email, contacts[idx].phone);
            }
        } else if (strcmp(cmd, "REPORT") == 0) {
            for (int i = 0; i < count; i++) {
                printf("%s,%s,%s\n", contacts[i].name, contacts[i].email, contacts[i].phone);
            }
        }
    }

    for (int i = 0; i < count; i++) {
        free(contacts[i].name);
        free(contacts[i].email);
        free(contacts[i].phone);
    }
    free(contacts);

    return 0;
}