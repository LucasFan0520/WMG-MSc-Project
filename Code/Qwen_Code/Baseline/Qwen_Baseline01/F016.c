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
int n = 0;

int find_contact(const char *name) {
    for (int i = 0; i < n; i++)
        if (strcmp(contacts[i].name, name) == 0) return i;
    return -1;
}

void load(const char *path) {
    FILE *f = fopen(path, "r");
    if (!f) return;
    char line[1000];
    while (fgets(line, sizeof(line), f)) {
        line[strcspn(line, "\n")] = 0;
        char *n1 = strtok(line, ",");
        char *n2 = strtok(NULL, ",");
        char *n3 = strtok(NULL, ",");
        if (n1 && n2 && n3) {
            contacts = realloc(contacts, sizeof(Contact) * (n + 1));
            contacts[n].name = strdup(n1);
            contacts[n].email = strdup(n2);
            contacts[n].phone = strdup(n3);
            n++;
        }
    }
    fclose(f);
}

int main(int argc, char **argv) {
    if (argc > 1) load(argv[1]);
    char line[10000];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        if (strcmp(line, "END") == 0) break;
        char cmd[20], arg1[1000];
        if (sscanf(line, "%s", cmd) != 1) continue;
        if (strcmp(cmd, "ADD") == 0) {
            char *p = line + 4;
            if (*p == ' ') p++;
            char *n1 = strtok(p, ",");
            char *n2 = strtok(NULL, ",");
            char *n3 = strtok(NULL, ",");
            if (n1 && n2 && n3) {
                contacts = realloc(contacts, sizeof(Contact) * (n + 1));
                contacts[n].name = strdup(n1);
                contacts[n].email = strdup(n2);
                contacts[n].phone = strdup(n3);
                n++;
            }
        } else if (strcmp(cmd, "DELETE") == 0) {
            sscanf(line, "%*s %s", arg1);
            int i = find_contact(arg1);
            if (i != -1) {
                free(contacts[i].name);
                free(contacts[i].email);
                free(contacts[i].phone);
                for (int j = i; j < n - 1; j++) contacts[j] = contacts[j + 1];
                n--;
            }
        } else if (strcmp(cmd, "FIND") == 0) {
            sscanf(line, "%*s %s", arg1);
            int i = find_contact(arg1);
            if (i != -1) printf("%s,%s,%s\n", contacts[i].name, contacts[i].email, contacts[i].phone);
        } else if (strcmp(cmd, "REPORT") == 0) {
            for (int i = 0; i < n; i++) printf("%s,%s,%s\n", contacts[i].name, contacts[i].email, contacts[i].phone);
        }
    }
    for (int i = 0; i < n; i++) {
        free(contacts[i].name);
        free(contacts[i].email);
        free(contacts[i].phone);
    }
    free(contacts);
    return 0;
}