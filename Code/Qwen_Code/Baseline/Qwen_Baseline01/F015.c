// F015.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *name;
    char *email;
    char *phone;
    char *note;
} Contact;

Contact *contacts = NULL;
int n = 0;

int find_contact(const char *name) {
    for (int i = 0; i < n; i++)
        if (strcmp(contacts[i].name, name) == 0) return i;
    return -1;
}

int main() {
    char line[10000];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        if (strcmp(line, "END") == 0) break;
        char cmd[20], arg1[1000], arg2[1000], arg3[1000];
        if (sscanf(line, "%s", cmd) != 1) continue;
        if (strcmp(cmd, "ADD") == 0) {
            sscanf(line, "%*s %s %s %s", arg1, arg2, arg3);
            contacts = realloc(contacts, sizeof(Contact) * (n + 1));
            contacts[n].name = strdup(arg1);
            contacts[n].email = strdup(arg2);
            contacts[n].phone = strdup(arg3);
            contacts[n].note = strdup("");
            n++;
        } else if (strcmp(cmd, "NOTE") == 0) {
            sscanf(line, "%*s %s", arg1);
            char *p = line + 5 + strlen(arg1);
            if (*p == ' ') p++;
            int i = find_contact(arg1);
            if (i != -1) {
                free(contacts[i].note);
                contacts[i].note = strdup(p);
            }
        } else if (strcmp(cmd, "MERGE") == 0) {
            sscanf(line, "%*s %s %s", arg1, arg2);
            int i1 = find_contact(arg1), i2 = find_contact(arg2);
            if (i1 != -1 && i2 != -1) {
                char *new_note = malloc(strlen(contacts[i1].note) + strlen(contacts[i2].note) + 2);
                sprintf(new_note, "%s %s", contacts[i1].note, contacts[i2].note);
                free(contacts[i1].note);
                contacts[i1].note = new_note;
                free(contacts[i2].name);
                free(contacts[i2].email);
                free(contacts[i2].phone);
                free(contacts[i2].note);
                for (int j = i2; j < n - 1; j++) contacts[j] = contacts[j + 1];
                n--;
            }
        } else if (strcmp(cmd, "DELETE") == 0) {
            sscanf(line, "%*s %s", arg1);
            int i = find_contact(arg1);
            if (i != -1) {
                free(contacts[i].name);
                free(contacts[i].email);
                free(contacts[i].phone);
                free(contacts[i].note);
                for (int j = i; j < n - 1; j++) contacts[j] = contacts[j + 1];
                n--;
            }
        } else if (strcmp(cmd, "FIND") == 0) {
            sscanf(line, "%*s %s", arg1);
            int i = find_contact(arg1);
            if (i != -1) printf("%s %s %s %s\n", contacts[i].name, contacts[i].email, contacts[i].phone, contacts[i].note);
        } else if (strcmp(cmd, "LIST") == 0) {
            for (int i = 0; i < n; i++) printf("%s %s %s %s\n", contacts[i].name, contacts[i].email, contacts[i].phone, contacts[i].note);
        }
    }
    for (int i = 0; i < n; i++) {
        free(contacts[i].name);
        free(contacts[i].email);
        free(contacts[i].phone);
        free(contacts[i].note);
    }
    free(contacts);
    return 0;
}