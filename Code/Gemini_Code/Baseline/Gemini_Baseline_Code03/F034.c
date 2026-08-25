// F034.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *name;
    char *note;
} FileNote;

int main(int argc, char *argv[]) {
    FileNote *store = NULL;
    int count = 0;
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
        if (strcmp(cmd, "LOAD") == 0) {
            if (argc > 1) {
                FILE *f = fopen(argv[1], "r");
                if (f) {
                    FileNote *temp_store = NULL;
                    int temp_count = 0;
                    char fline[4096];
                    int valid = 1;
                    while (fgets(fline, sizeof(fline), f)) {
                        int flen = strlen(fline);
                        while (flen > 0 && (fline[flen - 1] == '\n' || fline[flen - 1] == '\r')) {
                            fline[flen - 1] = '\0';
                            flen--;
                        }
                        char name[256];
                        int name_len = 0;
                        if (sscanf(fline, "%255s%n", name, &name_len) > 0) {
                            char *note = fline + name_len;
                            while (*note == ' ') note++;
                            temp_store = realloc(temp_store, (temp_count + 1) * sizeof(FileNote));
                            temp_store[temp_count].name = strdup(name);
                            temp_store[temp_count].note = strdup(note);
                            temp_count++;
                        }
                    }
                    fclose(f);
                    if (valid) {
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
        } else if (strcmp(cmd, "ADD") == 0) {
            char name[256];
            int name_len = 0;
            if (sscanf(ptr, "%255s%n", name, &name_len) > 0) {
                char *note = ptr + name_len;
                while (*note == ' ') note++;
                store = realloc(store, (count + 1) * sizeof(FileNote));
                store[count].name = strdup(name);
                store[count].note = strdup(note);
                count++;
            }
        } else if (strcmp(cmd, "DELETE") == 0) {
            char name[256];
            if (sscanf(ptr, "%255s", name) > 0) {
                int found = -1;
                for (int i = 0; i < count; i++) {
                    if (strcmp(store[i].name, name) == 0) {
                        found = i;
                        break;
                    }
                }
                if (found != -1) {
                    free(store[found].name);
                    free(store[found].note);
                    for (int i = found; i < count - 1; i++) {
                        store[i] = store[i + 1];
                    }
                    count--;
                    if (count > 0) {
                        store = realloc(store, count * sizeof(FileNote));
                    } else {
                        free(store);
                        store = NULL;
                    }
                }
            }
        } else if (strcmp(cmd, "REPORT") == 0) {
            for (int i = 0; i < count; i++) {
                printf("%s: %s\n", store[i].name, store[i].note);
            }
        }
    }
    for (int i = 0; i < count; i++) {
        free(store[i].name);
        free(store[i].note);
    }
    free(store);
    return 0;
}
