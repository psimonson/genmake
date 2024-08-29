/*
 * main - Generate a standard GNU Makefile for a project..
 *
 * Author: Philip R. Simonson
 * Date:   08/25/2024
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>

static int *is_lib;

/* Add an executable lib or exe.
 */
void add_exec(int **is_lib, int lib)
{
	static int count = 0;

	*is_lib = (int*)realloc(*is_lib, sizeof(int) * (count + 1));
	if (!(*is_lib)) {
		return;
	}

	(*is_lib)[count] = lib;
	count++;
}
/* Free is_lib array.
 */
void free_islib(int *is_lib)
{
	free(is_lib);
}
/* Get a string from the user.
 */
char *getstr(const char *prompt)
{
	static char input[256];
	int i;
	int c;

	printf("%s", prompt);
	for (i = 0; i < 256 && (c = getc(stdin)) != '\n' && c != EOF; i++) {
		if (c == '\b' && i > 0) {
			i -= 2;
		} else {
			input[i] = c;
		}
	}

	input[i] = '\0';
	return input;
}
/* Generate project Makefile from given directories.
 */
void generate(const char *dirname, int *count, FILE *fp)
{
	struct dirent *p;
	DIR *dir;

	if (!count) {
		fprintf(stderr, "Error: Count variable undefined... cannot continue.\n");
		return;
	}

	dir = opendir(dirname == NULL ? "." : dirname);
	if (!dir) {
		fprintf(stderr, "Error: Cannot open directory '%s' for reading.\n", dirname);
		return;
	}

	fprintf(fp, "SRC%d = ", *count);
	while ((p = readdir(dir))) {
		char *name = strstr(p->d_name, ".c");
		if (name && !strncmp(name, ".c", 3)) {
			char path[4096];
			memset(path, 0, sizeof(path));
			if (dirname == NULL) {
				snprintf(path, sizeof(path) - 1, "%s", p->d_name);
			} else {
				snprintf(path, sizeof(path) - 1, "%s/%s", dirname, p->d_name);
			}
			fprintf(fp, " %s", path);
		}
	}

	fprintf(fp, "\nOBJ%d = $(SRC%d:%%.c=%%.c.o)\n", *count, *count);

	int c = '\0';
	printf("Are you making a library (Y/N)? ");
	while (c != 'n' && c != 'N' && c != 'y' && c != 'Y') {
		c = getchar();
		while (getchar() != '\n');
		if (c != 'n' && c != 'N' && c != 'y' && c != 'Y') {
			printf("Are you making a library (Y/N)? ");
		}
	}

	if (c == 'y' || c == 'Y') {
		add_exec(&is_lib, 1);
		fprintf(fp, "LIB%d = ", *count);
	} else {
		add_exec(&is_lib, 0);
		fprintf(fp, "EXE%d = ", *count);
	}

	char *tmp = getstr("Enter program name: ");
	while (!strncmp(tmp, "", 1)) {
		tmp = getstr("Enter program name: ");
	}

	fprintf(fp, "%s\n\n", tmp);

	closedir(dir);
	(*count)++;
}
/* Entry point for simple Makefile generator.
 */
int main()
{
	char filename[256];
	char *tmp;
	FILE *fp;

	tmp = getstr("Enter filename [Makefile]: ");
	if (strncmp(tmp, "", 1)) {
		strncpy(filename, tmp, sizeof(filename) - 1);
	} else {
		strncpy(filename, "Makefile", sizeof(filename) - 1);
	}

	fp = fopen(filename, "wt");
	if (!fp) {
		fprintf(stderr, "Error: Cannot open '%s' for writing.\n", tmp);
		return 1;
	}

	tmp = getstr("Enter executable name (compiler): ");
	fprintf(fp, "CC = %s\n", tmp);

	tmp = getstr("Enter compiler flags:\n");
	fprintf(fp, "CFLAGS = %s\n", tmp);

	tmp = getstr("Enter linker flags:\n");
	fprintf(fp, "LDFLAGS = %s\n\nSRCDIR = $(shell basename $(shell pwd))\nDESTDIR ?= \nPREFIX ?= /usr\n\n", tmp);

	int count = 0;
	int c = '\0';

	while (c != 'n' && c != 'N') {
		c = '\0';
		printf("Do you want to enter a source directory (Y/N)? ");
		while (c != 'n' && c != 'N' && c != 'y' && c != 'Y') {
			c = getchar();
			while (getchar() != '\n');
			if (c != 'n' && c != 'N' && c != 'y' && c != 'Y') {
				printf("Do you want to enter a source directory (Y/N)? ");
			}
		}

		if (c == 'y' || c == 'Y') {
			tmp = getstr("Enter source directory (blank for current): ");
			if (!strncmp(tmp, "", 1)) {
				generate(NULL, &count, fp);
			} else {
				generate(tmp, &count, fp);
			}
			printf("Do you want to generate another (Y/N)? ");
			c = '\0';
			while (c != 'n' && c != 'N' && c != 'y' && c != 'Y') {
				c = getchar();
				while (getchar() != '\n');
				if (c != 'n' && c != 'N' && c != 'y' && c != 'Y') {
					printf("Do you want to generate another (Y/N)? ");
				}
			}
		} else {
			fprintf(fp, "SRC%d = ", count);
			tmp = getstr("Enter source files: ");
			while (!strncmp(tmp, "", 1)) {
				tmp = getstr("Enter source files: ");
			}
			fprintf(fp, "%s\nOBJ%d = $(SRC%d:%%.c=%%.c.o)\n", tmp, count, count);

			c = '\0';
			printf("Are you making a library (Y/N)? ");
			while (c != 'n' && c != 'N' && c != 'y' && c != 'Y') {
				c = getchar();
				while (getchar() != '\n');
				if (c != 'n' && c != 'N' && c != 'y' && c != 'Y') {
					printf("Are you making a library (Y/N)? ");
				}
			}

			if (c == 'y' || c == 'Y') {
				add_exec(&is_lib, 1);
				fprintf(fp, "LIB%d = ", count);
			} else {
				add_exec(&is_lib, 0);
				fprintf(fp, "EXE%d = ", count);
			}

			char *tmp = getstr("Enter program name: ");
			while (!strncmp(tmp, "", 1)) {
				tmp = getstr("Enter program name: ");
			}

			fprintf(fp, "%s\n\n", tmp);

			c = '\0';
			printf("Do you want to generate another (Y/N)? ");
			while (c != 'n' && c != 'N' && c != 'y' && c != 'Y') {
				c = getchar();
				while (getchar() != '\n');
				if (c != 'n' && c != 'N' && c != 'y' && c != 'Y') {
					printf("Do you want to generate another (Y/N)? ");
				}
			}
			count++;
		}
	}

	fprintf(fp, "all:");
	for (int i = 0; i < count; i++) {
		if (is_lib[i]) {
			fprintf(fp, " $(LIB%d)", i);
		} else {
			fprintf(fp, " $(EXE%d)", i);
		}
	}

	for (int i = 0; i < count; i++) {
		if (is_lib[i]) {
			fprintf(fp, "\n\n$(LIB%d): $(OBJ%d)\n", i, i);
			fprintf(fp, "\t$(AR) rcs $@ $^\n\n");
		} else {
			fprintf(fp, "\n\n$(EXE%d): $(OBJ%d)\n", i, i);
			fprintf(fp, "\t$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)\n\n");
		}
	}

	fprintf(fp, "%%.c.o: %%.c\n\t$(CC) $(CFLAGS) -c -o $@ $<\n\n");
	fprintf(fp, "clean:\n\trm -f");
	for (int i = 0; i < count; i++) {
		if (is_lib[i]) {
			fprintf(fp, " $(OBJ%d) $(LIB%d)", i, i);
		} else {
			fprintf(fp, " $(OBJ%d) $(EXE%d)", i, i);
		}
	}
	fprintf(fp, "\n\ninstall:\n\tcp");
	for (int i = 0; i < count; i++) {
		fprintf(fp, " $(EXE%d)", i);
	}
	fprintf(fp, " $(DESTDIR)$(PREFIX)/bin\n\nuninstall:\n\trm -f");
	for (int i = 0; i < count; i++) {
		fprintf(fp, " $(DESTDIR)$(PREFIX)/bin/$(EXE%d)", i);
	}
	fprintf(fp, "\n\ndist:\n\tcd .. && tar cvzf $(SRCDIR).tgz ./$(SRCDIR)\n\n");
	fclose(fp);

	fprintf(stderr, "Total directories processed: %d\n", count);
	if (count > 0) {
		fprintf(stderr, "Makefile generation done.\n");
	}
	free_islib(is_lib);

	return 0;
}
