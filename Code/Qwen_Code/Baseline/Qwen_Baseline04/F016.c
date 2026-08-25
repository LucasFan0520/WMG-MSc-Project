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
int ccount = 0;

int find_contact(const char *name) {
    for (int i = 0; i < ccount; i++) {
        if (strcmp(contacts[i].name, name) == 0) return i;
    }
    return -1;
}

void add_contact(char *line) {
    char *name = strtok(line, ",");
    char *email = strtok(NULL, ",");
    char *phone = strtok(NULL, ",\n");
    if (name && email && phone) {
        contacts = realloc(contacts, sizeof(Contact) * (ccount + 1));
        contacts[ccount].name = strdup(name);
        contacts[ccount].email = strdup(email);
        contacts[ccount].phone = strdup(phone);
        ccount++;
    }
}

int main(int argc, char *argv[]) {
    if (argc > 1) {
        FILE *f = fopen(argv[1], "r");
        if (f) {
            char line[1000];
            while (fgets(line, sizeof(line), f)) {
                line[strcspn(line, "\n")] = 0;
                add_contact(line);
            }
            fclose(f);
        }
    }
    
    char cmd[20];
    while (scanf("%s", cmd) == 1) {
        if (strcmp(cmd, "END") == 0) break;
        if (strcmp(cmd, "ADD") == 0) {
            char line[1000];
            scanf(" %[^\n]", line);
            add_contact(line);
        } else if (strcmp(cmd, "DELETE") == 0) {
            char name[100];
            scanf("%s", name);
            int idx = find_contact(name);
            if (idx != -1) {
                free(contacts[idx].name);
                free(contacts[idx].email);
                free(contacts[idx].phone);
                for (int i = idx; i < ccount - 1; i++) contacts[i] = contacts[i+1];
                ccount--;
            }
        } else if (strcmp(cmd, "FIND") == 0) {
            char name[100];
            scanf("%s", name);
            int idx = find_contact(name);
            if (idx != -1) {
                printf("%s,%s,%s\n", contacts[idx].name, contacts[idx].email, contacts[idx].phone);
            }
        } else if (strcmp(cmd, "REPORT") == 0) {
            for (int i = 0; i < ccount; i++) {
                printf("%s,%s,%s\n", contacts[i].name, contacts[i].email, contacts[i].phone);
            }
        }
    }
    for (int i = 0; i < ccount; i++) {
        free(contacts[i].name);
        free(contacts[i].email);
        free(contacts[i].phone);
    }
    free(contacts);
    return 0;
}