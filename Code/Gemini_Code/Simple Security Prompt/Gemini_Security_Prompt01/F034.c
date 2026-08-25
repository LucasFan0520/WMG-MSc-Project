// F034.c
#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *name;
    char *note;
} StoreNote;

int main(int argc, char **argv) {
    char *filepath = (argc >= 2) ? argv[1] : NULL;
    StoreNote *store = NULL;
    int count = 0;

    char *line = NULL;
    size_t len = 0;
    ssize_t read;

    while ((read = getline(&line, &len, stdin)) != -1) {
        line[strcspn(line, "\r\n")] = '\0';
        if (strcmp(line, "END") == 0) {
            break;
        }

        char *cmd = line;
        char *space1 = strchr(line, ' ');
        char *args = NULL;
        if (space1) {
            *space1 = '\0';
            args = space1 + 1;
            while (*args == ' ') args++;
        }

        if (strcmp(cmd, "LOAD") == 0) {
            if (filepath) {
                FILE *fp = fopen(filepath, "r");
                if (fp) {
                    StoreNote *temp_store = NULL;
                    int temp_count = 0;
                    int all_valid = 1;
                    char *fline = NULL;
                    size_t flen = 0;
                    ssize_t fread;

                    while ((fread = getline(&fline, &flen, fp)) != -1) {
                        fline[strcspn(fline, "\r\n")] = '\0';
                        char *sp = strchr(fline, ' ');
                        if (sp && sp != fline) {
                            *sp = '\0';
                            char *name = fline;
                            char *note = sp + 1;
                            while (*note == ' ') note++;
                            temp_store = realloc(temp_store, (temp_count + 1) * sizeof(StoreNote));
                            if (!temp_store) return 1;
                            temp_store[temp_count].name = strdup(name);
                            temp_store[temp_count].note = strdup(note);
                            temp_count++;
                        } else {
                            all_valid = 0;
                            break;
                        }
                    }
                    free(fline);
                    fclose(fp);

                    if (all_valid) {
                        for (int i = 0; i < count; i++) {
                            free(store[i].name);
                            free(store[i].note);
                        }
                        free(store);
                        store = temp_store;
                        count = temp_count;
                    } else {
                        for (int i = 0; i < temp_count; i++) {
                            free(temp_store[i].name);
                            free(temp_store[i].note);
                        }
                        free(temp_store);
                    }
                }
            }
        } else if (strcmp(cmd, "ADD") == 0 && args) {
            char *name = args;
            char *space2 = strchr(args, ' ');
            char *note = "";
            if (space2) {
                *space2 = '\0';
                note = space2 + 1;
                while (*note == ' ') note++;
            }
            store = realloc(store, (count + 1) * sizeof(StoreNote));
            if (!store) return 1;
            store[count].name = strdup(name);
            store[count].note = strdup(note);
            count++;
        } else if (strcmp(cmd, "DELETE") == 0 && args) {
            for (int i = 0; i < count; i++) {
                if (strcmp(store[i].name, args) == 0) {
                    free(store[i].name);
                    free(store[i].note);
                    for (int j = i; j < count - 1; j++) {
                        store[j] = store[j + 1];
                    }
                    count--;
                    store = realloc(store, count * sizeof(StoreNote));
                    if (count > 0 && !store) return 1;
                    break;
                }
            }
        } else if (strcmp(cmd, "REPORT") == 0) {
            for (int i = 0; i < count; i++) {
                printf("%s %s\n", store[i].name, store[i].note);
            }
        }
    }

    free(line);
    for (int i = 0; i < count; i++) {
        free(store[i].name);
        free(store[i].note);
    }
    free(store);
    return 0;
}
