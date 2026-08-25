// F034.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *name;
    char *note;
} NoteItem;

int main(int argc, char **argv) {
    if (argc < 2) return 1;
    NoteItem *store = NULL;
    int count = 0;
    char line[8192];
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
        if (strcmp(cmd, "LOAD") == 0) {
            FILE *fp = fopen(argv[1], "r");
            if (fp) {
                NoteItem *temp_store = NULL;
                int temp_count = 0;
                char fline[8192];
                int valid = 1;
                while (fgets(fline, sizeof(fline), fp)) {
                    fline[strcspn(fline, "\r\n")] = 0;
                    if (strlen(fline) == 0) continue;
                    char name[256];
                    int b = 0;
                    if (sscanf(fline, "%255s%n", name, &b) <= 0) {
                        valid = 0;
                        break;
                    }
                    char *note = fline + b;
                    while (*note == ' ') note++;
                    if (strlen(note) == 0) {
                        valid = 0;
                        break;
                    }
                    temp_store = realloc(temp_store, (temp_count + 1) * sizeof(NoteItem));
                    temp_store[temp_count].name = strdup(name);
                    temp_store[temp_count].note = strdup(note);
                    temp_count++;
                }
                fclose(fp);
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
        } else if (strcmp(cmd, "ADD") == 0) {
            char name[256];
            int b = 0;
            if (sscanf(args, "%255s%n", name, &b) == 1) {
                char *note = args + b;
                while (*note == ' ') note++;
                store = realloc(store, (count + 1) * sizeof(NoteItem));
                store[count].name = strdup(name);
                store[count].note = strdup(note);
                count++;
            }
        } else if (strcmp(cmd, "DELETE") == 0) {
            char name[256];
            if (sscanf(args, "%255s", name) == 1) {
                for (int i = 0; i < count; i++) {
                    if (strcmp(store[i].name, name) == 0) {
                        free(store[i].name);
                        free(store[i].note);
                        for (int j = i; j < count - 1; j++) {
                            store[j] = store[j + 1];
                        }
                        count--;
                        store = realloc(store, count * sizeof(NoteItem));
                        break;
                    }
                }
            }
        } else if (strcmp(cmd, "REPORT") == 0) {
            for (int i = 0; i < count; i++) {
                printf("%s %s\n", store[i].name, store[i].note);
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
