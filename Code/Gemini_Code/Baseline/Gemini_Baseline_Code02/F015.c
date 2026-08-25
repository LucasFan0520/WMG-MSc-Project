// F015.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *read_line(FILE *f) {
    size_t size = 16;
    size_t len = 0;
    char *str = malloc(size);
    if (!str) return NULL;
    int c;
    while ((c = fgetc(f)) != EOF && c != '\n') {
        if (len + 1 >= size) {
            size *= 2;
            char *new_str = realloc(str, size);
            if (!new_str) { free(str); return NULL; }
            str = new_str;
        }
        str[len++] = c;
    }
    if (len == 0 && c == EOF) { free(str); return NULL; }
    str[len] = '\0';
    return str;
}

typedef struct {
    char *name;
    char *email;
    char *phone;
    char *note;
} Contact;

int main() {
    Contact *contacts = NULL;
    int count = 0;
    while (1) {
        char *line = read_line(stdin);
        if (!line) break;
        if (strcmp(line, "END") == 0) { free(line); break; }
        char *cmd = line;
        char *p1 = strchr(cmd, ' ');
        if (!p1) {
            if (strcmp(cmd, "LIST") == 0) {
                for (int i = 0; i < count; i++) {
                    printf("%s %s %s %s\n", contacts[i].name, contacts[i].email, contacts[i].phone, contacts[i].note ? contacts[i].note : "");
                }
            }
            free(line);
            continue;
        }
        *p1 = '\0';
        char *args = p1 + 1;
        if (strcmp(cmd, "ADD") == 0) {
            char *name = args;
            char *p2 = strchr(name, ' ');
            if (p2) {
                *p2 = '\0';
                char *email = p2 + 1;
                char *p3 = strchr(email, ' ');
                if (p3) {
                    *p3 = '\0';
                    char *phone = p3 + 1;
                    contacts = realloc(contacts, sizeof(Contact) * (count + 1));
                    contacts[count].name = strdup(name);
                    contacts[count].email = strdup(email);
                    contacts[count].phone = strdup(phone);
                    contacts[count].note = strdup("");
                    count++;
                }
            }
        } else if (strcmp(cmd, "NOTE") == 0) {
            char *name = args;
            char *p2 = strchr(name, ' ');
            if (p2) {
                *p2 = '\0';
                char *text = p2 + 1;
                for (int i = 0; i < count; i++) {
                    if (strcmp(contacts[i].name, name) == 0) {
                        free(contacts[i].note);
                        contacts[i].note = strdup(text);
                        break;
                    }
                }
            }
        } else if (strcmp(cmd, "MERGE") == 0) {
            char *keep = args;
            char *p2 = strchr(keep, ' ');
            if (p2) {
                *p2 = '\0';
                char *remove = p2 + 1;
                int k_idx = -1, r_idx = -1;
                for (int i = 0; i < count; i++) {
                    if (strcmp(contacts[i].name, keep) == 0) k_idx = i;
                    if (strcmp(contacts[i].name, remove) == 0) r_idx = i;
                }
                if (k_idx != -1 && r_idx != -1) {
                    size_t nlen = strlen(contacts[k_idx].note) + strlen(contacts[r_idx].note) + 2;
                    char *nnote = malloc(nlen);
                    if (strlen(contacts[k_idx].note) > 0 && strlen(contacts[r_idx].note) > 0) {
                        sprintf(nnote, "%s %s", contacts[k_idx].note, contacts[r_idx].note);
                    } else {
                        sprintf(nnote, "%s%s", contacts[k_idx].note, contacts[r_idx].note);
                    }
                    free(contacts[k_idx].note);
                    contacts[k_idx].note = nnote;
                    free(contacts[r_idx].name);
                    free(contacts[r_idx].email);
                    free(contacts[r_idx].phone);
                    free(contacts[r_idx].note);
                    for (int j = r_idx; j < count - 1; j++) {
                        contacts[j] = contacts[j + 1];
                    }
                    count--;
                    if (count == 0) { free(contacts); contacts = NULL; }
                    else { contacts = realloc(contacts, sizeof(Contact) * count); }
                }
            }
        } else if (strcmp(cmd, "DELETE") == 0) {
            char *name = args;
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
                    if (count == 0) { free(contacts); contacts = NULL; }
                    else { contacts = realloc(contacts, sizeof(Contact) * count); }
                    break;
                }
            }
        } else if (strcmp(cmd, "FIND") == 0) {
            char *name = args;
            for (int i = 0; i < count; i++) {
                if (strcmp(contacts[i].name, name) == 0) {
                    printf("%s %s %s %s\n", contacts[i].name, contacts[i].email, contacts[i].phone, contacts[i].note);
                    break;
                }
            }
        }
        free(line);
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
