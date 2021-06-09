#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "dialog.h"
#include "table.h"

const char* MSGS0[] = { "0. Quit", "1. Create new table", "2. Load table" };

const int MSGS_SIZE0 = sizeof(MSGS0) / sizeof(MSGS0[0]);

Table* choise() {
    int c0;
    do {
        c0 = dialog(MSGS0, MSGS_SIZE0);
        switch (c0) {
        case 0: {
            break;
        }
        case 1: {
            Table* table = init_table();
            return table;
            break;
        }
        case 2: {
            Table* table = load_table("table_3b.bin");
            load_keyspace1("table_3b.bin", table);
            return table;
            break;
        }
        }
    } while (c0 != 0);
}

const char* MSGS1[] = { "0. Quit", "1. Search", "2. Delete element with key1", 
                        "3. Print the table", "4. Add the element" };

const int MSGS_SIZE1 = sizeof(MSGS1) / sizeof(MSGS1[0]);

int main() {
    int c;
    Table* table = choise();
    do {
        c = dialog(MSGS1, MSGS_SIZE1);
        switch (c) {
        case 0:
            break;
        case 1:
            puts("Enter key");
            char* key[N];
            scanf("%s", key);
            int index = find_ks1(table, key);
            if (index < 0) {
                puts("No such element in table");
            }
            if (fopen_s(&(table->fd), "table_3b.bin", "w+b") != 0)
            {
                printf("Error opening file!\n");

            }

            double num;
            int str_len;
            int buf_offset = table->Space1[index].item_offset;

            fseek(table->fd, buf_offset, SEEK_SET);
            fscanf(table->fd, "%lf", &num);
            fscanf(table->fd, "%d", &str_len);
            char* string = (char*)calloc(str_len + 1, sizeof(char));
            fscanf(table->fd, "%s", string);

            printf("%s -> %lf | %s", &key, num, string);

            break;
        case 2: {
            



            break;
        }
        case 3: {



            print(table);

            



            break;
        }
        case 4: {
            puts("Enter key");
            char key[N];
            scanf("%s", key);

            puts("Enter info number");
            double num;
            GetDouble(&num);

            puts("Enter info string");
            scanf("%*c");
            char* str = get_str();
            
            add_ks1(table, key, "table_3b.bin", str, num);

            break;
        }
        }

    } while (c != 0);


    //FREE_ALL_TABLE(table);
    return 0;
}