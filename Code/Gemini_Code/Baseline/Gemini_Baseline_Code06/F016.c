// F016.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *read_line(FILE *fp) {
    size_t size = 128;
    size_t len = 0;
    char *buf = malloc(size);
    if (!buf) return NULL;
    int c;
    while ((c = fgetc(fp)) != EOF && c != '\n') {
        if (len + 1 >= size) {
            size *= 2;
            char *new_buf = realloc(buf, size);
            if (!new_buf) {
                free(buf);
                return NULL;
            }
            buf = new_buf;
        }
        buf[len++] = c;
    }
    if (len == 0 && c == EOF) {
        free(buf);
        return NULL;
    }
    buf[len] = '\0';
    return buf;
}

typedef struct {
    char *name;
    char *email;
    char *phone;
} CSVContact;

int main(int argc, char *argv[]) {
    CSVContact *contacts = NULL;
    int count = 0;
    if (argc > 1) {
        FILE *fp = fopen(argv[1], "r");
        if (fp) {
            while (1) {
                char *line = read_line(fp);
                if (!line) break;
                char name[128], email[128], phone[128];
                if (sscanf(line, "%127[^,],%127[^,],%127[^,\n]", name, email, phone) == 3) {
                    contacts = realloc(contacts, (count + 1) * sizeof(CSVContact));
                    contacts[count].name = strdup(name);
                    contacts[count].email = strdup(email);
                    contacts[count].phone = strdup(phone);
                    count++;
                }
                free(line);
            }
            fclose(fp);
        }
    }
    while (1) {
        char *line = read_line(stdin);
        if (!line) break;
        if (strcmp(line, "END") == 0) {
            free(line);
            break;
        }
        if (strncmp(line, "ADD ", 4) == 0) {
            char name[128], email[128], phone[128];
            if (sscanf(line + 4, "%127[^,],%127[^,],%127[^,\n]", name, email, phone) == 3) {
                contacts = realloc(contacts, (count + 1) * sizeof(CSVContact));
                contacts[count].name = strdup(name);
                contacts[count].email = strdup(email);
                contacts[count].phone = strdup(phone);
                count++;
            }
        } else if (strncmp(line, "DELETE ", 7) == 0) {
            char *name = line + 7;
            for (int i = 0; i < count; i++) {
                if (strcmp(contacts[i].name, name) == 0) {
                    free(contacts[i].name);
                    free(contacts[i].email);
                    free(contacts[i].phone);
                    for (int j = i; j < count - 1; j++) {
                        contacts[j] = contacts[j + 1];
                    }
                    count--;
                    contacts = realloc(contacts, count * sizeof(CSVContact));
                    break;
                }
            }
        } else if (strncmp(line, "FIND ", 5) == 0) {
            char *name = line + 5;
            for (int i = 0; i < count; i++) {
                if (strcmp(contacts[i].name, name) == 0) {
                    printf("%s,%s,%s\n", contacts[i].name, contacts[i].email, contacts[i].phone);
                    break;
                }
            }
        } else if (strcmp(line, "REPORT") == 0) {
            for (int i = 0; i < count; i++) {
                printf("%s,%s,%s\n", contacts[i].name, contacts[i].email, contacts[i].phone);
            }
        }
        free(line);
    }
    for (int i = 0; i < count; i++) {
        free(contacts[i].name);
        free(contacts[i].email);
        free(contacts[i].phone);
    }
    free(contacts);
    return 0;
}
