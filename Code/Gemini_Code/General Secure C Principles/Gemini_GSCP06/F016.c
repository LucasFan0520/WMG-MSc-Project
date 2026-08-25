// F016.c
#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *name;
    char *email;
    char *phone;
} Contact;

int main(int argc, char **argv) {
    if (argc < 2) return 1;
    Contact *list = NULL;
    size_t count = 0;
    size_t capacity = 0;
    
    FILE *f = fopen(argv[1], "r");
    if (f) {
        char *fline = NULL;
        size_t flen = 0;
        ssize_t fread;
        while ((fread = getline(&fline, &flen, f)) != -1) {
            fline[strcspn(fline, "\r\n")] = '\0';
            char *p1 = strchr(fline, ',');
            if (p1) {
                *p1 = '\0';
                char *name = fline;
                char *p2 = strchr(p1 + 1, ',');
                if (p2) {
                    *p2 = '\0';
                    char *email = p1 + 1;
                    char *phone = p2 + 1;
                    if (count >= capacity) {
                        capacity = capacity == 0 ? 4 : capacity * 2;
                        Contact *next = realloc(list, capacity * sizeof(Contact));
                        if (!next) break;
                        list = next;
                    }
                    list[count].name = strdup(name);
                    list[count].email = strdup(email);
                    list[count].phone = strdup(phone);
                    count++;
                }
            }
        }
        free(fline);
        fclose(f);
    }

    char *line = NULL;
    size_t len = 0;
    ssize_t read;
    while ((read = getline(&line, &len, stdin)) != -1) {
        line[strcspn(line, "\r\n")] = '\0';
        if (strcmp(line, "END") == 0) {
            break;
        }
        if (strncmp(line, "ADD ", 4) == 0) {
            char *args = line + 4;
            char *p1 = strchr(args, ',');
            if (p1) {
                *p1 = '\0';
                char *name = args;
                char *p2 = strchr(p1 + 1, ',');
                if (p2) {
                    *p2 = '\0';
                    char *email = p1 + 1;
                    char *phone = p2 + 1;
                    if (count >= capacity) {
                        capacity = capacity == 0 ? 4 : capacity * 2;
                        Contact *next = realloc(list, capacity * sizeof(Contact));
                        if (!next) break;
                        list = next;
                    }
                    list[count].name = strdup(name);
                    list[count].email = strdup(email);
                    list[count].phone = strdup(phone);
                    count++;
                }
            }
        } else if (strncmp(line, "DELETE ", 7) == 0) {
            char *name = line + 7;
            for (size_t i = 0; i < count; i++) {
                if (strcmp(list[i].name, name) == 0) {
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
        } else if (strncmp(line, "FIND ", 5) == 0) {
            char *name = line + 5;
            for (size_t i = 0; i < count; i++) {
                if (strcmp(list[i].name, name) == 0) {
                    printf("%s,%s,%s\n", list[i].name, list[i].email, list[i].phone);
                    break;
                }
            }
        } else if (strcmp(line, "REPORT") == 0) {
            for (size_t i = 0; i < count; i++) {
                printf("%s,%s,%s\n", list[i].name, list[i].email, list[i].phone);
            }
        }
    }
    free(line);
    for (size_t i = 0; i < count; i++) {
        free(list[i].name);
        free(list[i].email);
        free(list[i].phone);
    }
    free(list);
    return 0;
}
