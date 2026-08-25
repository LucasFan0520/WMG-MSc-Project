// F016.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *name;
    char *email;
    char *phone;
} CSVContact;

int main(int argc, char *argv[]) {
    CSVContact *list = NULL;
    int count = 0;
    if (argc > 1) {
        FILE *f = fopen(argv[1], "r");
        if (f) {
            char fline[1024];
            while (fgets(fline, sizeof(fline), f)) {
                int flen = strlen(fline);
                while (flen > 0 && (fline[flen - 1] == '\n' || fline[flen - 1] == '\r')) {
                    fline[flen - 1] = '\0';
                    flen--;
                }
                char *c1 = strchr(fline, ',');
                if (!c1) continue;
                char *c2 = strchr(c1 + 1, ',');
                if (!c2) continue;
                *c1 = '\0';
                *c2 = '\0';
                list = realloc(list, (count + 1) * sizeof(CSVContact));
                list[count].name = strdup(fline);
                list[count].email = strdup(c1 + 1);
                list[count].phone = strdup(c2 + 1);
                count++;
            }
            fclose(f);
        }
    }
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        int len = strlen(line);
        while (len > 0 && (line[len - 1] == '\n' || line[len - 1] == '\r')) {
            line[len - 1] = '\0';
            len--;
        }
        if (strcmp(line, "END") == 0) {
            break;
        }
        char cmd[32];
        int read_bytes = 0;
        if (sscanf(line, "%31s%n", cmd, &read_bytes) <= 0) {
            continue;
        }
        char *ptr = line + read_bytes;
        while (*ptr == ' ') ptr++;
        if (strcmp(cmd, "ADD") == 0) {
            char *c1 = strchr(ptr, ',');
            if (!c1) continue;
            char *c2 = strchr(c1 + 1, ',');
            if (!c2) continue;
            *c1 = '\0';
            *c2 = '\0';
            list = realloc(list, (count + 1) * sizeof(CSVContact));
            list[count].name = strdup(ptr);
            list[count].email = strdup(c1 + 1);
            list[count].phone = strdup(c2 + 1);
            count++;
        } else if (strcmp(cmd, "DELETE") == 0) {
            char name[256];
            if (sscanf(ptr, "%255s", name) > 0) {
                int found = -1;
                for (int i = 0; i < count; i++) {
                    if (strcmp(list[i].name, name) == 0) {
                        found = i;
                        break;
                    }
                }
                if (found != -1) {
                    free(list[found].name);
                    free(list[found].email);
                    free(list[found].phone);
                    for (int i = found; i < count - 1; i++) {
                        list[i] = list[i + 1];
                    }
                    count--;
                    if (count > 0) {
                        list = realloc(list, count * sizeof(CSVContact));
                    } else {
                        free(list);
                        list = NULL;
                    }
                }
            }
        } else if (strcmp(cmd, "FIND") == 0) {
            char name[256];
            if (sscanf(ptr, "%255s", name) > 0) {
                int found = -1;
                for (int i = 0; i < count; i++) {
                    if (strcmp(list[i].name, name) == 0) {
                        found = i;
                        break;
                    }
                }
                if (found != -1) {
                    printf("%s,%s,%s\n", list[found].name, list[found].email, list[found].phone);
                } else {
                    printf("NOT_FOUND\n");
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
