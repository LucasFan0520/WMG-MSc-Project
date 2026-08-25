// F016.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *read_line(FILE *fp) {
    size_t size = 32;
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

char *mystrdup(const char *s) {
    size_t len = strlen(s);
    char *d = malloc(len + 1);
    if (!d) return NULL;
    strcpy(d, s);
    return d;
}

typedef struct {
    char *name;
    char *email;
    char *phone;
} CSVContact;

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
    CSVContact *list = NULL;
    size_t count = 0;
    size_t capacity = 0;
    if (argc > 1) {
        FILE *fp = fopen(argv[1], "r");
        if (fp) {
            while (1) {
                char *fline = read_line(fp);
                if (!fline) break;
                char *name = NULL, *email = NULL, *phone = NULL;
                if (parse_csv_line(fline, &name, &email, &phone)) {
                    if (count >= capacity) {
                        capacity = capacity == 0 ? 4 : capacity * 2;
                        CSVContact *new_list = realloc(list, capacity * sizeof(CSVContact));
                        if (new_list) list = new_list;
                    }
                    list[count].name = mystrdup(name);
                    list[count].email = mystrdup(email);
                    list[count].phone = mystrdup(phone);
                    count++;
                }
                free(fline);
            }
            fclose(fp);
        }
    }
    while (1) {
        char *line = read_line(stdin);
        if (!line) break;
        if (strcmp(line, "END") == 0) {
            free(line);
            break;
        }
        char *cmd = line;
        while (*cmd == ' ') cmd++;
        if (*cmd == '\0') {
            free(line);
            continue;
        }
        char *p = cmd;
        while (*p && *p != ' ') p++;
        if (*p != '\0') {
            *p = '\0';
            p++;
        }
        if (strcmp(cmd, "ADD") == 0) {
            while (*p == ' ') p++;
            char *name = NULL, *email = NULL, *phone = NULL;
            if (parse_csv_line(p, &name, &email, &phone)) {
                if (count >= capacity) {
                    capacity = capacity == 0 ? 4 : capacity * 2;
                    CSVContact *new_list = realloc(list, capacity * sizeof(CSVContact));
                    if (new_list) list = new_list;
                }
                list[count].name = mystrdup(name);
                list[count].email = mystrdup(email);
                list[count].phone = mystrdup(phone);
                count++;
            }
        } else if (strcmp(cmd, "DELETE") == 0) {
            while (*p == ' ') p++;
            for (size_t i = 0; i < count; i++) {
                if (strcmp(list[i].name, p) == 0) {
                    free(list[i].name);
                    free(list[i].email);
                    free(list[i].phone);
                    for (size_t j = i; j < count - 1; j++) {
                        list[j] = list[j + 1];
                    }
                    count--;
                    break;
                }
            }
        } else if (strcmp(cmd, "FIND") == 0) {
            while (*p == ' ') p++;
            for (size_t i = 0; i < count; i++) {
                if (strcmp(list[i].name, p) == 0) {
                    printf("%s,%s,%s\n", list[i].name, list[i].email, list[i].phone);
                    break;
                }
            }
        } else if (strcmp(cmd, "REPORT") == 0) {
            for (size_t i = 0; i < count; i++) {
                printf("%s,%s,%s\n", list[i].name, list[i].email, list[i].phone);
            }
        }
        free(line);
    }
    for (size_t i = 0; i < count; i++) {
        free(list[i].name);
        free(list[i].email);
        free(list[i].phone);
    }
    free(list);
    return 0;
}
