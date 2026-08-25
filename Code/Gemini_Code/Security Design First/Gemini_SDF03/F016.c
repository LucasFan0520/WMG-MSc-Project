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
    Contact *arr = NULL;
    size_t count = 0;
    size_t capacity = 0;
    FILE *f = fopen(argv[1], "r");
    if (f) {
        char *fline = NULL;
        size_t flen = 0;
        ssize_t fread;
        while ((fread = getline(&fline, &flen, f)) != -1) {
            if (fread > 0 && fline[fread - 1] == '\n') {
                fline[fread - 1] = '\0';
            }
            char *c1 = strchr(fline, ',');
            if (!c1) continue;
            char *c2 = strchr(c1 + 1, ',');
            if (!c2) continue;
            *c1 = '\0';
            *c2 = '\0';
            char *name = fline;
            char *email = c1 + 1;
            char *phone = c2 + 1;
            if (count >= capacity) {
                capacity = capacity == 0 ? 4 : capacity * 2;
                Contact *tmp = realloc(arr, capacity * sizeof(Contact));
                if (!tmp) exit(1);
                arr = tmp;
            }
            arr[count].name = strdup(name);
            arr[count].email = strdup(email);
            arr[count].phone = strdup(phone);
            if (!arr[count].name || !arr[count].email || !arr[count].phone) exit(1);
            count++;
        }
        free(fline);
        fclose(f);
    }
    char *line = NULL;
    size_t len = 0;
    ssize_t read;
    while ((read = getline(&line, &len, stdin)) != -1) {
        if (read > 0 && line[read - 1] == '\n') {
            line[read - 1] = '\0';
        }
        if (strcmp(line, "END") == 0) {
            break;
        }
        if (strncmp(line, "ADD ", 4) == 0) {
            char *csv = line + 4;
            while (*csv == ' ') csv++;
            char *c1 = strchr(csv, ',');
            if (!c1) continue;
            char *c2 = strchr(c1 + 1, ',');
            if (!c2) continue;
            *c1 = '\0';
            *c2 = '\0';
            char *name = csv;
            char *email = c1 + 1;
            char *phone = c2 + 1;
            if (count >= capacity) {
                capacity = capacity == 0 ? 4 : capacity * 2;
                Contact *tmp = realloc(arr, capacity * sizeof(Contact));
                if (!tmp) exit(1);
                arr = tmp;
            }
            arr[count].name = strdup(name);
            arr[count].email = strdup(email);
            arr[count].phone = strdup(phone);
            if (!arr[count].name || !arr[count].email || !arr[count].phone) exit(1);
            count++;
        } else if (strncmp(line, "DELETE ", 7) == 0) {
            char *name = line + 7;
            while (*name == ' ') name++;
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
        } else if (strncmp(line, "FIND ", 5) == 0) {
            char *name = line + 5;
            while (*name == ' ') name++;
            for (size_t i = 0; i < count; i++) {
                if (strcmp(arr[i].name, name) == 0) {
                    printf("%s,%s,%s\n", arr[i].name, arr[i].email, arr[i].phone);
                    break;
                }
            }
        } else if (strcmp(line, "REPORT") == 0) {
            for (size_t i = 0; i < count; i++) {
                printf("%s,%s,%s\n", arr[i].name, arr[i].email, arr[i].phone);
            }
        }
    }
    for (size_t i = 0; i < count; i++) {
        free(arr[i].name);
        free(arr[i].email);
        free(arr[i].phone);
    }
    free(arr);
    free(line);
    return 0;
}
