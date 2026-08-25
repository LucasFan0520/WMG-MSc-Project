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
int count = 0;

int find_name(const char *name) {
    for (int i = 0; i < count; i++) {
        if (strcmp(contacts[i].name, name) == 0) {
            return i;
        }
    }
    return -1;
}

int main() {
    char cmd[20];
    char n1[1000];
    char n2[1000];
    char email[1000];
    char phone[1000];
    char note[10000];

    while (scanf("%s", cmd) == 1 && strcmp(cmd, "END") != 0) {
        if (strcmp(cmd, "ADD") == 0) {
            scanf("%s %s %s", n1, email, phone);
            contacts = realloc(contacts, sizeof(Contact) * (count + 1));
            contacts[count].name = strdup(n1);
            contacts[count].email = strdup(email);
            contacts[count].phone = strdup(phone);
            contacts[count].note = strdup("");
            count++;
        } else if (strcmp(cmd, "NOTE") == 0) {
            scanf("%s", n1);
            fgets(note, sizeof(note), stdin);
            if (note[0] == ' ') {
                memmove(note, note + 1, strlen(note));
            }
            note[strcspn(note, "\n")] = 0;
            int idx = find_name(n1);
            if (idx != -1) {
                free(contacts[idx].note);
                contacts[idx].note = strdup(note);
            }
        } else if (strcmp(cmd, "MERGE") == 0) {
            scanf("%s %s", n1, n2);
            int i1 = find_name(n1);
            int i2 = find_name(n2);
            if (i1 != -1 && i2 != -1) {
                char *nn = malloc(strlen(contacts[i1].note) + strlen(contacts[i2].note) + 2);
                sprintf(nn, "%s %s", contacts[i1].note, contacts[i2].note);
                free(contacts[i1].note);
                contacts[i1].note = nn;
                free(contacts[i2].name);
                free(contacts[i2].email);
                free(contacts[i2].phone);
                free(contacts[i2].note);
                for (int i = i2; i < count - 1; i++) {
                    contacts[i] = contacts[i + 1];
                }
                count--;
            }
        } else if (strcmp(cmd, "DELETE") == 0) {
            scanf("%s", n1);
            int idx = find_name(n1);
            if (idx != -1) {
                free(contacts[idx].name);
                free(contacts[idx].email);
                free(contacts[idx].phone);
                free(contacts[idx].note);
                for (int i = idx; i < count - 1; i++) {
                    contacts[i] = contacts[i + 1];
                }
                count--;
            }
        } else if (strcmp(cmd, "FIND") == 0) {
            scanf("%s", n1);
            int idx = find_name(n1);
            if (idx != -1) {
                printf("%s %s %s %s\n", contacts[idx].name, contacts[idx].email, contacts[idx].phone, contacts[idx].note);
            }
        } else if (strcmp(cmd, "LIST") == 0) {
            for (int i = 0; i < count; i++) {
                printf("%s %s %s %s\n", contacts[i].name, contacts[i].email, contacts[i].phone, contacts[i].note);
            }
        }
    }

    for (int i = 0; i < count; i++) {
        free(contacts[i].name);
        free(contacts[i].email);
        free(contacts[i].phone);
        free(contacts[i].note);
    }
    free(contacts);

    return 0;
}