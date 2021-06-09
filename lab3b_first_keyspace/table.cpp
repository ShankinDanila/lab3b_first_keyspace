#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "table.h"
#include "dialog.h"


Table* init_table() {

	Table* table = (Table*)calloc(1, sizeof(Table));
	table->Space1 = (KeySpace1*)calloc(MSIZE1, sizeof(KeySpace1));
	table->size_max1 = MSIZE1;
	table->space1_offset = 4 * sizeof(int);
	table->size_tab = 4 * sizeof(int) + table->size_max1 * sizeof(KeySpace1);
	table->cur_items = 0;
	
	return table;
}

Table* load_table(char* filename) {

	Table* table = (Table*)calloc(1, sizeof(Table));

	if (fopen_s(&(table->fd), filename, "r+b") != 0)
	{
		printf("Error opening file!\n");
		return NULL;
	}

	fseek(table->fd, 0, SEEK_SET);
	fread(&(table->size_tab), sizeof(int), 1, table->fd);
	fread(&(table->space1_offset), sizeof(int), 1, table->fd);
	fread(&(table->size_max1), sizeof(int), 1, table->fd);
	fread(&(table->cur_items), sizeof(int), 1, table->fd);

	fclose(table->fd);

	return table;
}

void* load_keyspace1(char* filename, Table* table) {

	if (fopen_s(&(table->fd), filename, "r+b") != 0)
	{
		printf("Error opening file!\n");
		return NULL;
	}

	table->Space1 = (KeySpace1*)calloc(MSIZE1, sizeof(KeySpace1));
	fseek(table->fd, table->space1_offset, SEEK_SET);

	char* buf_key1 = (char*)calloc(N, sizeof(char));
	int buf_busy;
	int buf_offset;

	for (int i = 0; i < table->size_max1; i++) {
		fread((buf_key1), sizeof(char), N, table->fd);
		fread(&(buf_busy), sizeof(int), 1, table->fd);
		fread(&(buf_offset), sizeof(int), 1, table->fd);


		strcpy_s(&(table->Space1[i].key1), N * sizeof(char), buf_key1);
		table->Space1[i].busy = buf_busy;
		table->Space1[i].item_offset = buf_offset;
	}

	fclose(table->fd);

	return;
}

void write_file(Table* table, char* key1, char* filename, char* string, double number) {

	if (fopen_s(&(table->fd), filename, "w+b") != 0)
	{
		printf("Error opening file!\n");

	}
	perror(strerror(errno));

	fseek(table->fd, 0, SEEK_SET);
	fwrite(&(table->size_tab), sizeof(int), 1, table->fd);
	fwrite(&(table->space1_offset), sizeof(int), 1, table->fd);
	fwrite(&(table->size_max1), sizeof(int), 1, table->fd);
	fwrite(&(table->cur_items), sizeof(int), 1, table->fd);

	/*for (int i = 0; i < table->size_max1; i++) {
		fseek(table->fd, 4*sizeof(int) + i*sizeof(KeySpace1), SEEK_SET);
		char key1[N];
		strcpy(key1, table->Space1[i].key1);
		int busy = table->Space1[i].busy;
		table->Space1[i].item_offset = table->size_tab;


		fwrite(&(key1), sizeof(char), N, table->fd);
		fwrite(&(busy), sizeof(int), 1, table->fd);
		int item_offset = table->Space1[i].item_offset;
		fwrite(&(item_offset), sizeof(int), 1, table->fd);

		fseek(table->fd, item_offset, SEEK_SET);

		fwrite(&(number), sizeof(double), 1, table->fd);
		int string_len = strlen(string) + 1;
		fwrite(&(string_len), sizeof(int), 1, table->fd);
		fwrite(string, string_len, sizeof(char), table->fd);

		table->size_tab += (sizeof(int) + sizeof(double) + string_len * sizeof(char));
	}*/


	int i = find_ks1(table, key1);

	fseek(table->fd, 4 * sizeof(int) + i * sizeof(KeySpace1), SEEK_SET);
	//char key1[N];
	strcpy(key1, table->Space1[i].key1);
	int busy = table->Space1[i].busy;
	table->Space1[i].item_offset = table->size_tab;


	fwrite(&(key1), sizeof(char), N, table->fd);
	fwrite(&(busy), sizeof(int), 1, table->fd);
	int item_offset = table->Space1[i].item_offset;
	fwrite(&(item_offset), sizeof(int), 1, table->fd);

	fseek(table->fd, item_offset, SEEK_SET);

	fwrite(&(number), sizeof(double), 1, table->fd);
	int string_len = strlen(string) + 1;
	fwrite(&(string_len), sizeof(int), 1, table->fd);
	fwrite(string, string_len, sizeof(char), table->fd);

	table->size_tab += (sizeof(int) + sizeof(double) + string_len * sizeof(char));


	int finish_size = table->size_tab;
	fseek(table->fd, 0, SEEK_SET);
	fwrite(&(finish_size), sizeof(int), 1, table->fd);


	fclose(table->fd);
}

void insert(Table* table, char* filename, char* string, double number) {
	
	if (fopen_s(&(table->fd), filename, "r+b") != 0)
	{
		printf("Error opening file!\n");
	}

	int item_offset = table->size_tab;
	fseek(table->fd, item_offset, SEEK_SET);

	fwrite(&(number), 1, sizeof(double), table->fd);
	int string_len = strlen(string);
	fwrite(&(string_len), 1, sizeof(int), table->fd);
	fwrite(string, string_len, sizeof(char), table->fd);

	table->cur_items += 1;
	table->size_tab += sizeof(Item);
}

int find_ks1(Table* table, char* key) {
	for (int i = 0; i < MSIZE1; i++) {
		if ((table->Space1[i].busy == 1) && (strcmp(table->Space1[i].key1, key) == 0)) {
			return i;
		}
	}
	return -1;
}

void reorganization(struct Table* table) {
	int count = 0;
	for (int i = 0; i < table->size_max1; i++) {
		if (table->Space1[i].busy == -1) {
			table->Space1[i].busy = 0;
		}
		if (table->Space1[i].busy == 1) {
			count += 1;
		}
	}

	for (int i = 0; i < table->size_max1; i++) {
		if (table->Space1[i].busy == 0) {
			for (int j = i; j < table->size_max1; j++) {
				if (table->Space1[j].busy == 1) {
					swap(&(table->Space1[i]), &(table->Space1[j]));
					break;
				}
			}
		}
	}
}

void swap(struct KeySpace1* x, struct KeySpace1* y) {
	struct KeySpace1 tmp = *x;
	*x = *y;
	*y = tmp;
	return;
}

int add_ks1(Table* table, char* key1, char* filename, char* string, double number) {
	for (int i = 0; i < table->size_max1; i++) {
		if (table->Space1[i].busy == 0) {
			table->Space1[i].busy = 1;
			table->cur_items += 1;
			strcpy(table->Space1[i].key1, key1);
			write_file(table, key1, filename, string, number);
			return 1;
		}
	}
	reorganization(table);
	for (int i = 0; i < table->size_max1; i++) {
		if (table->Space1[i].busy == 0) {
			table->Space1[i].busy = 1;
			strcpy(table->Space1[i].key1, key1);
			table->Space1[i].item_offset = table->size_tab;
			write_file(table, key1, filename, string, number);
			return 1;
		}
	}

	return 0;
}

int delete_ks1(Table* table, char* key) {
	if (find_ks1(table, key) < 0) {
		return -1; //элемента не найдено
	}
	int index = find_ks1(table, key);
	table->Space1[index].busy = -1;
	return 0; //удаление успешно
}

void print(Table* table) {
	FILE* file = table->fd;
	KeySpace1* space = table->Space1;
	printf("\nSpace1:\n");

	for (int i = 0; i < table->size_max1; i++) {
		double num;
		char* string;
		int str_len;
		int item_offset = table->Space1[i].item_offset;
		int busy = table->Space1[i].busy;
		char* key = (char*)calloc(N + 1, sizeof(char));
		strcpy(key, table->Space1[i].key1);

		fseek(table->fd, 4*sizeof(int) + i*sizeof(KeySpace1), SEEK_SET);

		/*fread((key), sizeof(char), N, table->fd);
		fread(&(busy), sizeof(int), 1, table->fd);
		fread(&(item_offset), sizeof(int), 1, table->fd);*/

		fseek(table->fd, item_offset, SEEK_SET);
		fread(&(num), sizeof(double), 1, table->fd);
		fread(&(str_len), sizeof(int), 1, table->fd);
		string = (char*)calloc(str_len, sizeof(char));
		fread(string, sizeof(char), str_len, table->fd);

		if (busy == 1) {
			printf("%s -> (%lf | %s)\n", key, num, string);
		}
	}
}