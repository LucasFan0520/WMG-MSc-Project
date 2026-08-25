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
int ccount = 0;

int find_contact(const char *name) {
    for (int i = 0; i < ccount; i++) {
        if (strcmp(contacts[i].name, name) == 0) return i;
    }
    return -1;
}

int main() {
    char cmd[20];
    while (scanf("%s", cmd) == 1) {
        if (strcmp(cmd, "END") == 0) break;
        if (strcmp(cmd, "ADD") == 0) {
            char name[100], email[100], phone[100];
            scanf("%s %s %s", name, email, phone);
            contacts = realloc(contacts, sizeof(Contact) * (ccount + 1));
            contacts[ccount].name = strdup(name);
            contacts[ccount].email = strdup(email);
            contacts[ccount].phone = strdup(phone);
            contacts[ccount].note = strdup("");
            ccount++;
        } else if (strcmp(cmd, "NOTE") == 0) {
            char name[100], text[1000];
            scanf("%s", name);
            scanf(" %[^\n]", text);
            int idx = find_contact(name);
            if (idx != -1) {
                free(contacts[idx].note);
                contacts[idx].note = strdup(text);
            }
        } else if (strcmp(cmd, "MERGE") == 0) {
            char keep[100], remove[100];
            scanf("%s %s", keep, remove);
            int ik = find_contact(keep);
            int ir = find_contact(remove);
            if (ik != -1 && ir != -1) {
                char *new_note = malloc(strlen(contacts[ik].note) + strlen(contacts[ir].note) + 2);
                sprintf(new_note, "%s %s", contacts[ik].note, contacts[ir].note);
                free(contacts[ik].note);
                contacts[ik].note = new_note;
                
                free(contacts[ir].name);
                free(contacts[ir].email);
                free(contacts[ir].phone);
                free(contacts[ir].note);
                for (int i = ir; i < ccount - 1; i++) contacts[i] = contacts[i+1];
                ccount--;
            }
        } else if (strcmp(cmd, "DELETE") == 0) {
            char name[100];
            scanf("%s", name);
            int idx = find_contact(name);
            if (idx != -1) {
                free(contacts[idx].name);
                free(contacts[idx].email);
                free(contacts[idx].phone);
                free(contacts[idx].note);
                for (int i = idx; i < ccount - 1; i++) contacts[i] = contacts[i+1];
                ccount--;
            }
        } else if (strcmp(cmd, "FIND") == 0) {
            char name[100];
            scanf("%s", name);
            int idx = find_contact(name);
            if (idx != -1) {
                printf("%s %s %s %s\n", contacts[idx].name, contacts[idx].email, contacts[idx].phone, contacts[idx].note);
            }
        } else if (strcmp(cmd, "LIST") == 0) {
            for (int i = 0; i < ccount; i++) {
                printf("%s %s %s %s\n", contacts[i].name, contacts[i].email, contacts[i].phone, contacts[i].note);
            }
        }
    }
    for (int i = 0; i < ccount; i++) {
        free(contacts[i].name);
        free(contacts[i].email);
        free(contacts[i].phone);
        free(contacts[i].note);
    }
    free(contacts);
    return 0;
}