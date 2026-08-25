// F015.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *read_line(FILE *stream) {
    size_t capacity = 16;
    size_t len = 0;
    char *str = malloc(capacity);
    if (!str) return NULL;
    int c;
    while ((c = fgetc(stream)) != EOF && c != '\n') {
        if (len + 1 >= capacity) {
            capacity *= 2;
            char *new_str = realloc(str, capacity);
            if (!new_str) {
                free(str);
                return NULL;
            }
            str = new_str;
        }
        str[len++] = (char)c;
    }
    if (len == 0 && c == EOF) {
        free(str);
        return NULL;
    }
    if (len > 0 && str[len - 1] == '\n') {
        len--;
    }
    str[len] = '\0';
    return str;
}

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
    while (1) {
        char *line = read_line(stdin);
        if (!line) break;
        if (strcmp(line, "END") == 0) {
            free(line);
            break;
        }
        if (strncmp(line, "ADD ", 4) == 0) {
            char *p = line + 4;
            while (*p == ' ') p++;
            char *name_start = p;
            while (*p && *p != ' ') p++;
            if (*p == ' ') {
                *p = '\0';
                p++;
                while (*p == ' ') p++;
                char *email_start = p;
                while (*p && *p != ' ') p++;
                if (*p == ' ') {
                    *p = '\0';
                    p++;
                    while (*p == ' ') p++;
                    char *phone_start = p;
                    if (count >= capacity) {
                        capacity = capacity == 0 ? 4 : capacity * 2;
                        Contact *new_c = realloc(contacts, capacity * sizeof(Contact));
                        if (new_c) contacts = new_c;
                    }
                    if (count < capacity) {
                        contacts[count].name = strdup(name_start);
                        contacts[count].email = strdup(email_start);
                        contacts[count].phone = strdup(phone_start);
                        contacts[count].note = strdup("");
                        count++;
                    }
                }
            }
        } else if (strncmp(line, "NOTE ", 5) == 0) {
            char *p = line + 5;
            while (*p == ' ') p++;
            char *name_start = p;
            while (*p && *p != ' ') p++;
            if (*p == ' ') {
                *p = '\0';
                char *text_start = p + 1;
                while (*text_start == ' ') text_start++;
                for (int i = 0; i < count; i++) {
                    if (strcmp(contacts[i].name, name_start) == 0) {
                        free(contacts[i].note);
                        contacts[i].note = strdup(text_start);
                        break;
                    }
                }
            }
        } else if (strncmp(line, "MERGE ", 6) == 0) {
            char *p = line + 6;
            while (*p == ' ') p++;
            char *keep_start = p;
            while (*p && *p != ' ') p++;
            if (*p == ' ') {
                *p = '\0';
                char *rem_start = p + 1;
                while (*rem_start == ' ') rem_start++;
                int idxK = -1, idxR = -1;
                for (int i = 0; i < count; i++) {
                    if (strcmp(contacts[i].name, keep_start) == 0) idxK = i;
                    if (strcmp(contacts[i].name, rem_start) == 0) idxR = i;
                }
                if (idxK != -1 && idxR != -1) {
                    size_t len1 = strlen(contacts[idxK].note);
                    size_t len2 = strlen(contacts[idxR].note);
                    char *new_note = malloc(len1 + len2 + 2);
                    if (new_note) {
                        strcpy(new_note, contacts[idxK].note);
                        if (len1 > 0 && len2 > 0) {
                            strcat(new_note, " ");
                            strcat(new_note, contacts[idxR].note);
                        } else if (len2 > 0) {
                            strcpy(new_note, contacts[idxR].note);
                        }
                        free(contacts[idxK].note);
                        contacts[idxK].note = new_note;
                    }
                    free(contacts[idxR].name);
                    free(contacts[idxR].email);
                    free(contacts[idxR].phone);
                    free(contacts[idxR].note);
                    for (int j = idxR; j < count - 1; j++) {
                        contacts[j] = contacts[j + 1];
                    }
                    count--;
                }
            }
        } else if (strncmp(line, "DELETE ", 7) == 0) {
            char *name_start = line + 7;
            while (*name_start == ' ') name_start++;
            for (int i = 0; i < count; i++) {
                if (strcmp(contacts[i].name, name_start) == 0) {
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
            char *name_start = line + 5;
            while (*name_start == ' ') name_start++;
            for (int i = 0; i < count; i++) {
                if (strcmp(contacts[i].name, name_start) == 0) {
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
