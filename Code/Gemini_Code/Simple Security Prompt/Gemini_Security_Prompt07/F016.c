// F016.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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
    int count = 0;
    if (argc > 1) {
        FILE *f = fopen(argv[1], "r");
        if (f) {
            char fline[4096];
            while (fgets(fline, sizeof(fline), f)) {
                fline[strcspn(fline, "\r\n")] = 0;
                char *name = NULL;
                char *email = NULL;
                char *phone = NULL;
                if (parse_csv_line(fline, &name, &email, &phone)) {
                    list = realloc(list, (count + 1) * sizeof(CSVContact));
                    list[count].name = strdup(name);
                    list[count].email = strdup(email);
                    list[count].phone = strdup(phone);
                    count++;
                }
            }
            fclose(f);
        }
    }
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\r\n")] = 0;
        char *cmd = strtok(line, " ");
        if (!cmd) {
            continue;
        }
        if (strcmp(cmd, "END") == 0) {
            break;
        }
        if (strcmp(cmd, "ADD") == 0) {
            char *args = strtok(NULL, "");
            if (args) {
                char *name = NULL;
                char *email = NULL;
                char *phone = NULL;
                if (parse_csv_line(args, &name, &email, &phone)) {
                    list = realloc(list, (count + 1) * sizeof(CSVContact));
                    list[count].name = strdup(name);
                    list[count].email = strdup(email);
                    list[count].phone = strdup(phone);
                    count++;
                }
            }
        } else if (strcmp(cmd, "DELETE") == 0) {
            char *name = strtok(NULL, "");
            if (name) {
                for (int i = 0; i < count; i++) {
                    if (strcmp(list[i].name, name) == 0) {
                        free(list[i].name);
                        free(list[i].email);
                        free(list[i].phone);
                        for (int j = i; j < count - 1; j++) {
                            list[j] = list[j + 1];
                        }
                        count--;
                        i--;
                    }
                }
                if (count == 0) {
                    free(list);
                    list = NULL;
                } else {
                    list = realloc(list, count * sizeof(CSVContact));
                }
            }
        } else if (strcmp(cmd, "FIND") == 0) {
            char *name = strtok(NULL, "");
            if (name) {
                for (int i = 0; i < count; i++) {
                    if (strcmp(list[i].name, name) == 0) {
                        printf("%s,%s,%s\n", list[i].name, list[i].email, list[i].phone);
                        break;
                    }
                }
            }
        } else if (strcmp(cmd, "REPORT") == 0) {
            for (int i = 0; i < count; i++) {
                printf("%s,%s,%s\n", list[i].name, list[i].email, list[i].phone);
            }
        }
    }
    for (int i = 0; i < count; i++) {
        free(list[i].name);
        free(list[i].email);
        free(list[i].phone);
    }
    free(list);
    return 0;
}
