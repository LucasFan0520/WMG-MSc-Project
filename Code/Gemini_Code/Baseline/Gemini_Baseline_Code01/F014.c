// F014.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *name;
    char *note;
    char **labels;
    int label_count;
} Item;

int main() {
    Item *items = NULL;
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
        if (strcmp(cmd, "ITEM") == 0) {
            char name[256];
            int name_bytes = 0;
            if (sscanf(args, "%255s%n", name, &name_bytes) == 1) {
                char *note = args + name_bytes;
                while (*note == ' ') note++;
                items = realloc(items, (count + 1) * sizeof(Item));
                items[count].name = strdup(name);
                items[count].note = strdup(note);
                items[count].labels = NULL;
                items[count].label_count = 0;
                count++;
            }
        } else if (strcmp(cmd, "LABEL") == 0) {
            char name[256], lbl[256];
            if (sscanf(args, "%255s %255s", name, lbl) == 2) {
                for (int i = 0; i < count; i++) {
                    if (strcmp(items[i].name, name) == 0) {
                        items[i].labels = realloc(items[i].labels, (items[i].label_count + 1) * sizeof(char *));
                        items[i].labels[items[i].label_count] = strdup(lbl);
                        items[i].label_count++;
                        break;
                    }
                }
            }
        } else if (strcmp(cmd, "UNLABEL") == 0) {
            char name[256], lbl[256];
            if (sscanf(args, "%255s %255s", name, lbl) == 2) {
                for (int i = 0; i < count; i++) {
                    if (strcmp(items[i].name, name) == 0) {
                        for (int j = 0; j < items[i].label_count; j++) {
                            if (strcmp(items[i].labels[j], lbl) == 0) {
                                free(items[i].labels[j]);
                                for (int m = j; m < items[i].label_count - 1; m++) {
                                    items[i].labels[m] = items[i].labels[m + 1];
                                }
                                items[i].label_count--;
                                items[i].labels = realloc(items[i].labels, items[i].label_count * sizeof(char *));
                                break;
                            }
                        }
                        break;
                    }
                }
            }
        } else if (strcmp(cmd, "DELETE") == 0) {
            char name[256];
            if (sscanf(args, "%255s", name) == 1) {
                for (int i = 0; i < count; i++) {
                    if (strcmp(items[i].name, name) == 0) {
                        free(items[i].name);
                        free(items[i].note);
                        for (int j = 0; j < items[i].label_count; j++) {
                            free(items[i].labels[j]);
                        }
                        free(items[i].labels);
                        for (int j = i; j < count - 1; j++) {
                            items[j] = items[j + 1];
                        }
                        count--;
                        items = realloc(items, count * sizeof(Item));
                        break;
                    }
                }
            }
        } else if (strcmp(cmd, "FIND") == 0) {
            char name[256];
            if (sscanf(args, "%255s", name) == 1) {
                for (int i = 0; i < count; i++) {
                    if (strcmp(items[i].name, name) == 0) {
                        printf("%s %s Labels:", items[i].name, items[i].note);
                        for (int j = 0; j < items[i].label_count; j++) {
                            printf(" %s", items[i].labels[j]);
                        }
                        printf("\n");
                        break;
                    }
                }
            }
        } else if (strcmp(cmd, "REPORT") == 0) {
            for (int i = 0; i < count; i++) {
                printf("%s %s\n", items[i].name, items[i].note);
            }
        }
    }
    for (int i = 0; i < count; i++) {
        free(items[i].name);
        free(items[i].note);
        for (int j = 0; j < items[i].label_count; j++) {
            free(items[i].labels[j]);
        }
        free(items[i].labels);
    }
    free(items);
    return 0;
}
