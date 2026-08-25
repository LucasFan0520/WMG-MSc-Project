// F016.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *name;
    char *email;
    char *phone;
} CSVContact;

int main(int argc, char **argv) {
    if (argc < 2) {
        return 1;
    }
    CSVContact *list = NULL;
    int count = 0;
    FILE *fp = fopen(argv[1], "r");
    char *line = NULL;
    size_t cap = 0;
    ssize_t read;
    if (fp) {
        while ((read = getline(&line, &cap, fp)) != -1) {
            line[strcspn(line, "\r\n")] = 0;
            char *c1 = strchr(line, ',');
            if (!c1) {
                continue;
            }
            *c1 = '\0';
            char *c2 = strchr(c1 + 1, ',');
            if (!c2) {
                continue;
            }
            *c2 = '\0';
            CSVContact *tmp = realloc(list, (count + 1) * sizeof(CSVContact));
            if (tmp) {
                list = tmp;
                list[count].name = strdup(line);
                list[count].email = strdup(c1 + 1);
                list[count].phone = strdup(c2 + 1);
                count++;
            }
        }
        fclose(fp);
    }
    while ((read = getline(&line, &cap, stdin)) != -1) {
        line[strcspn(line, "\r\n")] = 0;
        if (strcmp(line, "END") == 0) {
            break;
        }
        if (strncmp(line, "ADD ", 4) == 0) {
            char *args = line + 4;
            while (*args == ' ') {
                args++;
            }
            char *c1 = strchr(args, ',');
            if (!c1) {
                continue;
            }
            *c1 = '\0';
            char *c2 = strchr(c1 + 1, ',');
            if (!c2) {
                continue;
            }
            *c2 = '\0';
            CSVContact *tmp = realloc(list, (count + 1) * sizeof(CSVContact));
            if (tmp) {
                list = tmp;
                list[count].name = strdup(args);
                list[count].email = strdup(c1 + 1);
                list[count].phone = strdup(c2 + 1);
                count++;
            }
        } else if (strncmp(line, "DELETE ", 7) == 0) {
            char *name = line + 7;
            while (*name == ' ') {
                name++;
            }
            for (int i = 0; i < count; i++) {
                if (strcmp(list[i].name, name) == 0) {
                    free(list[i].name);
                    free(list[i].email);
                    free(list[i].phone);
                    for (int j = i; j < count - 1; j++) {
                        list[j] = list[j + 1];
                    }
                    count--;
                    break;
                }
            }
        } else if (strncmp(line, "FIND ", 5) == 0) {
            char *name = line + 5;
            while (*name == ' ') {
                name++;
            }
            for (int i = 0; i < count; i++) {
                if (strcmp(list[i].name, name) == 0) {
                    printf("%s,%s,%s\n", list[i].name, list[i].email, list[i].phone);
                    break;
                }
            }
        } else if (strcmp(line, "REPORT") == 0) {
            for (int i = 0; i < count; i++) {
                printf("%s,%s,%s\n", list[i].name, list[i].email, list[i].phone);
            }
        }
    }
    free(line);
    for (int i = 0; i < count; i++) {
        free(list[i].name);
        free(list[i].email);
        free(list[i].phone);
    }
    free(list);
    return 0;
}
