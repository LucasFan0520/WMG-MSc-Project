// F015.c
#define _GNU_SOURCE
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

int main(void) {
    char *line = NULL;
    size_t len = 0;
    ssize_t nread;
    while ((nread = getline(&line, &len, stdin)) != -1) {
        size_t l = strlen(line);
        if (l > 0 && line[l - 1] == '\n') {
            line[l - 1] = '\0';
        }
        char *cmd = line;
        char *space1 = strchr(cmd, ' ');
        if (space1) {
            *space1 = '\0';
            char *arg1 = space1 + 1;
            if (strcmp(cmd, "ADD") == 0) {
                char *space2 = strchr(arg1, ' ');
                if (space2) {
                    *space2 = '\0';
                    char *email = space2 + 1;
                    char *space3 = strchr(email, ' ');
                    if (space3) {
                        *space3 = '\0';
                        char *phone = space3 + 1;
                        if (count >= capacity) {
                            capacity = capacity == 0 ? 4 : capacity * 2;
                            Contact *tmp = realloc(contacts, capacity * sizeof(Contact));
                            if (!tmp) {
                                free(line);
                                return 1;
                            }
                            contacts = tmp;
                        }
                        contacts[count].name = strdup(arg1);
                        contacts[count].email = strdup(email);
                        contacts[count].phone = strdup(phone);
                        contacts[count].note = strdup("");
                        if (contacts[count].name && contacts[count].email && contacts[count].phone && contacts[count].note) {
                            count++;
                        }
                    }
                }
            } else if (strcmp(cmd, "NOTE") == 0) {
                char *space2 = strchr(arg1, ' ');
                if (space2) {
                    *space2 = '\0';
                    char *text = space2 + 1;
                    int idx = find_contact(arg1);
                    if (idx != -1) {
                        char *nt = strdup(text);
                        if (nt) {
                            free(contacts[idx].note);
                            contacts[idx].note = nt;
                        }
                    }
                }
            } else if (strcmp(cmd, "MERGE") == 0) {
                char *space2 = strchr(arg1, ' ');
                if (space2) {
                    *space2 = '\0';
                    char *remove_name = space2 + 1;
                    int idx_keep = find_contact(arg1);
                    int idx_rem = find_contact(remove_name);
                    if (idx_keep != -1 && idx_rem != -1 && idx_keep != idx_rem) {
                        size_t nlen = strlen(contacts[idx_keep].note) + strlen(contacts[idx_rem].note) + 2;
                        char *new_note = malloc(nlen);
                        if (new_note) {
                            strcpy(new_note, contacts[idx_keep].note);
                            if (strlen(new_note) > 0 && strlen(contacts[idx_rem].note) > 0) {
                                strcat(new_note, " ");
                            }
                            strcat(new_note, contacts[idx_rem].note);
                            free(contacts[idx_keep].note);
                            contacts[idx_keep].note = new_note;
                        }
                        free(contacts[idx_rem].name);
                        free(contacts[idx_rem].email);
                        free(contacts[idx_rem].phone);
                        free(contacts[idx_rem].note);
                        for (int i = idx_rem; i < count - 1; i++) {
                            contacts[i] = contacts[i + 1];
                        }
                        count--;
                    }
                }
            } else if (strcmp(cmd, "DELETE") == 0) {
                int idx = find_contact(arg1);
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
                int idx = find_contact(arg1);
                if (idx != -1) {
                    printf("%s %s %s %s\n", contacts[idx].name, contacts[idx].email, contacts[idx].phone, contacts[idx].note);
                }
            }
        } else {
            if (strcmp(cmd, "LIST") == 0) {
                for (int i = 0; i < count; i++) {
                    printf("%s %s %s %s\n", contacts[i].name, contacts[i].email, contacts[i].phone, contacts[i].note);
                }
            } else if (strcmp(cmd, "END") == 0) {
                break;
            }
        }
    }
    free(line);
    for (int i = 0; i < count; i++) {
        free(contacts[i].name);
        free(contacts[i].email);
        free(contacts[i].phone);
        free(contacts[i].note);
    }
    free(contacts);
    return 0;
}