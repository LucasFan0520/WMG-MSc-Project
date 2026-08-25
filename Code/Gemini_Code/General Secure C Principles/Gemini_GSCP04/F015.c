// F015.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *read_line(FILE *stream) {
    size_t size = 16;
    size_t len = 0;
    char *buf = malloc(size);
    if (!buf) return NULL;
    int c;
    while ((c = fgetc(stream)) != EOF && c != '\n') {
        if (len + 1 >= size) {
            size *= 2;
            char *new_buf = realloc(buf, size);
            if (!new_buf) {
                free(buf);
                return NULL;
            }
            buf = new_buf;
        }
        buf[len++] = c;
    }
    if (len == 0 && c == EOF) {
        free(buf);
        return NULL;
    }
    buf[len] = '\0';
    if (len > 0 && buf[len - 1] == '\r') {
        buf[len - 1] = '\0';
    }
    return buf;
}

typedef struct {
    char *name;
    char *email;
    char *phone;
    char *note;
} Contact;

int main(void) {
    Contact *contacts = NULL;
    size_t count = 0;
    size_t capacity = 0;
    while (1) {
        char *line = read_line(stdin);
        if (!line) break;
        char *p = line;
        while (*p == ' ' || *p == '\t') p++;
        if (*p == '\0') {
            free(line);
            continue;
        }
        char *cmd = p;
        while (*p && *p != ' ' && *p != '\t') p++;
        if (*p) {
            *p = '\0';
            p++;
        }
        if (strcmp(cmd, "END") == 0) {
            free(line);
            break;
        }
        if (strcmp(cmd, "ADD") == 0) {
            while (*p == ' ' || *p == '\t') p++;
            char *name = p;
            while (*p && *p != ' ' && *p != '\t') p++;
            if (*p) {
                *p = '\0';
                p++;
            }
            while (*p == ' ' || *p == '\t') p++;
            char *email = p;
            while (*p && *p != ' ' && *p != '\t') p++;
            if (*p) {
                *p = '\0';
                p++;
            }
            while (*p == ' ' || *p == '\t') p++;
            char *phone = p;
            while (*p && *p != ' ' && *p != '\t') p++;
            if (*p) *p = '\0';
            int found = 0;
            for (size_t i = 0; i < count; i++) {
                if (strcmp(contacts[i].name, name) == 0) {
                    found = 1;
                    break;
                }
            }
            if (!found && *name != '\0') {
                if (count >= capacity) {
                    capacity = capacity == 0 ? 4 : capacity * 2;
                    Contact *new_contacts = realloc(contacts, capacity * sizeof(Contact));
                    if (new_contacts) contacts = new_contacts;
                }
                if (count < capacity) {
                    contacts[count].name = strdup(name);
                    contacts[count].email = strdup(email);
                    contacts[count].phone = strdup(phone);
                    contacts[count].note = strdup("");
                    count++;
                }
            }
        } else if (strcmp(cmd, "NOTE") == 0) {
            while (*p == ' ' || *p == '\t') p++;
            char *name = p;
            while (*p && *p != ' ' && *p != '\t') p++;
            if (*p) {
                *p = '\0';
                p++;
            }
            while (*p == ' ' || *p == '\t') p++;
            char *note = p;
            for (size_t i = 0; i < count; i++) {
                if (strcmp(contacts[i].name, name) == 0) {
                    free(contacts[i].note);
                    contacts[i].note = strdup(note);
                    break;
                }
            }
        } else if (strcmp(cmd, "MERGE") == 0) {
            while (*p == ' ' || *p == '\t') p++;
            char *keep = p;
            while (*p && *p != ' ' && *p != '\t') p++;
            if (*p) {
                *p = '\0';
                p++;
            }
            while (*p == ' ' || *p == '\t') p++;
            char *remove = p;
            while (*p && *p != ' ' && *p != '\t') p++;
            if (*p) *p = '\0';
            long long keep_idx = -1;
            long long rem_idx = -1;
            for (size_t i = 0; i < count; i++) {
                if (strcmp(contacts[i].name, keep) == 0) keep_idx = (long long)i;
                if (strcmp(contacts[i].name, remove) == 0) rem_idx = (long long)i;
            }
            if (keep_idx != -1 && rem_idx != -1 && keep_idx != rem_idx) {
                char *k_note = contacts[keep_idx].note;
                char *r_note = contacts[rem_idx].note;
                size_t n_len = strlen(k_note) + strlen(r_note) + 2;
                char *cmb = malloc(n_len);
                if (cmb) {
                    if (strlen(k_note) > 0 && strlen(r_note) > 0) {
                        sprintf(cmb, "%s %s", k_note, r_note);
                    } else if (strlen(k_note) > 0) {
                        strcpy(cmb, k_note);
                    } else {
                        strcpy(cmb, r_note);
                    }
                    free(contacts[keep_idx].note);
                    contacts[keep_idx].note = cmb;
                }
                free(contacts[rem_idx].name);
                free(contacts[rem_idx].email);
                free(contacts[rem_idx].phone);
                free(contacts[rem_idx].note);
                for (size_t j = (size_t)rem_idx; j < count - 1; j++) {
                    contacts[j] = contacts[j + 1];
                }
                count--;
            }
        } else if (strcmp(cmd, "DELETE") == 0) {
            while (*p == ' ' || *p == '\t') p++;
            char *name = p;
            while (*p && *p != ' ' && *p != '\t') p++;
            if (*p) *p = '\0';
            for (size_t i = 0; i < count; i++) {
                if (strcmp(contacts[i].name, name) == 0) {
                    free(contacts[i].name);
                    free(contacts[i].email);
                    free(contacts[i].phone);
                    free(contacts[i].note);
                    for (size_t j = i; j < count - 1; j++) {
                        contacts[j] = contacts[j + 1];
                    }
                    count--;
                    break;
                }
            }
        } else if (strcmp(cmd, "FIND") == 0) {
            while (*p == ' ' || *p == '\t') p++;
            char *name = p;
            while (*p && *p != ' ' && *p != '\t') p++;
            if (*p) *p = '\0';
            for (size_t i = 0; i < count; i++) {
                if (strcmp(contacts[i].name, name) == 0) {
                    printf("%s %s %s %s\n", contacts[i].name, contacts[i].email, contacts[i].phone, contacts[i].note);
                    break;
                }
            }
        } else if (strcmp(cmd, "LIST") == 0) {
            for (size_t i = 0; i < count; i++) {
                printf("%s %s %s %s\n", contacts[i].name, contacts[i].email, contacts[i].phone, contacts[i].note);
            }
        }
        free(line);
    }
    for (size_t i = 0; i < count; i++) {
        free(contacts[i].name);
        free(contacts[i].email);
        free(contacts[i].phone);
        free(contacts[i].note);
    }
    free(contacts);
    return 0;
}
