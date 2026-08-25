// F015.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_CONTACTS 1000
#define MAX_LEN 1000

typedef struct {
    char name[100];
    char email[100];
    char phone[100];
    char note[MAX_LEN];
} Contact;

Contact contacts[MAX_CONTACTS];
int count = 0;

int find_contact(const char *name) {
    for (int i = 0; i < count; i++) if (strcmp(contacts[i].name, name) == 0) return i;
    return -1;
}

int main() {
    char cmd[20], n1[100], n2[100], text[MAX_LEN];
    while (scanf("%s", cmd) == 1 && strcmp(cmd, "END") != 0) {
        if (strcmp(cmd, "ADD") == 0) {
            scanf("%s %s %s", n1, contacts[count].email, contacts[count].phone);
            contacts[count].note[0] = 0;
            strcpy(contacts[count].name, n1);
            if (count < MAX_CONTACTS) count++;
        } else if (strcmp(cmd, "NOTE") == 0) {
            scanf("%s", n1);
            fgets(text, MAX_LEN, stdin);
            text[strcspn(text, "\n")] = 0;
            if (text[0] == ' ') memmove(text, text + 1, strlen(text));
            int idx = find_contact(n1);
            if (idx != -1) strcpy(contacts[idx].note, text);
        } else if (strcmp(cmd, "MERGE") == 0) {
            scanf("%s %s", n1, n2);
            int i1 = find_contact(n1), i2 = find_contact(n2);
            if (i1 != -1 && i2 != -1) {
                if (contacts[i1].note[0] && contacts[i2].note[0]) {
                    strcat(contacts[i1].note, " ");
                    strcat(contacts[i1].note, contacts[i2].note);
                } else if (contacts[i2].note[0]) {
                    strcpy(contacts[i1].note, contacts[i2].note);
                }
                for (int i = i2; i < count - 1; i++) contacts[i] = contacts[i + 1];
                count--;
            }
        } else if (strcmp(cmd, "DELETE") == 0) {
            scanf("%s", n1);
            int idx = find_contact(n1);
            if (idx != -1) {
                for (int i = idx; i < count - 1; i++) contacts[i] = contacts[i + 1];
                count--;
            }
        } else if (strcmp(cmd, "FIND") == 0) {
            scanf("%s", n1);
            int idx = find_contact(n1);
            if (idx != -1) printf("%s %s %s %s\n", contacts[idx].name, contacts[idx].email, contacts[idx].phone, contacts[idx].note);
        } else if (strcmp(cmd, "LIST") == 0) {
            for (int i = 0; i < count; i++) printf("%s %s %s %s\n", contacts[i].name, contacts[i].email, contacts[i].phone, contacts[i].note);
        }
    }
    return 0;
}