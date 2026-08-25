// F016.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *name;
    char *email;
    char *phone;
} Contact;

char *read_line_from_file(FILE *f) {
    size_t cap = 16;
    size_t len = 0;
    char *buf = malloc(cap);
    int c;
    while ((c = fgetc(f)) != EOF && c != '\n') {
        if (len + 1 >= cap) {
            cap *= 2;
            buf = realloc(buf, cap);
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

char *read_line() {
    return read_line_from_file(stdin);
}

int parse_contact(char *line, Contact *c) {
    char *comma1 = strchr(line, ',');
    if (!comma1) return 0;
    char *comma2 = strchr(comma1 + 1, ',');
    if (!comma2) return 0;
    *comma1 = '\0';
    *comma2 = '\0';
    c->name = strdup(line);
    c->email = strdup(comma1 + 1);
    c->phone = strdup(comma2 + 1);
    return 1;
}

int main(int argc, char **argv) {
    if (argc < 2) return 1;
    FILE *f = fopen(argv[1], "r");
    Contact *arr = NULL;
    int count = 0;
    if (f) {
        while (1) {
            char *line = read_line_from_file(f);
            if (!line) break;
            Contact c;
            if (parse_contact(line, &c)) {
                arr = realloc(arr, (count + 1) * sizeof(Contact));
                arr[count] = c;
                count++;
            }
            free(line);
        }
        fclose(f);
    }
    while (1) {
        char *line = read_line();
        if (!line) break;
        if (strcmp(line, "END") == 0) {
            free(line);
            break;
        }
        if (strncmp(line, "ADD ", 4) == 0) {
            Contact c;
            if (parse_contact(line + 4, &c)) {
                arr = realloc(arr, (count + 1) * sizeof(Contact));
                arr[count] = c;
                count++;
            }
        } else if (strncmp(line, "DELETE ", 7) == 0) {
            char *name = line + 7;
            for (int i = 0; i < count; i++) {
                if (strcmp(arr[i].name, name) == 0) {
                    free(arr[i].name);
                    free(arr[i].email);
                    free(arr[i].phone);
                    for (int j = i; j < count - 1; j++) {
                        arr[j] = arr[j + 1];
                    }
                    count--;
                    break;
                }
            }
        } else if (strncmp(line, "FIND ", 5) == 0) {
            char *name = line + 5;
            for (int i = 0; i < count; i++) {
                if (strcmp(arr[i].name, name) == 0) {
                    printf("%s,%s,%s\n", arr[i].name, arr[i].email, arr[i].phone);
                    break;
                }
            }
        } else if (strcmp(line, "REPORT") == 0) {
            for (int i = 0; i < count; i++) {
                printf("%s,%s,%s\n", arr[i].name, arr[i].email, arr[i].phone);
            }
        }
        free(line);
    }
    for (int i = 0; i < count; i++) {
        free(arr[i].name);
        free(arr[i].email);
        free(arr[i].phone);
    }
    free(arr);
    return 0;
}
