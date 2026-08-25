// F031.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *name;
    char *note;
} Record;

int main() {
    Record *storage = NULL;
    int count = 0;
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\r\n")] = 0;
        char *cmd = strtok(line, " ");
        if (!cmd) continue;
        if (strcmp(cmd, "END") == 0) break;
        if (strcmp(cmd, "BATCH") == 0) {
            char *n_str = strtok(NULL, " ");
            if (n_str) {
                int n = atoi(n_str);
                Record *batch_rec = malloc(n * sizeof(Record));
                int batch_valid = 1;
                for (int i = 0; i < n; i++) {
                    if (fgets(line, sizeof(line), stdin)) {
                        line[strcspn(line, "\r\n")] = 0;
                        char *name = strtok(line, " ");
                        char *note = strtok(NULL, "");
                        if (name && note) {
                            batch_rec[i].name = strdup(name);
                            batch_rec[i].note = strdup(note);
                        } else {
                            batch_rec[i].name = NULL;
                            batch_rec[i].note = NULL;
                            batch_valid = 0;
                        }
                    } else {
                        batch_rec[i].name = NULL;
                        batch_rec[i].note = NULL;
                        batch_valid = 0;
                    }
                }
                if (batch_valid) {
                    storage = realloc(storage, (count + n) * sizeof(Record));
                    for (int i = 0; i < n; i++) {
                        storage[count + i] = batch_rec[i];
                    }
                    count += n;
                    free(batch_rec);
                } else {
                    for (int i = 0; i < n; i++) {
                        if (batch_rec[i].name) free(batch_rec[i].name);
                        if (batch_rec[i].note) free(batch_rec[i].note);
                    }
                    free(batch_rec);
                }
            }
        } else if (strcmp(cmd, "DELETE") == 0) {
            char *name = strtok(NULL, " ");
            if (name) {
                for (int i = 0; i < count; i++) {
                    if (strcmp(storage[i].name, name) == 0) {
                        free(storage[i].name);
                        free(storage[i].note);
                        for (int j = i; j < count - 1; j++) {
                            storage[j] = storage[j + 1];
                        }
                        count--;
                        i--;
                    }
                }
                if (count == 0) {
                    free(storage);
                    storage = NULL;
                } else {
                    storage = realloc(storage, count * sizeof(Record));
                }
            }
        } else if (strcmp(cmd, "FIND") == 0) {
            char *name = strtok(NULL, " ");
            if (name) {
                for (int i = 0; i < count; i++) {
                    if (strcmp(storage[i].name, name) == 0) {
                        printf("%s\n", storage[i].note);
                        break;
                    }
                }
            }
        } else if (strcmp(cmd, "REPORT") == 0) {
            for (int i = 0; i < count; i++) {
                printf("%s %s\n", storage[i].name, storage[i].note);
            }
        }
    }
    for (int i = 0; i < count; i++) {
        free(storage[i].name);
        free(storage[i].note);
    }
    free(storage);
    return 0;
}
