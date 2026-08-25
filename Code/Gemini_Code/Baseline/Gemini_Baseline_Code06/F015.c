// F015.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *read_line(FILE *fp) {
    size_t size = 128;
    size_t len = 0;
    char *buf = malloc(size);
    if (!buf) return NULL;
    int c;
    while ((c = fgetc(fp)) != EOF && c != '\n') {
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
    return buf;
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
        if (strcmp(line, "END") == 0) {
            free(line);
            break;
        }
        if (strncmp(line, "ADD ", 4) == 0) {
            char name[128], email[128], phone[128];
            if (sscanf(line + 4, "%127s %127s %127s", name, email, phone) == 3) {
                contacts = realloc(contacts, (count + 1) * sizeof(Contact));
                contacts[count].name = strdup(name);
                contacts[count].email = strdup(email);
                contacts[count].phone = strdup(phone);
                contacts[count].note = strdup("");
                count++;
            }
        } else if (strncmp(line, "NOTE ", 5) == 0) {
            char *p = line + 5;
            char *name = p;
            while (*p && *p != ' ') p++;
            if (*p == ' ') {
                *p = '\0';
                char *text = p + 1;
                for (int i = 0; i < count; i++) {
                    if (strcmp(contacts[i].name, name) == 0) {
                        free(contacts[i].note);
                        contacts[i].note = strdup(text);
                        break;
                    }
                }
            }
        } else if (strncmp(line, "MERGE ", 6) == 0) {
            char keep[128], remove[128];
            if (sscanf(line + 6, "%127s %127s", keep, remove) == 2) {
                int idx_keep = -1;
                int idx_remove = -1;
                for (int i = 0; i < count; i++) {
                    if (strcmp(contacts[i].name, keep) == 0) idx_keep = i;
                    if (strcmp(contacts[i].name, remove) == 0) idx_remove = i;
                }
                if (idx_keep != -1 && idx_remove != -1) {
                    size_t len1 = strlen(contacts[idx_keep].note);
                    size_t len2 = strlen(contacts[idx_remove].note);
                    char *new_note = malloc(len1 + len2 + 2);
                    strcpy(new_note, contacts[idx_keep].note);
                    if (len1 > 0 && len2 > 0) strcat(new_note, " ");
                    strcat(new_note, contacts[idx_remove].note);
                    free(contacts[idx_keep].note);
                    contacts[idx_keep].note = new_note;
                    
                    free(contacts[idx_remove].name);
                    free(contacts[idx_remove].email);
                    free(contacts[idx_remove].phone);
                    free(contacts[idx_remove].note);
                    for (int j = idx_remove; j < count - 1; j++) {
                        contacts[j] = contacts[j + 1];
                    }
                    count--;
                    contacts = realloc(contacts, count * sizeof(Contact));
                }
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
                    contacts = realloc(contacts, count * sizeof(Contact));
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
