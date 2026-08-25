// F002.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *tag;
    char *message;
} MessageRecord;

int main(void) {
    MessageRecord *records = NULL;
    int count = 0;
    char *line = NULL;
    size_t cap = 0;
    ssize_t read;
    while ((read = getline(&line, &cap, stdin)) != -1) {
        line[strcspn(line, "\r\n")] = 0;
        if (strcmp(line, "END") == 0) {
            break;
        }
        if (strncmp(line, "ADD ", 4) == 0) {
            char *tag = line + 4;
            while (*tag == ' ') {
                tag++;
            }
            char *sp = strchr(tag, ' ');
            char *msg = "";
            if (sp) {
                *sp = '\0';
                msg = sp + 1;
                while (*msg == ' ') {
                    msg++;
                }
            }
            MessageRecord *tmp = realloc(records, (count + 1) * sizeof(MessageRecord));
            if (tmp) {
                records = tmp;
                records[count].tag = strdup(tag);
                records[count].message = strdup(msg);
                count++;
            }
        } else if (strncmp(line, "CONCAT ", 7) == 0) {
            char t1[256], t2[256], nt[256];
            if (sscanf(line + 7, "%255s %255s %255s", t1, t2, nt) == 3) {
                char *m1 = NULL;
                char *m2 = NULL;
                for (int i = 0; i < count; i++) {
                    if (!m1 && strcmp(records[i].tag, t1) == 0) {
                        m1 = records[i].message;
                    }
                    if (!m2 && strcmp(records[i].tag, t2) == 0) {
                        m2 = records[i].message;
                    }
                }
                if (m1 && m2) {
                    size_t nlen = strlen(m1) + 1 + strlen(m2) + 1;
                    char *nmsg = malloc(nlen);
                    if (nmsg) {
                        sprintf(nmsg, "%s %s", m1, m2);
                        MessageRecord *tmp = realloc(records, (count + 1) * sizeof(MessageRecord));
                        if (tmp) {
                            records = tmp;
                            records[count].tag = strdup(nt);
                            records[count].message = nmsg;
                            count++;
                        } else {
                            free(nmsg);
                        }
                    }
                }
            }
        } else if (strncmp(line, "REMOVE ", 7) == 0) {
            char *tag = line + 7;
            while (*tag == ' ') {
                tag++;
            }
            int i = 0;
            while (i < count) {
                if (strcmp(records[i].tag, tag) == 0) {
                    free(records[i].tag);
                    free(records[i].message);
                    for (int j = i; j < count - 1; j++) {
                        records[j] = records[j + 1];
                    }
                    count--;
                } else {
                    i++;
                }
            }
        } else if (strncmp(line, "FIND ", 5) == 0) {
            char *tag = line + 5;
            while (*tag == ' ') {
                tag++;
            }
            char *found = NULL;
            for (int i = 0; i < count; i++) {
                if (strcmp(records[i].tag, tag) == 0) {
                    found = records[i].message;
                    break;
                }
            }
            if (found) {
                for (int k = 0; found[k]; k++) {
                    putchar(found[k] == ' ' ? '_' : found[k]);
                }
                putchar('\n');
            }
        } else if (strcmp(line, "REPORT") == 0) {
            for (int i = 0; i < count; i++) {
                printf("%s %s\n", records[i].tag, records[i].message);
            }
        }
    }
    free(line);
    for (int i = 0; i < count; i++) {
        free(records[i].tag);
        free(records[i].message);
    }
    free(records);
    return 0;
}
