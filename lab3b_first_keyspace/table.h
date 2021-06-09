#define N 6
#define MSIZE1 25

typedef struct Item {
	//char key[N];
	double num;
	int str_len;
	char* str;
} Item;

typedef struct KeySpace1 {
	char key1[N];
	int busy;
	int item_offset;
} KeySpace1;

typedef struct Table {
	KeySpace1* Space1;

	int size_tab;

	int space1_offset;
	int size_max1;

	//int size_tab;

	int cur_items;

	FILE* fd;
} Table;

Table* init_table();
Table* load_table(char* filename);
void* load_keyspace1(char* filename, Table* table);
void write_file(Table* table, char* key1, char* filename, char* string, double number);
void insert(Table* table, char* filename, char* string, double number);
int find_ks1(Table* table, char* key);
void reorganization(struct Table* table);
void swap(struct KeySpace1* x, struct KeySpace1* y);
int add_ks1(Table* table, char* key1, char* filename, char* string, double number);
int delete_ks1(Table* table, char* key);
void print(Table* table);
