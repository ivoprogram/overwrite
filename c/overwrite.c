
/*
TITLE:			Overwrite
DESCRIPTION:	Overwrite empty space on disk, write small files and data.
CODE:			github.com/ivoprogram/overwrite
LICENSE:		GNU General Public License v3.0 http://www.gnu.org/licenses/gpl.html
AUTHOR:			Ivo Gjorgjievski
WEBSITE:		ivoprogram.github.io
DATE:			2019-10-28
VERSION:		1.2

*/

// Include -----
#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <time.h>
#include <limits.h>
#include <ctype.h>


// Definitions -----
#define BLOCK_SIZE 4096				// Default block size, used in NTFS EXT4.
#define FILE_PREFIX "000000000-"	// File prefix for small temporary files.
#define VERSION "Version 1.2 2019-10-28"	// Program version


// Arguments structure
struct args {
	int res;				// Result from called function
	int test;				// For test, don't delete written files
	int one;				// Write one's
	int ran;				// Write random data
	int blocksize;			// Block size
	int files;				// Number of files to write
	char *data;				// Data to write, mb, gb, all
	char *path;				// Path where to write 
	char filepath[1024];	// Temp files path.
	char dirpath[1024];		// Directory path.
	unsigned char *blockbuf;	// Block buffer init with calloc.
};


// Headers -----
struct args parseargs(int argc, char *argv[]);
struct args prepargs(struct args argsv);
struct args writefiles(struct args argsv);
struct args writedata(struct args argsv);
struct args cleandata(struct args argsv);
int writefile(char *filepath, unsigned char *buf, int bufsize);
void freemem(struct args argsv);

void printtime(clock_t ticks);
void randbuf(unsigned char *buf, int bufsize);
int strstarti(const char *str, const char *substr);
int strendi(const char* str, const char* substr);
int strnicmp2(const char *str1, const char *str2, int len);

void errmsg(struct args argsv, char *message);
void exitmsg(struct args argsv, char *message);
void exiterr(struct args argsv, char *message);
void errcheckint(int res, struct args argsv, char *message);
void errcheckptr(void *res, struct args argsv, char *message);
void usage();



// main
int main(int argc, char *argv[])
{
	// Local vars
	struct args argsv;
	clock_t ticks;

	// Init 
	printf("\n");
	ticks = clock();

	// Parse, prepare arguments
	argsv = parseargs(argc, argv);
	argsv = prepargs(argsv);

	// Write files
	argsv = writefiles(argsv);
	//if(argsv.res){ errmsg(argsv, "Error writing files."); }

	// Write data
	argsv = writedata(argsv);
	//if(argsv.res){ errmsg(argsv, "Error writing data."); }

	// Clean
	freemem(argsv);
	if(!argsv.test){
		argsv = cleandata(argsv);
		//if(argsv.res){ errmsg(argsv, "Error cleaning data."); }
	}

	// Done
	ticks = clock() - ticks;
	printf("\n");
	printtime(ticks);
	printf("Done. \n");
	return 0;

}// 



// Parse arguments
struct args parseargs(int argc, char *argv[])
{
	// 
	int index = 0;
	int res = 0;
	struct args argsv;

	// init argsv
	argsv.res = 0;
	argsv.test = 0;
	argsv.one = 0;
	argsv.ran = 0;
	argsv.blocksize = BLOCK_SIZE;
	argsv.files = 0;
	argsv.data = NULL;
	argsv.path = NULL;
	argsv.blockbuf = NULL;
	strcpy(argsv.dirpath, "");
	strcpy(argsv.filepath, "");

	// usage
	if(argc < 2){ usage(); exit(0); }

	// parse args
	for(index = 0; index < argc; index++)
	{
		// 
		if(strstarti(argv[index], "-h") || strstarti(argv[index], "--help") ){ 
			usage();
			exit(0);
		}
		// 
		else if(strstarti(argv[index], "-v") || strstarti(argv[index], "--version") ){ 
			printf(VERSION "\n");
			exit(0);
		}
		else if(strstarti(argv[index], "-test")){ 
			argsv.test = 1; 
		}
		else if(strstarti(argv[index], "-one")){ 
			argsv.one = 1; 
		}
		else if(strstarti(argv[index], "-rand")){ 
			argsv.ran = 1; 
		}
		else if(strstarti(argv[index], "-block:")){
			argsv.blocksize = atoi(argv[index] + 7);
			if(argsv.blocksize == 0){ argsv.blocksize = BLOCK_SIZE; }
		}
		else if(strstarti(argv[index], "-files:")){
			if(strendi(argv[index], "all")){ argsv.files = INT_MAX; }
			else{ argsv.files = atoi(argv[index] + 7); }
		}
		else if(strstarti(argv[index], "-data:")){
			argsv.data = argv[index] + 6;
		}
		else if(strstarti(argv[index], "-path:")){
			argsv.path = argv[index] + 6;
		}

	}// for

	// Check for space in args
	for(index = 0; index < argc; index++)
	{
		if(strendi(argv[index], ":")){
			exitmsg(argsv, "Error, arguments contain space or ends with : ");
		}
	}

	// Files or data args are required
	if(argsv.files == 0 && argsv.data == NULL)
	{
		exitmsg(argsv, "Error, arguments -files: or -data: are missing.");
	}

	// Path arg is required
	if(argsv.path == NULL)
	{
		exitmsg(argsv, "Error, argument -path: is missing.");
	}

	argsv.res = 0;
	return argsv;

}//



// Prepare arguments, memory, directory
struct args prepargs(struct args argsv)
{
	// 
	int index = 0;
	int res = 0;

	// Initialize blockbuf with calloc, set with 0.
	argsv.blockbuf = (unsigned char*) calloc(argsv.blocksize, sizeof (unsigned char));
	errcheckptr(argsv.blockbuf, argsv, "Block buffer initialization error. ");

	// If one, set buffer with one's
	if(argsv.one){ memset(argsv.blockbuf, 255, argsv.blocksize);  }

	// If random set buffer with random data
	if(argsv.ran){ randbuf(argsv.blockbuf, argsv.blocksize); }

	// Prepare path
	strcat(argsv.dirpath, argsv.path);
	if(!strendi(argsv.dirpath, "/") && !strendi(argsv.dirpath, "\\")){
		strcat(argsv.dirpath, "/");
	}

	argsv.res = 0;
	return argsv;

}// 


// writefiles
struct args writefiles(struct args argsv)
{
	// 
	int index = 0;
	int res = 0;
	int cont = 1000;
	float stat = 0.1F;
	char stri[16];


	// If files 0 return
	if(!argsv.files){ argsv.res = 0; return argsv; }

	// Write files
	printf("Writing files: \n");
	for(index = 1; index <= argsv.files; index++)
	{
		// File path
		sprintf(stri, "%d", index);
		strcpy(argsv.filepath, argsv.dirpath);
		strcat(argsv.filepath, FILE_PREFIX);
		strcat(argsv.filepath, stri);

		// Writefile
		res = writefile(argsv.filepath, argsv.blockbuf, argsv.blocksize);
		if(res && index == 1){ errcheckint(res, argsv, "Error writing files."); }
		if(res){ 
			printf("100%% \n");
			argsv.files = index; 
			argsv.res = -1;
			return argsv; 
		}

		// Statistics
		if(argsv.files != INT_MAX && index > argsv.files * stat)
		{
			printf("%.0f%% ", stat * 100 );
			stat += 0.1F;

		}// if
		
		// Statistics for -files:all, up to 10.000 files.
		if(argsv.files == INT_MAX && index > cont && cont < 10000)
		{
			printf("%dth ", cont / 1000 );
			cont += 1000;

		}// if


	}// for

	printf("100%% \n");
	argsv.res = 0;
	return argsv;

}//


// writefile
int writefile(char *filepath, unsigned char *buf, int bufsize)
{
	int resi;
	size_t rest;

	FILE *fp = fopen(filepath, "wb");
	if(fp == NULL){ return -1; }

	rest = fwrite(buf, bufsize, 1, fp);
	if(rest != 1){ fclose(fp); return -1; }

	resi = fclose(fp);
	if(resi == EOF){ return -1; }

	return 0;

}// 


// writedata
struct args writedata(struct args argsv)
{
	int blocks = 0;
	int blocksmb = 0;
	int blocksgb = 0;
	int blockscont = 0;

	int cont = 0;
	int index = 0;
	int resi = 0;
	size_t rest = 0;
	double stat = 0.1;
	FILE *fp;

	// if data 0 return
	if(!argsv.data){ argsv.res = 0; return argsv; }

	// How many blocks to write, 1Mb / blocksize
	blocksmb = (1024 * 1000) / argsv.blocksize;
	blocksgb = (1024 * 1000 * 1000) / argsv.blocksize;
	blockscont = blocksgb;

	// Calculate blocks to write
	if(strendi(argsv.data, "mb")) { blocks = atoi(argsv.data) * blocksmb; }
	else if(strendi(argsv.data, "gb")) { blocks = atoi(argsv.data) * blocksgb; }
	else if(strstarti(argsv.data, "all")) { blocks = INT_MAX; }
	else{ argsv.res = 0; return argsv; }

	// Open file
	strcpy(argsv.filepath, argsv.dirpath);
	strcat(argsv.filepath, FILE_PREFIX);
	strcat(argsv.filepath, "0");
	fp = fopen(argsv.filepath, "wb");
	if(fp == NULL){ argsv.res = -1; return argsv; }

	// Write data
	printf("Writing data: \n");
	for(index = 0; index < blocks; index++)
	{
		// write block
		rest = fwrite(argsv.blockbuf, argsv.blocksize, 1, fp);
		if(rest != 1){ 
			printf("100%% \n");
			fclose(fp);
			argsv.res = -1; 
			return argsv;
		}

		// Statistics
		if(blocks != INT_MAX && index > blocks * stat)
		{
			printf("%.0f%% ", stat * 100 );
			stat += 0.1;

		}// if

		// Statistics for -data:all argument, up to 10gb
		if(blocks == INT_MAX && index >= blockscont && cont < 10)
		{
			cont++;
			blockscont += blocksgb;
			printf("%dGb ", cont );
		}

	}// for

	printf("100%% \n");

	// close file
	fclose(fp);
	argsv.res = 0;
	return argsv;

}// 


// cleandata
struct args cleandata(struct args argsv)
{
	// 
	int index;
	int res;
	float stat = 0.1F;
	char stri[16];

	//
	printf("Cleaning: \n");

	// Delete data file if exists
	strcpy(argsv.filepath, argsv.dirpath);
	strcat(argsv.filepath, FILE_PREFIX);
	strcat(argsv.filepath, "0");
	remove(argsv.filepath);
	
	// Delete files
	for(index = 1; index <= argsv.files; index++)
	{
		// filepath
		sprintf(stri, "%d", index);
		strcpy(argsv.filepath, argsv.dirpath);
		strcat(argsv.filepath, FILE_PREFIX);
		strcat(argsv.filepath, stri);

		// delete file
		res = remove(argsv.filepath);
		if(res != 0){ argsv.res = -1; return argsv; }
		//errcheckint(res, argsv, "Error deleting file.");

		// Statistics
		if(index > argsv.files * stat)
		{
			printf("%.0f%% ", stat * 100 );
			stat += 0.1F;

		}// if

	}// for

	// 
	printf("100%% \n");
	argsv.res = 0; 
	return argsv;

}//


// Free memory 
void freemem(struct args argsv)
{
	errno = 0;

	if(argsv.blockbuf != NULL){
		free(argsv.blockbuf);
		argsv.blockbuf = NULL;
	}

	if(errno != 0){
		printf("Error on free(): %d %s \n", errno, strerror(errno));
	}

}// 



// Util functions ---------------------


// printtime
void printtime(clock_t ticks)
{
	float total = (float) ticks / CLOCKS_PER_SEC;

	int hours = (int)total / 60 / 60;
	int minutes = (int)total / 60 % 60;
	int seconds = (int)total % 60;

	int intpart = (int)total;
	float decpart = total - intpart;
	float milisec = decpart * 1000;

	printf("Time: %02d:%02d:%02d.%.0f \n", hours, minutes, seconds, milisec);

}// 


// Set buffer with random data
void randbuf(unsigned char *buf, int bufsize)
{
	int index;
	srand((unsigned)time(NULL));

	for(index = 0; index < bufsize - 1; index++)
	{
		buf[index] = rand() % 256; // 
	}
}


// String starts with, case insensitive, 0 = false, 1 = true
int strstarti(const char *str, const char *substr)
{
	return strnicmp2(str, substr, strlen(substr) - 1);
}


// String ends with, case insensitive, 0 = false, 1 = true
int strendi(const char* str, const char* substr){
	return strnicmp2(str + strlen(str) - strlen(substr), substr, strlen(substr));
}


// Compare strings case insensitive, windows linux, 0 = false, 1 = true
int strnicmp2(const char *str1, const char *str2, int len)
{
	int a = 0, b = 0;

	for(a = 0; a < len, str1[a], str2[a]; a++)
	{
		if(str1[a]-str2[a] == 0 || str1[a]-str2[a] == 32 || str1[a]-str2[a] == -32) 
		{ b++; }
	}

	return a == b;

}// 


// Print error message, do not exit
void errmsg(struct args argsv, char *message)
{
	printf("Error %d, %s. \n%s \n", errno, strerror(errno), message);
}// 



// Exit with message
void exitmsg(struct args argsv, char *message)
{
	printf("%s \n", message);
	freemem(argsv);
	exit(0);

}// 



// Exit with error message
void exiterr(struct args argsv, char *message)
{
	printf("Error %d, %s. \n%s \n", errno, strerror(errno), message);
	freemem(argsv);
	exit(errno);

}// 


// Check error integer
void errcheckint(int res, struct args argsv, char *message)
{
	if(res == -1){ exiterr(argsv, message); }
}// 


// Check error pointer
void errcheckptr(void *res, struct args argsv, char *message)
{
	if(res == NULL){ exiterr(argsv, message); }
}// 


// 
void usage()
{
	printf(
		"Overwrite empty space on disk, write small files and data. \n\n"
		"USAGE: \n"
		"overwrite [-h -v -test -one -rand -block:] (-files: &| -data:) -path: \n"
		"  -h \t\t Print help and usage message. \n"
		"  -v \t\t Print program version. \n"
		"  -test \t For test, don't delete written files. \n"
		"  -one \t\t Overwrite with 1, default with 0. \n"
		"  -rand \t Overwrite with random data, default with 0. \n"
		"  -block \t Block size, default 4096 bytes as NTFS and EXT4. \n"
		"  -files \t Number of files to write block size each, ex: 10, all. \n"
		"  -data \t Quantity of data to write, ex: 1mb, 1gb, all. \n"
		"  -path \t Path to directory where to write files. \n\n"
		"EXAMPLE: \n"
		"Write 10 files and 10Mb data, on Windows \n"
		"overwrite -files:10 -data:10mb -path:c:\\temp\\ \n\n"
		"Write random data, block size 512 bytes on Linux \n"
		"overwrite -rand -block:512 -files:10 -path:/mnt/usbdisk/ \n\n"
		"Copyright GPLv3 http://github.com/ivoprogram/overwrite \n"

		);

}// 

