#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

//struct for an entry
typedef struct {
	char* title;
	char* date;
	char* director;
	int id;
} entry;

//array holding all entries
int numEntries = 0;
int maxSize = 10;
entry* db;
entry* dbSort;

//file pointers
FILE * dbFile;
FILE * cmdFile;
FILE * outputFile;

//helper function for trimming leading whitespace from a string
char * trim(char * str) {
	while(isspace((unsigned char)*str)) str++;
	return str;
}

//check if a date string is valid
int isValidDate(char * date) {
	int month = atoi(strtok(strdup(date), "/"));
	int day = atoi(strtok(NULL, "/"));
	int year = atoi(strtok(NULL, "/"));
	if ((year < 0) || (year > 9999))
		return 0;
	if ((month < 1) || (month > 12))
		return 0;
	if (day < 1)
		return 0;
	int maxDay = 0;
	if ((month == 1) || (month == 3) || (month == 5) || (month == 7) || (month == 8) ||
	(month == 10) || (month == 12)) {
		maxDay = 31;
	} else if ((month == 4) || (month == 6) || (month == 9) || (month == 11)) {
		maxDay = 30;
	} else if (month == 2) {
		if ((year % 4) == 0)
			maxDay = 29;
		else
			maxDay = 28;
	}
	if (day > maxDay)
		return 0;
	return 1;
}

//add an entry to the database
void addEntry(char* title, char* date, char* director, int id) {
	int i;
	for (i = 0; i < numEntries; i++) {
		if (id == db[i].id)
			return;
	}
	if (!isValidDate(date))
		return;
	db[numEntries].title = strdup(title);
	db[numEntries].date = strdup(date);
	db[numEntries].director = strdup(director);
	db[numEntries].id = id;
	numEntries++;
	if (numEntries >= maxSize) {
		maxSize = maxSize * 2;
		db = (entry *) realloc(db, maxSize * sizeof(entry));
	}
}

//edit an entry
void editEntry(int id, char* feature, char* data) {
	if (!strcmp(feature, "DATE") && !isValidDate(data))
		return;
	int i;
	for (i = 0; i < numEntries; i++) {
		if (db[i].id == id) {
			if (!strcmp(feature, "TITLE"))
				db[i].title = strcpy(db[i].title, data);
			else if (!strcmp(feature, "DATE"))
				db[i].date = strcpy(db[i].date, data);
			else if (!strcmp(feature, "DIRECTOR"))
				db[i].director = strcpy(db[i].director, data);
		}
	}
}

//remove an entry
void removeEntry(int id) {
	int i;
	for (i = 0; i < numEntries; i++) {
		if (db[i].id == id) {
			int j;
			numEntries--;
			for (j = i; j < numEntries; j++) {
				db[j] = db[j + 1];
			}
		}
	}
	if (numEntries < (maxSize / 2)) {
		maxSize = maxSize / 2;
		db = (entry *) realloc(db, maxSize * sizeof(entry));
	}
}

//wildcards
int matches(char* search, char* str) {
	if (search[0] == '*')
		search++;
	if (search[strlen(search) - 1] == '*')
		search[strlen(search) - 1] = '\0';
	if (strstr(str, search) == NULL)
		return 0;
	else
		return 1;
}

//lookup
void lookup(char* line, char* feature, char* data) {
	fprintf(outputFile, "%s\n", line);
	int i;
	for (i = 0; i < numEntries; i++) {
		if ((!strcmp(feature, "TITLE") && matches(data, db[i].title)) ||
		(!strcmp(feature, "DATE") && matches(data, db[i].date)) ||
		(!strcmp(feature, "DIRECTOR") && matches(data, db[i].director))) {
			fprintf(outputFile, "%s, ", db[i].title);
			fprintf(outputFile, "%s, ", db[i].date);
			fprintf(outputFile, "%s, ", db[i].director);
			fprintf(dbFile, "%d\n", db[i].id);
		}
	}
}

//copy array db to dbSort
void copyArray() {
	dbSort = malloc(sizeof(entry) * maxSize);
	int i;
	for (i = 0; i < numEntries; i++) {
		dbSort[i].title = strdup(db[i].title);
		dbSort[i].date = strdup(db[i].date);
		dbSort[i].director = strdup(db[i].director);
		dbSort[i].id = db[i].id;
	}
}

//compare dates
int compareDates(char * d1, char * d2) {
	int month1  = atoi(strtok(strdup(d1), "/"));
	int day1 = atoi(strtok(NULL, "/"));
	int year1 = atoi(strtok(NULL, "/"));
	int month2  = atoi(strtok(strdup(d2), "/"));
	int day2 = atoi(strtok(NULL, "/"));
	int year2 = atoi(strtok(NULL, "/"));
	if (year1 > year2) 
		return 1;
	else if (year1 < year2)
		return -1;
	else {
		if (month1 > month2)
			return 1;
		else if (month1 < month2)
			return -1;
		else {
			if (day1 > day2)
				return 1;
			else if (day1 < day2)
				return -1;
			else
				return 0;
		}
	}
	return 0;
}

//swap two elements of entry array
void swap(entry * array, int i, int j) {
	entry temp = array[i];
	array[i] = array[j];
	array[j] = temp;
}

//Sort dbSort as specified
void sort(char * feature, int order) {
	//sort by id first
	int i, j;
	for (i = 0; i < numEntries - 1; i++) {
		for (j = i + 1; j < numEntries; j++) {
			if (((order == 0) && (dbSort[i].id > dbSort[j].id)) || ((order != 0) && (dbSort[i].id < dbSort[j].id)))
				swap(dbSort, i, j);
		}
	}

	if (!strcmp(feature, "TITLE")) {
		for (i = 0; i < numEntries - 1; i++) {
			for (j = i + 1; j < numEntries; j++) {
				if (((order == 0) && (strcmp(dbSort[i].title, dbSort[j].title) > 0)) ||
				((order != 0) && (strcmp(dbSort[i].title, dbSort[j].title) < 0)))
					swap(dbSort, i, j);
			}
		}
	}

	if (!strcmp(feature, "DATE")) {
		for (i = 0; i < numEntries - 1; i++) {
			for (j = i + 1; j < numEntries; j++) {
				if (((order == 0) && (compareDates(dbSort[i].date, dbSort[j].date) > 0)) ||
				((order != 0) && (compareDates(dbSort[i].date, dbSort[j].date) < 0)))
					swap(dbSort, i, j);
			}
		}
	}

	if (!strcmp(feature, "DIRECTOR")) {
		for (i = 0; i < numEntries - 1; i++) {
			for (j = i + 1; j < numEntries; j++) {
				if (((order == 0) && (strcmp(dbSort[i].director, dbSort[j].director) > 0)) ||
				((order != 0) && (strcmp(dbSort[i].director, dbSort[j].director) < 0)))
					swap(dbSort, i, j);
			}
		}
	}
}

//display
void display(char* line, char* feature, int order, int max) {
	fprintf(outputFile, "%s\n", line);
	copyArray();
	sort(feature, order);
	int i;
	if (max > numEntries)
		max = numEntries;
	for (i = 0; i < max; i++) {
		fprintf(outputFile, "%s, ", dbSort[i].title);
		fprintf(outputFile, "%s, ", dbSort[i].date);
		fprintf(outputFile, "%s, ", dbSort[i].director);
		fprintf(dbFile, "%d\n", dbSort[i].id);
	}
}

//parse a line, call addEntry
void parseEntry(char* line) {
	char *token;
	token = strtok(strdup(line), ",");
	char * title = strdup(trim(token));
	token = strtok(NULL, ",");
	char * date = strdup(trim(token));
	token = strtok(NULL, ",");
	char * director = strdup(trim(token));
	token = strtok(NULL, ",");
	int id = atoi(token);
	addEntry(title, date, director, id);
}

//get a single line from a file
char * readLine(FILE * fp) {
	char line[512];
	char ch = getc(fp);
	int count = 0;
	while ((ch != '\n') && (ch != EOF)) {
		line[count] = ch;
		count++;
		ch = getc(fp);
	}
	line[count] = '\0';
	return strdup(line);
}

//load the database from the file
void readDataBase(char * path) {
	dbFile = fopen(path, "r");
	char * line = readLine(dbFile);
	while ((line[0] != EOF) && (line[0] != '\0')) {
		parseEntry(line);
		line = readLine(dbFile);
	}
	fclose(dbFile);
}

//write the database to the file
void writeDataBase(char * path) {
	dbFile = fopen(path, "w+");
	int i;
	for (i = 0; i < numEntries; i++) {
		printf("%d", i);
		fprintf(dbFile, "%s, ", db[i].title);
		fprintf(dbFile, "%s, ", db[i].date);
		fprintf(dbFile, "%s, ", db[i].director);
		fprintf(dbFile, "%d\n", db[i].id);
	}
	fclose(dbFile);
}

//parse a line, call appropriate command
void parseCommand(char* line) {
	char * cmd = strtok(strdup(line), ",");
	if (!strcmp(cmd, "ADD")) {
		char * title = strdup(trim(strtok(NULL, ",")));
		char * date = strdup(trim(strtok(NULL, ",")));
		char * director = strdup(trim(strtok(NULL, ",")));
		int id = atoi(strtok(NULL, ","));
		addEntry(title, date, director, id);
	} else if (!strcmp(cmd, "EDIT")) {
		int id = atoi(strtok(NULL, ","));
		char * feature = strdup(trim(strtok(NULL, ",")));
		char * data = strdup(trim(strtok(NULL, ",")));
		editEntry(id, feature, data);
	} else if (!strcmp(cmd, "REMOVE")) {
		int id = atoi(strtok(NULL, ","));
		removeEntry(id);
	} else if (!strcmp(cmd, "LOOKUP")) {
		char * feature = strdup(trim(strtok(NULL, ",")));
		char * data = strdup(trim(strtok(NULL, ",")));
		lookup(line, feature, data);
	} else if (!strcmp(cmd, "DISPLAY")) {
		char * feature = strdup(trim(strtok(NULL, ",")));
		int order = atoi(strtok(NULL, ","));
		int max = atoi(strtok(NULL, ","));
		display(line, feature, order, max);
	}
}

//read commands from file
void readCommands(char * path) {
	cmdFile = fopen(path, "r");
	char * line = readLine(cmdFile);
	while ((line[0] != EOF) && (line[0] != '\0')) {
		parseCommand(line);
		line = readLine(cmdFile);
	}
	fclose(cmdFile);
}

//main
int main(int argc, char ** argv) {
	db = (entry *) malloc(maxSize * sizeof(entry));

	if (argc != 4)
		return -1;
	
	readDataBase(argv[1]);
	outputFile = fopen(argv[3], "w+");
	readCommands(argv[2]);
	fclose(outputFile);
	writeDataBase(argv[1]);

	free(db);
}
