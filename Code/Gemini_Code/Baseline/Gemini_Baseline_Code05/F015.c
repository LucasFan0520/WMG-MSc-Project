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
int capacity = 0;

int find_contact(const char *name) {
    for (int i = 0; i < count; i++) {
        if (strcmp(contacts[i].name, name) == 0) {
            return i;
        }
    }
    return -1;
}

int main() {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\r\n")] = 0;
        if (strcmp(line, "END") == 0) {
            break;
        }
        if (strncmp(line, "ADD ", 4) == 0) {
            char *p1 = line + 4;
            char *p2 = strchr(p1, ' ');
            if (p2) {
                *p2 = 0;
                char *p3 = strchr(p2 + 1, ' ');
                if (p3) {
                    *p3 = 0;
                    if (count >= capacity) {
                        capacity = capacity == 0 ? 4 : capacity * 2;
                        contacts = realloc(contacts, capacity * sizeof(Contact));
                    }
                    contacts[count].name = strdup(p1);
                    contacts[count].email = strdup(p2 + 1);
                    contacts[count].phone = strdup(p3 + 1);
                    contacts[count].note = strdup("");
                    count++;
                }
            }
        } else if (strncmp(line, "NOTE ", 5) == 0) {
            char *p1 = line + 5;
            char *p2 = strchr(p1, ' ');
            if (p2) {
                *p2 = 0;
                int idx = find_contact(p1);
                if (idx != -1) {
                    free(contacts[idx].note);
                    contacts[idx].note = strdup(p2 + 1);
                }
            }
        } else if (strncmp(line, "MERGE ", 6) == 0) {
            char *p1 = line + 6;
            char *p2 = strchr(p1, ' ');
            if (p2) {
                *p2 = 0;
                int idxKeep = find_contact(p1);
                int idxRemove = find_contact(p2 + 1);
                if (idxKeep != -1 && idxRemove != -1) {
                    char *n1 = contacts[idxKeep].note;
                    char *n2 = contacts[idxRemove].note;
                    int new_len = strlen(n1) + strlen(n2) + 2;
                    char *new_note = malloc(new_len);
                    if (strlen(n1) > 0 && strlen(n2) > 0) {
                        sprintf(new_note, "%s %s", n1, n2);
                    } else if (strlen(n1) > 0) {
                        strcpy(new_note, n1);
                    } else {
                        strcpy(new_note, n2);
                    }
                    free(contacts[idxKeep].note);
                    contacts[idxKeep].note = new_note;

                    free(contacts[idxRemove].name);
                    free(contacts[idxRemove].email);
                    free(contacts[idxRemove].phone);
                    free(contacts[idxRemove].note);
                    for (int i = idxRemove; i < count - 1; i++) {
                        contacts[i] = contacts[i + 1];
                    }
                    count--;
                }
            }
        } else if (strncmp(line, "DELETE ", 7) == 0) {
            char *name = line + 7;
            int idx = find_contact(name);
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
        } else if (strncmp(line, "FIND ", 5) == 0) {
            char *name = line + 5;
            int idx = find_contact(name);
            if (idx != -1) {
                printf("%s %s %s %s\n", contacts[idx].name, contacts[idx].email, contacts[idx].phone, contacts[idx].note);
            }
        } else if (strcmp(line, "LIST") == 0) {
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
