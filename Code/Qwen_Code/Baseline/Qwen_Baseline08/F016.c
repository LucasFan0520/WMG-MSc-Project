// F016.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_CONTACTS 1000

typedef struct {
    char name[100];
    char email[100];
    char phone[100];
} Contact;

Contact contacts[MAX_CONTACTS];
int count = 0;

int find_contact(const char *name) {
    for (int i = 0; i < count; i++) if (strcmp(contacts[i].name, name) == 0) return i;
    return -1;
}

void add_contact(char *line) {
    char *tok1 = strtok(line, ",");
    char *tok2 = strtok(NULL, ",");
    char *tok3 = strtok(NULL, ",\n");
    if (tok1 && tok2 && tok3) {
        if (find_contact(tok1) == -1 && count < MAX_CONTACTS) {
            strcpy(contacts[count].name, tok1);
            strcpy(contacts[count].email, tok2);
            strcpy(contacts[count].phone, tok3);
            count++;
        }
    }
}

int main(int argc, char *argv[]) {
    if (argc > 1) {
        FILE *f = fopen(argv[1], "r");
        if (f) {
            char line[300];
            while (fgets(line, sizeof(line), f)) add_contact(line);
            fclose(f);
        }
    }
    char cmd[20], name[100], line[300];
    while (scanf("%s", cmd) == 1 && strcmp(cmd, "END") != 0) {
        if (strcmp(cmd, "ADD") == 0) {
            scanf("%s", line);
            add_contact(line);
        } else if (strcmp(cmd, "DELETE") == 0) {
            scanf("%s", name);
            int idx = find_contact(name);
            if (idx != -1) {
                for (int i = idx; i < count - 1; i++) contacts[i] = contacts[i + 1];
                count--;
            }
        } else if (strcmp(cmd, "FIND") == 0) {
            scanf("%s", name);
            int idx = find_contact(name);
            if (idx != -1) printf("%s,%s,%s\n", contacts[idx].name, contacts[idx].email, contacts[idx].phone);
        } else if (strcmp(cmd, "REPORT") == 0) {
            for (int i = 0; i < count; i++) printf("%s,%s,%s\n", contacts[i].name, contacts[i].email, contacts[i].phone);
        }
    }
    return 0;
}