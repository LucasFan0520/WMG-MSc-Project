// F016.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *read_line(FILE *f) {
    size_t size = 16;
    size_t len = 0;
    char *buf = malloc(size);
    if (!buf) return NULL;
    int c;
    while ((c = fgetc(f)) != EOF && c != '\n') {
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
} CsvContact;

int main(int argc, char **argv) {
    CsvContact *arr = NULL;
    size_t count = 0;
    size_t cap = 0;
    if (argc > 1) {
        FILE *f = fopen(argv[1], "r");
        if (f) {
            while (1) {
                char *line = read_line(f);
                if (!line) break;
                char *c1 = strchr(line, ',');
                if (c1) {
                    *c1 = '\0';
                    char *email = c1 + 1;
                    char *c2 = strchr(email, ',');
                    if (c2) {
                        *c2 = '\0';
                        char *phone = c2 + 1;
                        if (count >= cap) {
                            cap = cap == 0 ? 4 : cap * 2;
                            arr = realloc(arr, cap * sizeof(CsvContact));
                        }
                        arr[count].name = strdup(line);
                        arr[count].email = strdup(email);
                        arr[count].phone = strdup(phone);
                        count++;
                    }
                }
                free(line);
            }
            fclose(f);
        }
    }
    while (1) {
        char *line = read_line(stdin);
        if (!line) break;
        if (strcmp(line, "END") == 0) {
            free(line);
            break;
        }
        if (strncmp(line, "ADD ", 4) == 0) {
            char *p = line + 4;
            char *c1 = strchr(p, ',');
            if (c1) {
                *c1 = '\0';
                char *email = c1 + 1;
                char *c2 = strchr(email, ',');
                if (c2) {
                    *c2 = '\0';
                    char *phone = c2 + 1;
                    if (count >= cap) {
                        cap = cap == 0 ? 4 : cap * 2;
                        arr = realloc(arr, cap * sizeof(CsvContact));
                    }
                    arr[count].name = strdup(p);
                    arr[count].email = strdup(email);
                    arr[count].phone = strdup(phone);
                    count++;
                }
            }
        } else if (strncmp(line, "DELETE ", 7) == 0) {
            char *name_start = line + 7;
            for (size_t i = 0; i < count; i++) {
                if (strcmp(arr[i].name, name_start) == 0) {
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
        } else if (strncmp(line, "FIND ", 5) == 0) {
            char *name_start = line + 5;
            for (size_t i = 0; i < count; i++) {
                if (strcmp(arr[i].name, name_start) == 0) {
                    printf("%s,%s,%s\n", arr[i].name, arr[i].email, arr[i].phone);
                    break;
                }
            }
        } else if (strcmp(line, "REPORT") == 0) {
            for (size_t i = 0; i < count; i++) {
                printf("%s,%s,%s\n", arr[i].name, arr[i].email, arr[i].phone);
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
