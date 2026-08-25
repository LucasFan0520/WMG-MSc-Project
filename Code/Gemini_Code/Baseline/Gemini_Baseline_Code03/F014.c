// F014.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *name;
    char *note;
    char **labels;
    int l_count;
} TrackerItem;

int main() {
    TrackerItem *items = NULL;
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
        if (strcmp(cmd, "ITEM") == 0) {
            char name[256];
            int n_len = 0;
            if (sscanf(ptr, "%255s%n", name, &n_len) > 0) {
                char *note = ptr + n_len;
                while (*note == ' ') note++;
                items = realloc(items, (count + 1) * sizeof(TrackerItem));
                items[count].name = strdup(name);
                items[count].note = strdup(note);
                items[count].labels = NULL;
                items[count].l_count = 0;
                count++;
            }
        } else if (strcmp(cmd, "LABEL") == 0) {
            char name[256], label[256];
            if (sscanf(ptr, "%255s %255s", name, label) == 2) {
                for (int i = 0; i < count; i++) {
                    if (strcmp(items[i].name, name) == 0) {
                        items[i].labels = realloc(items[i].labels, (items[i].l_count + 1) * sizeof(char *));
                        items[i].labels[items[i].l_count] = strdup(label);
                        items[i].l_count++;
                        break;
                    }
                }
            }
        } else if (strcmp(cmd, "UNLABEL") == 0) {
            char name[256], label[256];
            if (sscanf(ptr, "%255s %255s", name, label) == 2) {
                for (int i = 0; i < count; i++) {
                    if (strcmp(items[i].name, name) == 0) {
                        int f_idx = -1;
                        for (int j = 0; j < items[i].l_count; j++) {
                            if (strcmp(items[i].labels[j], label) == 0) {
                                f_idx = j;
                                break;
                            }
                        }
                        if (f_idx != -1) {
                            free(items[i].labels[f_idx]);
                            for (int j = f_idx; j < items[i].l_count - 1; j++) {
                                items[i].labels[j] = items[i].labels[j + 1];
                            }
                            items[i].l_count--;
                            if (items[i].l_count > 0) {
                                items[i].labels = realloc(items[i].labels, items[i].l_count * sizeof(char *));
                            } else {
                                free(items[i].labels);
                                items[i].labels = NULL;
                            }
                        }
                        break;
                    }
                }
            }
        } else if (strcmp(cmd, "DELETE") == 0) {
            char name[256];
            if (sscanf(ptr, "%255s", name) > 0) {
                int found = -1;
                for (int i = 0; i < count; i++) {
                    if (strcmp(items[i].name, name) == 0) {
                        found = i;
                        break;
                    }
                }
                if (found != -1) {
                    free(items[found].name);
                    free(items[found].note);
                    for (int j = 0; j < items[found].l_count; j++) {
                        free(items[found].labels[j]);
                    }
                    free(items[found].labels);
                    for (int i = found; i < count - 1; i++) {
                        items[i] = items[i + 1];
                    }
                    count--;
                    if (count > 0) {
                        items = realloc(items, count * sizeof(TrackerItem));
                    } else {
                        free(items);
                        items = NULL;
                    }
                }
            }
        } else if (strcmp(cmd, "FIND") == 0) {
            char name[256];
            if (sscanf(ptr, "%255s", name) > 0) {
                int found = -1;
                for (int i = 0; i < count; i++) {
                    if (strcmp(items[i].name, name) == 0) {
                        found = i;
                        break;
                    }
                }
                if (found != -1) {
                    printf("%s %s Labels:", items[found].name, items[found].note);
                    for (int j = 0; j < items[found].l_count; j++) {
                        printf(" %s", items[found].labels[j]);
                    }
                    printf("\n");
                } else {
                    printf("NOT_FOUND\n");
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
        for (int j = 0; j < items[i].l_count; j++) {
            free(items[i].labels[j]);
        }
        free(items[i].labels);
    }
    free(items);
    return 0;
}
