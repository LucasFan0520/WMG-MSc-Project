/* F015.c */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *name;
    char *email;
    char *phone;
    char *note;
} Contact;

int main(void) {
    Contact *contacts = NULL;
    int count = 0;
    int capacity = 0;
    char *line = NULL;
    size_t len = 0;
    ssize_t read;

    while ((read = getline(&line, &len, stdin)) != -1) {
        line[strcspn(line, "\r\n")] = '\0';
        if (strcmp(line, "END") == 0) {
            break;
        }
        if (strncmp(line, "ADD ", 4) == 0) {
            char *args = line + 4;
            char *space1 = strchr(args, ' ');
            if (!space1) continue;
            *space1 = '\0';
            char *name = args;
            char *rest = space1 + 1;
            char *space2 = strchr(rest, ' ');
            if (!space2) {
                *space1 = ' ';
                continue;
            }
            *space2 = '\0';
            char *email = rest;
            char *phone = space2 + 1;

            char *n_name = strdup(name);
            char *n_email = strdup(email);
            char *n_phone = strdup(phone);
            char *n_note = strdup("");

            *space1 = ' ';
            *space2 = ' ';

            if (n_name && n_email && n_phone && n_note) {
                if (count >= capacity) {
                    int new_cap = capacity == 0 ? 4 : capacity * 2;
                    Contact *tmp = realloc(contacts, new_cap * sizeof(Contact));
                    if (tmp) {
                        contacts = tmp;
                        capacity = new_cap;
                    } else {
                        free(n_name); free(n_email); free(n_phone); free(n_note);
                        continue;
                    }
                }
                contacts[count].name = n_name;
                contacts[count].email = n_email;
                contacts[count].phone = n_phone;
                contacts[count].note = n_note;
                count++;
            } else {
                free(n_name); free(n_email); free(n_phone); free(n_note);
            }
        } else if (strncmp(line, "NOTE ", 5) == 0) {
            char *args = line + 5;
            char *space = strchr(args, ' ');
            if (space) {
                *space = '\0';
                char *name = args;
                char *text = space + 1;
                for (int i = 0; i < count; i++) {
                    if (strcmp(contacts[i].name, name) == 0) {
                        char *n_note = strdup(text);
                        if (n_note) {
                            free(contacts[i].note);
                            contacts[i].note = n_note;
                        }
                        break;
                    }
                }
                *space = ' ';
            }
        } else if (strncmp(line, "MERGE ", 6) == 0) {
            char *args = line + 6;
            char *space = strchr(args, ' ');
            if (space) {
                *space = '\0';
                char *keep = args;
                char *remove = space + 1;
                int k_idx = -1, r_idx = -1;
                for (int i = 0; i < count; i++) {
                    if (strcmp(contacts[i].name, keep) == 0) k_idx = i;
                    if (strcmp(contacts[i].name, remove) == 0) r_idx = i;
                }
                if (k_idx != -1 && r_idx != -1) {
                    size_t l1 = strlen(contacts[k_idx].note);
                    size_t l2 = strlen(contacts[r_idx].note);
                    char *n_note = malloc(l1 + l2 + 2);
                    if (n_note) {
                        strcpy(n_note, contacts[k_idx].note);
                        if (l1 > 0 && l2 > 0) strcat(n_note, " ");
                        strcat(n_note, contacts[r_idx].note);
                        free(contacts[k_idx].note);
                        contacts[k_idx].note = n_note;
                    }
                    free(contacts[r_idx].name);
                    free(contacts[r_idx].email);
                    free(contacts[r_idx].phone);
                    free(contacts[r_idx].note);
                    for (int j = r_idx; j < count - 1; j++) {
                        contacts[j] = contacts[j + 1];
                    }
                    count--;
                }
                *space = ' ';
            }
        } else if (strncmp(line, "DELETE ", 7) == 0) {
            char *name = line + 7;
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
                    break;
                }
            }
        } else if (strncmp(line, "FIND ", 5) == 0) {
            char *name = line + 5;
            for (int i = 0; i < count; i++) {
                if (strcmp(contacts[i].name, name) == 0) {
                    printf("%s %s %s %s\n", contacts[i].name, contacts[i].email, contacts[i].phone, contacts[i].note);
                    break;
                }
            }
        } else if (strcmp(line, "LIST") == 0) {
            for (int i = 0; i < count; i++) {
                printf("%s %s %s %s\n", contacts[i].name, contacts[i].email, contacts[i].phone, contacts[i].note);
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
