// F020.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *title;
    char **body;
    int body_count;
} Record;

int main(int argc, char **argv) {
    if (argc < 2) return 1;
    Record *catalog = NULL;
    int count = 0;
    FILE *fp = fopen(argv[1], "r");
    if (fp) {
        char fline[1024];
        int in_record = 0;
        while (fgets(fline, sizeof(fline), fp)) {
            fline[strcspn(fline, "\r\n")] = 0;
            if (strcmp(fline, "--") == 0) {
                in_record = 0;
                continue;
            }
            if (!in_record) {
                catalog = realloc(catalog, (count + 1) * sizeof(Record));
                catalog[count].title = strdup(fline);
                catalog[count].body = NULL;
                catalog[count].body_count = 0;
                in_record = 1;
                count++;
            } else {
                catalog[count - 1].body = realloc(catalog[count - 1].body, (catalog[count - 1].body_count + 1) * sizeof(char *));
                catalog[count - 1].body[catalog[count - 1].body_count] = strdup(fline);
                catalog[count - 1].body_count++;
            }
        }
        fclose(fp);
    }
    char line[1024];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\r\n")] = 0;
        if (strcmp(line, "END") == 0) {
            break;
        }
        char cmd[16];
        int read_bytes = 0;
        if (sscanf(line, "%15s%n", cmd, &read_bytes) <= 0) {
            continue;
        }
        char *args = line + read_bytes;
        while (*args == ' ') args++;
        if (strcmp(cmd, "ADD") == 0) {
            catalog = realloc(catalog, (count + 1) * sizeof(Record));
            catalog[count].title = strdup(args);
            catalog[count].body = NULL;
            catalog[count].body_count = 0;
            char bline[1024];
            while (fgets(bline, sizeof(bline), stdin)) {
                bline[strcspn(bline, "\r\n")] = 0;
                if (strcmp(bline, "--") == 0) {
                    break;
                }
                catalog[count].body = realloc(catalog[count].body, (catalog[count].body_count + 1) * sizeof(char *));
                catalog[count].body[catalog[count].body_count] = strdup(bline);
                catalog[count].body_count++;
            }
            count++;
        } else if (strcmp(cmd, "DELETE") == 0) {
            for (int i = 0; i < count; i++) {
                if (strcmp(catalog[i].title, args) == 0) {
                    free(catalog[i].title);
                    for (int j = 0; j < catalog[i].body_count; j++) {
                        free(catalog[i].body[j]);
                    }
                    free(catalog[i].body);
                    for (int j = i; j < count - 1; j++) {
                        catalog[j] = catalog[j + 1];
                    }
                    count--;
                    catalog = realloc(catalog, count * sizeof(Record));
                    break;
                }
            }
        } else if (strcmp(cmd, "FIND") == 0) {
            for (int i = 0; i < count; i++) {
                if (strcmp(catalog[i].title, args) == 0) {
                    printf("Title: %s\n", catalog[i].title);
                    for (int j = 0; j < catalog[i].body_count; j++) {
                        printf("%s\n", catalog[i].body[j]);
                    }
                    break;
                }
            }
        } else if (strcmp(cmd, "PRINT") == 0) {
            for (int i = 0; i < count; i++) {
                printf("Title: %s\n", catalog[i].title);
                for (int j = 0; j < catalog[i].body_count; j++) {
                    printf("%s\n", catalog[i].body[j]);
                }
            }
        }
    }
    for (int i = 0; i < count; i++) {
        free(catalog[i].title);
        for (int j = 0; j < catalog[i].body_count; j++) {
            free(catalog[i].body[j]);
        }
        free(catalog[i].body);
    }
    free(catalog);
    return 0;
}
