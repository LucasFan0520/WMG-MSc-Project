// F031.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *name;
    char *note;
} ImportRecord;

int main(void) {
    ImportRecord *records = NULL;
    int count = 0;
    char *line = NULL;
    size_t cap = 0;
    while (getline(&line, &cap, stdin) != -1) {
        line[strcspn(line, "\r\n")] = 0;
        if (strcmp(line, "END") == 0) {
            break;
        }
        if (strncmp(line, "BATCH ", 6) == 0) {
            int n;
            if (sscanf(line + 6, "%d", &n) == 1 && n >= 0) {
                char **bnames = malloc(n * sizeof(char *));
                char **bnotes = malloc(n * sizeof(char *));
                int all_valid = 1;
                for (int i = 0; i < n; i++) {
                    char *bline = NULL;
                    size_t bcap = 0;
                    if (getline(&bline, &bcap, stdin) != -1) {
                        bline[strcspn(bline, "\r\n")] = 0;
                        char bname[256];
                        if (sscanf(bline, "%255s", bname) == 1) {
                            char *bnote = strchr(bline, ' ');
                            if (bnote) {
                                while (*bnote == ' ') bnote++;
                            }
                            if (!bnote) bnote = "";
                            bnames[i] = strdup(bname);
                            bnotes[i] = strdup(bnote);
                        } else {
                            all_valid = 0;
                            bnames[i] = NULL;
                            bnotes[i] = NULL;
                        }
                    } else {
                        all_valid = 0;
                        bnames[i] = NULL;
                        bnotes[i] = NULL;
                    }
                    free(bline);
                }
                if (all_valid) {
                    ImportRecord *tmp = realloc(records, (count + n) * sizeof(ImportRecord));
                    if (tmp) {
                        records = tmp;
                        for (int i = 0; i < n; i++) {
                            records[count].name = bnames[i];
                            records[count].note = bnotes[i];
                            count++;
                        }
                    } else {
                        for (int i = 0; i < n; i++) {
                            free(bnames[i]);
                            free(bnotes[i]);
                        }
                    }
                } else {
                    for (int i = 0; i < n; i++) {
                        free(bnames[i]);
                        free(bnotes[i]);
                    }
                }
                free(bnames);
                free(bnotes);
            }
        } else if (strncmp(line, "DELETE ", 7) == 0) {
            char *name = line + 7;
            while (*name == ' ') name++;
            int i = 0;
            while (i < count) {
                if (strcmp(records[i].name, name) == 0) {
                    free(records[i].name);
                    free(records[i].note);
                    for (int j = i; j < count - 1; j++) {
                        records[j] = records[j + 1];
                    }
                    count--;
                } else {
                    i++;
                }
            }
        } else if (strncmp(line, "FIND ", 5) == 0) {
            char *name = line + 5;
            while (*name == ' ') name++;
            for (int i = 0; i < count; i++) {
                if (strcmp(records[i].name, name) == 0) {
                    printf("%s: %s\n", records[i].name, records[i].note);
                    break;
                }
            }
        } else if (strcmp(line, "REPORT") == 0) {
            for (int i = 0; i < count; i++) {
                printf("%s: %s\n", records[i].name, records[i].note);
            }
        }
    }
    free(line);
    for (int i = 0; i < count; i++) {
        free(records[i].name);
        free(records[i].note);
    }
    free(records);
    return 0;
}
