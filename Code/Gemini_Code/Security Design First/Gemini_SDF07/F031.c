/* F031.c */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *name;
    char *note;
} Record31;

int main(void) {
    Record31 *main_store = NULL;
    int main_count = 0;
    int main_cap = 0;
    char *line = NULL;
    size_t len = 0;
    ssize_t read;

    while ((read = getline(&line, &len, stdin)) != -1) {
        line[strcspn(line, "\r\n")] = '\0';
        if (strcmp(line, "END") == 0) {
            break;
        }
        if (strncmp(line, "BATCH ", 6) == 0) {
            int n = atoi(line + 6);
            if (n <= 0) continue;
            Record31 *batch_store = malloc(n * sizeof(Record31));
            if (!batch_store) continue;
            int batch_valid = 1;
            for (int i = 0; i < n; i++) {
                char *bline = NULL;
                size_t blen = 0;
                if (getline(&bline, &blen, stdin) == -1) {
                    batch_valid = 0;
                    free(bline);
                    break;
                }
                bline[strcspn(bline, "\r\n")] = '\0';
                char *space = strchr(bline, ' ');
                if (!space || space == bline) {
                    batch_valid = 0;
                    free(bline);
                    break;
                }
                *space = '\0';
                char *name = bline;
                char *note = space + 1;
                batch_store[i].name = strdup(name);
                batch_store[i].note = strdup(note);
                if (!batch_store[i].name || !batch_store[i].note) {
                    batch_valid = 0;
                }
                free(bline);
            }
            if (batch_valid) {
                for (int i = 0; i < n; i++) {
                    if (main_count >= main_cap) {
                        int new_cap = main_cap == 0 ? 4 : main_cap * 2;
                        Record31 *tmp = realloc(main_store, new_cap * sizeof(Record31));
                        if (tmp) {
                            main_store = tmp;
                            main_cap = new_cap;
                        } else {
                            break;
                        }
                    }
                    main_store[main_count] = batch_store[i];
                    main_count++;
                }
                free(batch_store);
            } else {
                for (int i = 0; i < n; i++) {
                    if (i < main_count) {
                        free(batch_store[i].name);
                        free(batch_store[i].note);
                    }
                }
                free(batch_store);
            }
        } else if (strncmp(line, "DELETE ", 7) == 0) {
            char *name = line + 7;
            int i = 0;
            while (i < main_count) {
                if (strcmp(main_store[i].name, name) == 0) {
                    free(main_store[i].name);
                    free(main_store[i].note);
                    for (int j = i; j < main_count - 1; j++) {
                        main_store[j] = main_store[j + 1];
                    }
                    main_count--;
                } else {
                    i++;
                }
            }
        } else if (strncmp(line, "FIND ", 5) == 0) {
            char *name = line + 5;
            for (int i = 0; i < main_count; i++) {
                if (strcmp(main_store[i].name, name) == 0) {
                    printf("%s\n", main_store[i].note);
                    break;
                }
            }
        } else if (strcmp(line, "REPORT") == 0) {
            for (int i = 0; i < main_count; i++) {
                printf("%s %s\n", main_store[i].name, main_store[i].note);
            }
        }
    }
    free(line);
    for (int i = 0; i < main_count; i++) {
        free(main_store[i].name);
        free(main_store[i].note);
    }
    free(main_store);
    return 0;
}
