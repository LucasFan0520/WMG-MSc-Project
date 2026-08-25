// F016.c
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
} Contact16;

int add_contact(Contact16 **contacts, int *count, int *capacity, const char *csv_line) {
    const char *c1 = strchr(csv_line, ',');
    if (!c1) return 0;
    const char *c2 = strchr(c1 + 1, ',');
    if (!c2) return 0;
    size_t n_len = c1 - csv_line;
    size_t e_len = c2 - (c1 + 1);
    size_t p_len = strlen(c2 + 1);
    if (n_len == 0 || e_len == 0 || p_len == 0) return 0;
    if (*count >= *capacity) {
        *capacity = *capacity == 0 ? 4 : *capacity * 2;
        Contact16 *new_c = realloc(*contacts, *capacity * sizeof(Contact16));
        if (!new_c) return 0;
        *contacts = new_c;
    }
    Contact16 *c = &((*contacts)[*count]);
    c->name = malloc(n_len + 1);
    c->email = malloc(e_len + 1);
    c->phone = malloc(p_len + 1);
    if (c->name && c->email && c->phone) {
        memcpy(c->name, csv_line, n_len); c->name[n_len] = '\0';
        memcpy(c->email, c1 + 1, e_len); c->email[e_len] = '\0';
        strcpy(c->phone, c2 + 1);
        (*count)++;
        return 1;
    }
    return 0;
}

int main(int argc, char **argv) {
    if (argc < 2) return 1;
    FILE *fp = fopen(argv[1], "r");
    if (!fp) return 1;
    Contact16 *contacts = NULL;
    int count = 0;
    int capacity = 0;
    while (1) {
        char *line = read_line(fp);
        if (!line) break;
        add_contact(&contacts, &count, &capacity, line);
        free(line);
    }
    fclose(fp);
    while (1) {
        char *line = read_line(stdin);
        if (!line) break;
        if (strcmp(line, "END") == 0) {
            free(line);
            break;
        }
        if (strncmp(line, "ADD ", 4) == 0) {
            add_contact(&contacts, &count, &capacity, line + 4);
        } else if (strncmp(line, "DELETE ", 7) == 0) {
            char *name_start = line + 7;
            while (*name_start == ' ') name_start++;
            for (int i = 0; i < count; i++) {
                if (strcmp(contacts[i].name, name_start) == 0) {
                    free(contacts[i].name);
                    free(contacts[i].email);
                    free(contacts[i].phone);
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
                    printf("%s,%s,%s\n", contacts[i].name, contacts[i].email, contacts[i].phone);
                    break;
                }
            }
        } else if (strcmp(line, "REPORT") == 0) {
            for (int i = 0; i < count; i++) {
                printf("%s,%s,%s\n", contacts[i].name, contacts[i].email, contacts[i].phone);
            }
        }
        free(line);
    }
    for (int i = 0; i < count; i++) {
        free(contacts[i].name);
        free(contacts[i].email);
        free(contacts[i].phone);
    }
    free(contacts);
    return 0;
}
