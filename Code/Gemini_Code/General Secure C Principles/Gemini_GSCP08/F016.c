// F016.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *read_line(FILE *fp) {
    size_t cap = 128;
    size_t len = 0;
    char *buf = malloc(cap);
    if (!buf) return NULL;
    int c;
    while ((c = fgetc(fp)) != EOF && c != '\n') {
        if (len + 1 >= cap) {
            size_t ncap = cap * 2;
            char *nb = realloc(buf, ncap);
            if (!nb) {
                free(buf);
                return NULL;
            }
            buf = nb;
            cap = ncap;
        }
        buf[len++] = c;
    }
    if (c == EOF && len == 0) {
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
    size_t pl = strlen(*phone);
    while (pl > 0 && ((*phone)[pl - 1] == '\n' || (*phone)[pl - 1] == '\r')) {
        (*phone)[pl - 1] = '\0';
        pl--;
    }
    return 1;
}

int main(int argc, char **argv) {
    CSVContact *arr = NULL;
    size_t count = 0;
    size_t cap = 0;
    if (argc > 1) {
        FILE *fp = fopen(argv[1], "r");
        if (fp) {
            while (1) {
                char *fline = read_line(fp);
                if (!fline) break;
                char *name = NULL, *email = NULL, *phone = NULL;
                if (parse_csv_line(fline, &name, &email, &phone)) {
                    if (count >= cap) {
                        size_t ncap = cap == 0 ? 4 : cap * 2;
                        CSVContact *narr = realloc(arr, ncap * sizeof(CSVContact));
                        if (narr) {
                            arr = narr;
                            cap = ncap;
                        }
                    }
                    if (count < cap) {
                        arr[count].name = strdup(name);
                        arr[count].email = strdup(email);
                        arr[count].phone = strdup(phone);
                        count++;
                    }
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
        char *p1 = strchr(cmd, ' ');
        if (!p1) {
            if (strcmp(cmd, "REPORT") == 0) {
                for (size_t i = 0; i < count; i++) {
                    printf("%s,%s,%s\n", arr[i].name, arr[i].email, arr[i].phone);
                }
            }
            free(line);
            continue;
        }
        *p1 = '\0';
        char *args = p1 + 1;
        if (strcmp(cmd, "ADD") == 0) {
            char *name = NULL, *email = NULL, *phone = NULL;
            if (parse_csv_line(args, &name, &email, &phone)) {
                if (count >= cap) {
                    size_t ncap = cap == 0 ? 4 : cap * 2;
                    CSVContact *narr = realloc(arr, ncap * sizeof(CSVContact));
                    if (narr) {
                        arr = narr;
                        cap = ncap;
                    }
                }
                if (count < cap) {
                    arr[count].name = strdup(name);
                    arr[count].email = strdup(email);
                    arr[count].phone = strdup(phone);
                    count++;
                }
            }
        } else if (strcmp(cmd, "DELETE") == 0) {
            char *name = args;
            for (size_t i = 0; i < count; i++) {
                if (strcmp(arr[i].name, name) == 0) {
                    free(arr[i].name);
                    free(arr[i].email);
                    free(arr[i].phone);
                    for (size_t j = i; j < count - 1; j++) {
                        arr[j] = arr[j + 1];
                    }
                    count--;
                    break;
                }
            }
        } else if (strcmp(cmd, "FIND") == 0) {
            char *name = args;
            for (size_t i = 0; i < count; i++) {
                if (strcmp(arr[i].name, name) == 0) {
                    printf("%s,%s,%s\n", arr[i].name, arr[i].email, arr[i].phone);
                    break;
                }
            }
        }
        free(line);
    }
    for (size_t i = 0; i < count; i++) {
        free(arr[i].name);
        free(arr[i].email);
        free(arr[i].phone);
    }
    free(arr);
    return 0;
}
