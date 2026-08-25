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

int main() {
    Contact *contacts = NULL;
    int count = 0;
    char line[8192];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\r\n")] = 0;
        if (strcmp(line, "END") == 0) {
            break;
        }
        char cmd[16];
        int read_bytes = 0;
        if (sscanf(line, "%15s%n", cmd, &read_bytes) <= 0) {
            continue;
        }
        char *args = line + read_bytes;
        while (*args == ' ') args++;
        if (strcmp(cmd, "ADD") == 0) {
            char name[256], email[256], phone[256];
            if (sscanf(args, "%255s %255s %255s", name, email, phone) == 3) {
                contacts = realloc(contacts, (count + 1) * sizeof(Contact));
                contacts[count].name = strdup(name);
                contacts[count].email = strdup(email);
                contacts[count].phone = strdup(phone);
                contacts[count].note = strdup("");
                count++;
            }
        } else if (strcmp(cmd, "NOTE") == 0) {
            char name[256];
            int name_bytes = 0;
            if (sscanf(args, "%255s%n", name, &name_bytes) == 1) {
                char *text = args + name_bytes;
                while (*text == ' ') text++;
                for (int i = 0; i < count; i++) {
                    if (strcmp(contacts[i].name, name) == 0) {
                        free(contacts[i].note);
                        contacts[i].note = strdup(text);
                        break;
                    }
                }
            }
        } else if (strcmp(cmd, "MERGE") == 0) {
            char keep[256], remove_name[256];
            if (sscanf(args, "%255s %255s", keep, remove_name) == 2) {
                int idx_keep = -1, idx_rem = -1;
                for (int i = 0; i < count; i++) {
                    if (strcmp(contacts[i].name, keep) == 0) idx_keep = i;
                    if (strcmp(contacts[i].name, remove_name) == 0) idx_rem = i;
                }
                if (idx_keep != -1 && idx_rem != -1) {
                    int len1 = strlen(contacts[idx_keep].note);
                    int len2 = strlen(contacts[idx_rem].note);
                    char *new_note = malloc(len1 + len2 + 2);
                    strcpy(new_note, contacts[idx_keep].note);
                    if (len1 > 0 && len2 > 0) {
                        strcat(new_note, " ");
                    }
                    strcat(new_note, contacts[idx_rem].note);
                    free(contacts[idx_keep].note);
                    contacts[idx_keep].note = new_note;
                    free(contacts[idx_rem].name);
                    free(contacts[idx_rem].email);
                    free(contacts[idx_rem].phone);
                    free(contacts[idx_rem].note);
                    for (int j = idx_rem; j < count - 1; j++) {
                        contacts[j] = contacts[j + 1];
                    }
                    count--;
                    contacts = realloc(contacts, count * sizeof(Contact));
                }
            }
        } else if (strcmp(cmd, "DELETE") == 0) {
            char name[256];
            if (sscanf(args, "%255s", name) == 1) {
                for (int i = 0; i < count; i++) {
                    if (strcmp(contacts[i].name, name) == 0) {
                        free(contacts[i].name);
                        free(contacts[i].email);
                        free(contacts[i].phone);
                        free(contacts[i].note);
                        for (int j = i; j < count - 1; j++) {
                            contacts[j] = contacts[j + 1];
                        }
                        count--;
                        contacts = realloc(contacts, count * sizeof(Contact));
                        break;
                    }
                }
            }
        } else if (strcmp(cmd, "FIND") == 0) {
            char name[256];
            if (sscanf(args, "%255s", name) == 1) {
                for (int i = 0; i < count; i++) {
                    if (strcmp(contacts[i].name, name) == 0) {
                        printf("%s %s %s %s\n", contacts[i].name, contacts[i].email, contacts[i].phone, contacts[i].note);
                        break;
                    }
                }
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
