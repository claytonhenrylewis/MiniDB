Homework 12: MiniDB
Author: Clayton Lewis, lewis323@purdue.edu

Files:
1. README.txt: Description of work
2. miniDB.c: C program, a miniature database parser
3. Makefile: Compiles miniDB.c into an executable file, miniDB

---------------------
1 Description of Work
---------------------
1.1 Summary
-----------
In my implementation of miniDB, I used a dynamically sized array to hold the
database entries. I wrote several functions which parsed the entries from the
database file and added them to the array, and which parsed commands from the
commands file and executed them. Full description of these functions is found
below.

----------------------------
1.2 Data and Data Structures
----------------------------
struct entry: This struct holds the data for one database entry. Each entry
contains a title, date, and director (each represented by char pointers), and
a unique ID (represented by an int).

db: This is an array of entries. It holds all entries added to the database
while the program is running. It is dynamically sized to accomodate an unknown
number of entries. The int variables numEntries and maxSize are used to
record and adjust the array's size as necessary.

numEntries: Holds the number of entries currently contained in db

maxSize: Holds the current size of db and the number of entries it can hold
before being resized.

dbSort: This is another array of entries. When the DISPLAY command is run, the
database array (db) is copied into dbSort and then sorted as specified by the
user. This separate array is necessary because it is desirable to retain the
order of addition in the database array.

-------------
1.3 Functions
-------------
1.3.1 Summary
-------------
I have organized the functions into groups for ease of understanding.

Main Function: Driver, calls all other functions.
Includes: main

File IO: Functions which read or write from or to given files. These are
called directly by main and call parser functions to process read data.
Includes: readDataBase, readCommands, writeDataBase, readLine

Parsers: Functions which interpret data from a file and translate that
information into database operations. These are called by the File IO funtions
and call database command functions.
Includes: parseEntry, parseCommand

Database Commands: These functions access or manipulate the database entries.
These are called by the parser functions.
Includes: addEntry, editEntry, removeEntry, lookup, display

Helper Functions: These are other functions used at various points in the
program. Each function executes a common task which may be repeated in the
program.
Includes: trim, isValidDate, compareDates, copyArray, sort, swap, matches

-------------------
1.3.2 Main Function
-------------------
int main(int argc, char** argv):
	Takes string arguments when run from the command line. The first element of 
argv should be the filename of the database file. The second should be the
filename of the commands file. The third should be the filename of the output
file.
	The main function allocates enough memory for db to hold (maxSize)
entries. It can be resized later. Main calls readDataBase to read and store
the entries from the database file. It then opens the output file. It then
calls readCommands to read and execute the commands from the commands file.
Output is written to the output file when appropriate. The output file is
closed. Lastly, main calls writeDataBase to update the database file with all
changes made during the program.

-------------
1.3.3 File IO
-------------
void readDataBase(char * path):
	Takes the path to the database file as input.
	This function opens the database file. It calls readLine to get each line
as a string, and parseEntry to interpret each string and add the data as an
entry to the database. After reading all lines, it closes the file.

void readCommands(char * path):
	Takes the path to the commands file as input.
	This function opens the commands file. It calls readLine to get each line
as a string, and parseCommand to interpret each string and execute the
appropriate command. After reading all lines, it closes the file.

void writeDataBase(char * path):
	Takes the path to the database file as input.
	This function opens the database file. For each entry in the database
array, it writes that entry to the file as a set of characters separated by
commas. Each entry is written on a new line. After writing all entries, it
closes the file.

char * readLine(FILE * fp):
	Takes a pointer to an open file as input.
	This function reads the next line from the given file. It stores each
character (starting from where the pointer currently is in the file) until it
finds a newline or EOF character. It returns a copy of this set of characters.

-------------
1.3.4 Parsers
-------------
void parseEntry(char * line):
	Takes a string which contains a line from the database file as input.
	This function splits the line by commas using the standard string function
strtok. It finds the entry's title, date, director, and id, and passes this
information to addEntry, which will add the entry to the database.

void parseCommand(char * line):
	Takes a string which contains a line from the commands file as input.
	This function splits the line by commas using the standard string function
strtok. The first token denotes which command to run. This token is checked,
and the rest of the line is interpreted and the appropriate function is called
on the gathered data based on which command should be run. The commands and
associated functions are shown below:
	ADD: addEntry
	EDIT: editEntry
	REMOVE: removeEntry
	LOOKUP: lookup
	DISPLAY: display

-----------------------
1.3.5 Database Commands
-----------------------
void addEntry(char * title, char * date, char * director, int id):
	Takes the title, date, director, and ID of the entry to add as input.
	This function first checks if the given ID already exists in the db array.
It also checks if the given date is invalid. If either of these are true, the
function returns immediately. If not, it proceeds to add the given entry. The
title, date, and director fields of the next empty entry in db are filled with
copies of the given strings. The id field is filled with the given id. Then
the number of entries (numEntries) is incremented. If the end of the db array
is reached, the array is resized so that more entries can be added.

void editEntry(int id, char * feature, char * data):
	Takes the id, feature, and data of the entry to change as input. The user
wants to change the (feature) of the entry identified by (id) to (data).
	This function first checks if the feature is DATE and the given date is
invalid. If so, it returns immediately. If not, the function loops through the
db array to find an entry which matches the given id. If it finds one, it then
checks the given feature to find which feature should be changed. It changes
that feature to a copy of the string data.

void removeEntry(int id):
	Takes the ID of the entry to remove as input.
	This function searches the db array until it finds an entry with an id
matches the given id. If it does, it shifts every entry after it forward one.
It also decrements numEntries, the number of entries in the array. This means
that there will be extra entries beyond numEntries that are not accessed.
After removing the entry, if numEntries is less than half the size of the
array, the size of the array is cut in half to save space.

void lookup(char * line, char * feature, char * data):
	Takes the full line from the commands file, the feature to lookup, and the
desired value of that feature.
	This function prints the given line to the output file. It then loops
through the db array and checks if the specified feature of each entry matches
the given data, using the function matches to deal with wildcards. If so, it
prints that entry to the output file as a comma-separated string.

void display(char * line, char * feature, int order, int max):
	Takes the full line from the command file, the feature to sort by, a flag
specifying ascending/descending, and the max number of entries to display as
input.
	This function prints the given line to the output file. It then calls the
function copyArray to copy the array db into the array dbSort. It then passes
the given feature and order to the function sort, which sorts dbSort
accordingly. If max is greater than numEntries, the value of max is reset to
that of numEntries. The function then loops through the dbSort array until it
reaches max. It prints each entry to the output file as a comma-separated
string.

----------------------
1.3.6 Helper Functions
----------------------
char * trim(char * str):
	Takes a string (character pointer) as input.
	This function trims leading whitespace from str by incrementing the
pointer until the first non-space character. This function is used by the
parser functions to eliminate the spaces between items found in csv files.

int isValidDate(char * date):
	Takes a date in the format "MM/DD/YYYY" as input.
	This function splits the date into the month, day, and year using the
standard string function strtok. It then checks that the year is four digits,
the month is between 1 and 12, and the day is in an appropriate range
depending on the month. If any of these checks fail, the function returns 0,
indicating an invalid date. Otherwise, the function returns 1, indicating a
valid date.

int compareDates(char * d1, char * d2):
	Takes two dates in the format "MM/DD/YYYY" as input.
	This function splits each date into the month, day and year using the
standard string function strtok. It then compares the years, then months, then
days to find out which date is later. The function returns 1 if the first date
is later, -1 if the second date is later, or 0 if they are the same date. 

void copyArray():
	This function allocates memory for dbSort, the current size of db. It then
loops through db and copies each element to dbSort. This ensures that dbSort
contains every entry currently in the database.

void sort(char * feature, int order):
	Takes the feature to sort by and the ascending/descending flag as input.
	This function first sorts the array dbSort by id. This ensures that when
the specified feature of two or more entries is the same, they are sorted by
id. It then checks the feature and sorts dbSort according to that feature, in
ascending or descending order depending on the order flag. All sorts are
bubble sorts and use the function swap to swap elements that are out of order.
The date sort uses compareDates to compare the dates of each pair of entries.

void swap(entry * array, int i, int j):
	Takes an entry array and the positions of the entries to swap as input.
	This function swaps the elements of the array at positions i and j. It's
used in the sort function.

int matches(char * search, char * str):
	Takes the string to search for and the string to find it in as input.
	This function first removes asterisks from the beginning and end of the
char array search, which represent wildcards. It then uses the standard string
function strstr to check if search exists within str. If so, the function
returns 1 (true). If not, it returns 0 (false).
