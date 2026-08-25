// F016.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *read_line(FILE *f) {
    int cap = 16;
    int len = 0;
    char *buf = malloc(cap);
    if (!buf) return NULL;
    int c;
    while ((c = fgetc(f)) != EOF && c != '\n') {
        if (len + 1 >= cap) {
            cap *= 2;
            char *nb = realloc(buf, cap);
            if (!nb) { free(buf); return NULL; }
            buf = nb;
        }
        buf[len++] = c;
    }
    if (len == 0 && c == EOF) { free(buf); return NULL; }
    buf[len] = '\0';
    return buf;
}

char *get_token(char **p) {
    while (**p == ' ') (*p)++;
    if (**p == '\0') return NULL;
    char *start = *p;
    while (**p && **p != ' ') (*p)++;
    if (**p) {
        **p = '\0';
        (*p)++;
    }
    return start;
}

char *get_rest(char **p) {
    while (**p == ' ') (*p)++;
    return **p ? *p : "";
}

typedef struct {
    char *name;
    char *email;
    char *phone;
} Contact;

int parse_csv_line(char *line, char **name, char **email, char **phone) {
    char *c1 = strchr(line, ',');
    if (!c1) return 0;
    char *c2 = strchr(c1 + 1, ',');
    if (!c2) return 0;
    *c1 = '\0';
    *c2 = '\0';
    *name = line;
    *email = c1 + 1;
    *phone = c2 + 1;
    return 1;
}

int main(int argc, char **argv) {
    if (argc < 2) return 1;
    Contact *arr = NULL;
    int count = 0;
    int cap = 0;
    FILE *f = fopen(argv[1], "r");
    if (f) {
        while (1) {
            char *line = read_line(f);
            if (!line) break;
            char *name, *email, *phone;
            if (parse_csv_line(line, &name, &email, &phone)) {
                if (count >= cap) {
                    cap = cap == 0 ? 4 : cap * 2;
                    Contact *narr = realloc(arr, cap * sizeof(Contact));
                    if (narr) arr = narr;
                }
                arr[count].name = strdup(name);
                arr[count].email = strdup(email);
                arr[count].phone = strdup(phone);
                count++;
            }
            free(line);
        }
        fclose(f);
    }
    while (1) {
        char *line = read_line(stdin);
        if (!line) break;
        char *p = line;
        char *cmd = get_token(&p);
        if (!cmd || strcmp(cmd, "END") == 0) {
            free(line);
            break;
        }
        if (strcmp(cmd, "ADD") == 0) {
            char *rest = get_rest(&p);
            char *name, *email, *phone;
            if (parse_csv_line(rest, &name, &email, &phone)) {
                if (count >= cap) {
                    cap = cap == 0 ? 4 : cap * 2;
                    Contact *narr = realloc(arr, cap * sizeof(Contact));
                    if (narr) arr = narr;
                }
                arr[count].name = strdup(name);
                arr[count].email = strdup(email);
                arr[count].phone = strdup(phone);
                count++;
            }
        } else if (strcmp(cmd, "DELETE") == 0) {
            char *name = get_rest(&p);
            if (name) {
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
            }
        } else if (strcmp(cmd, "FIND") == 0) {
            char *name = get_rest(&p);
            if (name) {
                for (int i = 0; i < count; i++) {
                    if (strcmp(arr[i].name, name) == 0) {
                        printf("%s,%s,%s\n", arr[i].name, arr[i].email, arr[i].phone);
                        break;
                    }
                }
            }
        } else if (strcmp(cmd, "REPORT") == 0) {
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
