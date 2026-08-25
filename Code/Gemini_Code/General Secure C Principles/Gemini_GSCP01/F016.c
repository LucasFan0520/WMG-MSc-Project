// F016.c
#define _GNU_SOURCE
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
    size_t count = 0;
    FILE *fp = fopen(argv[1], "r");
    char *line = NULL;
    size_t line_len = 0;
    ssize_t read;
    if (fp) {
        while ((read = getline(&line, &line_len, fp)) != -1) {
            if (read > 0 && line[read - 1] == '\n') line[read - 1] = '\0';
            if (read > 1 && line[read - 2] == '\r') line[read - 2] = '\0';
            char *c1 = strchr(line, ',');
            if (!c1) continue;
            char *c2 = strchr(c1 + 1, ',');
            if (!c2) continue;
            *c1 = '\0';
            *c2 = '\0';
            CSVContact *temp = realloc(list, (count + 1) * sizeof(CSVContact));
            if (temp) {
                list = temp;
                list[count].name = strdup(line);
                list[count].email = strdup(c1 + 1);
                list[count].phone = strdup(c2 + 1);
                count++;
            }
        }
        fclose(fp);
    }
    while ((read = getline(&line, &line_len, stdin)) != -1) {
        if (read > 0 && line[read - 1] == '\n') line[read - 1] = '\0';
        if (read > 1 && line[read - 2] == '\r') line[read - 2] = '\0';
        char *p = line;
        while (*p == ' ' || *p == '\t') p++;
        if (*p == '\0') continue;
        char *cmd = p;
        while (*p && *p != ' ' && *p != '\t') p++;
        if (*p) {
            *p = '\0';
            p++;
        }
        if (strcmp(cmd, "END") == 0) {
            break;
        }
        if (strcmp(cmd, "ADD") == 0) {
            while (*p == ' ' || *p == '\t') p++;
            char *args = p;
            char *c1 = strchr(args, ',');
            if (!c1) continue;
            char *c2 = strchr(c1 + 1, ',');
            if (!c2) continue;
            *c1 = '\0';
            *c2 = '\0';
            CSVContact *temp = realloc(list, (count + 1) * sizeof(CSVContact));
            if (temp) {
                list = temp;
                list[count].name = strdup(args);
                list[count].email = strdup(c1 + 1);
                list[count].phone = strdup(c2 + 1);
                count++;
            }
        } else if (strcmp(cmd, "DELETE") == 0) {
            while (*p == ' ' || *p == '\t') p++;
            char *name = p;
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
        } else if (strcmp(cmd, "FIND") == 0) {
            while (*p == ' ' || *p == '\t') p++;
            char *name = p;
            for (size_t i = 0; i < count; i++) {
                if (strcmp(list[i].name, name) == 0) {
                    printf("%s,%s,%s\n", list[i].name, list[i].email, list[i].phone);
                    break;
                }
            }
        } else if (strcmp(cmd, "REPORT") == 0) {
            for (size_t i = 0; i < count; i++) {
                printf("%s,%s,%s\n", list[i].name, list[i].email, list[i].phone);
            }
        }
    }
    for (size_t i = 0; i < count; i++) {
        free(list[i].name);
        free(list[i].email);
        free(list[i].phone);
    }
    free(list);
    free(line);
    return 0;
}
