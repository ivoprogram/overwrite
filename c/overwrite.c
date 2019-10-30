
/*
TITLE:			Overwrite
DESCRIPTION:	Overwrite empty space on disk, metadata and data.
CODE:			github.com/ivoprogram/overwrite
LICENSE:		GNU General Public License v3.0 http://www.gnu.org/licenses/gpl.html
AUTHOR:			Ivo Gjorgjievski
WEBSITE:		ivoprogram.github.io
DATE:			2019-10-30
VERSION:		1.3

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


#ifdef _WIN32	// _WINDOWS
// _mkdir, _rmdir windows
#include <direct.h>
#else
// mkdir, rmdir, linux
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#endif


// Definitions -----
#define BLOCK_SIZE 4096		// Default block size, used in NTFS EXT4.
#define FILE_PREFIX "0"		// File prefix for temporary files.
#define VERSION "Version 1.3 2019-10-30"	// Program version


// Arguments structure
struct args {
	int res;				// Result from called function
	int test;				// For test, don't delete written files
	int one;				// Write one's
	int ran;				// Write random data
	int blocksize;			// Block size
	int dirs;				// Number of directories to write
	char *data;				// Quantity of data to write, mb, gb, all
	char *path;				// Path where to write 
	char dirpath[1024];		// Directory path.
	char filepath[1024];	// Temp files path.
	unsigned char *blockbuf;	// Block buffer init with calloc.
};


// Headers -----
struct args parseargs(int argc, char *argv[]);
struct args prepargs(struct args argsv);
struct args writedirs(struct args argsv);
struct args writedata(struct args argsv);
struct args cleandirs(struct args argsv);
struct args cleandata(struct args argsv);
int writefile(char *filepath, unsigned char *buf, int bufsize);
void freemem(struct args argsv);

int makedir(const char* name);
int remdir(const char* name);
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

	// Write dirs
	argsv = writedirs(argsv);
	if(!argsv.test){ cleandirs(argsv); }

	// Write data
	argsv = writedata(argsv);
	if(!argsv.test){ cleandata(argsv); }

	// Clean memory
	freemem(argsv);

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
	argsv.dirs = 0;
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
		else if(strstarti(argv[index], "-dirs:")){
			argsv.dirs = atoi(argv[index] + 6);
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

	// Dirs or data args are required
	if(argsv.dirs == 0 && argsv.data == NULL)
	{
		exitmsg(argsv, "Error, arguments -dirs: or -data: are missing.");
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


// writedirs
struct args writedirs(struct args argsv)
{
	// 
	int index = 0;
	int res = 0;
	float stat = 0.1F;
	char stri[16];


	// If files 0 return
	if(!argsv.dirs){ argsv.res = 0; return argsv; }

	// Write dirs
	printf("Writing directories: \n");
	for(index = 1; index <= argsv.dirs; index++)
	{
		// Dir path
		sprintf(stri, "%d", index);
		strcpy(argsv.filepath, argsv.dirpath);
		strcat(argsv.filepath, FILE_PREFIX);
		strcat(argsv.filepath, stri);

		// Write dir
		res = makedir(argsv.filepath);
		if(res && index == 1)
		{ errcheckint(res, argsv, "Error writing directories."); }
		if(res){ 
			printf("100%% \n");
			argsv.dirs = index;
			argsv.res = -1;
			return argsv; 
		}

		// Statistics
		if(argsv.dirs != INT_MAX && index > argsv.dirs * stat)
		{
			printf("%.0f%% ", stat * 100 );
			stat += 0.1F;

		}// if

	}// for

	printf("100%% \n");
	argsv.res = 0;
	return argsv;

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
	if(argsv.data == 0){ argsv.res = 0; return argsv; }

	// How many blocks to write, 1Mb / blocksize
	blocksmb = (1024 * 1000) / argsv.blocksize;
	blocksgb = (1024 * 1000 * 1000) / argsv.blocksize;
	blockscont = blocksgb;

	// Calculate blocks to write
	if(strendi(argsv.data, "mb")) { blocks = atoi(argsv.data) * blocksmb; }
	else if(strendi(argsv.data, "gb")) { blocks = atoi(argsv.data) * blocksgb; }
	else if(strstarti(argsv.data, "all")) { blocks = INT_MAX; }
	else{ argsv.res = 0; return argsv; }

	// Create temp dir for data file
	strcpy(argsv.filepath, argsv.dirpath);
	strcat(argsv.filepath, FILE_PREFIX);
	strcat(argsv.filepath, "0");
	resi = makedir(argsv.filepath);
	if(resi != 0 && errno != 17) // Error 17, File exists.
	{
		errcheckint(resi, argsv, "Error creating data directory.");
		//argsv.res = -1; 
		//return argsv;
	}

	// Open file
	strcat(argsv.filepath, "\\0");
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
			fflush(fp);
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

	// Done
	printf("100%% \n");
	fflush(fp);
	fclose(fp);
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

	fflush(fp);
	resi = fclose(fp);
	if(resi == EOF){ return -1; }

	return 0;

}// 


// Clean directories
struct args cleandirs(struct args argsv)
{
	// 
	int index;
	int res;
	float stat = 0.1F;
	char stri[16];

	//
	//printf("Cleaning: \n");

	// Delete directories
	for(index = 1; index <= argsv.dirs; index++)
	{
		// File path
		sprintf(stri, "%d", index);
		strcpy(argsv.filepath, argsv.dirpath);
		strcat(argsv.filepath, FILE_PREFIX);
		strcat(argsv.filepath, stri);

		// Delete dir
		res = remdir(argsv.filepath);
		if(res != 0){ argsv.res = -1; return argsv; }
		//errcheckint(res, argsv, "Error deleting file.");

		//// Statistics
		//if(index > argsv.dirs * stat)
		//{
		//	printf("%.0f%% ", stat * 100 );
		//	stat += 0.1F;

		//}// if

	}// for

	// 
	//printf("100%% \n");
	argsv.res = 0; 
	return argsv;

}//


// Clean data file and dir
struct args cleandata(struct args argsv)
{
	int res;
		
	// Delete data file
	strcpy(argsv.filepath, argsv.dirpath);
	strcat(argsv.filepath, FILE_PREFIX);
	strcat(argsv.filepath, "0\\0");
	remove(argsv.filepath);

	// Delete temp dir of data file
	strcpy(argsv.filepath, argsv.dirpath);
	strcat(argsv.filepath, FILE_PREFIX);
	strcat(argsv.filepath, "0");
	res = remdir(argsv.filepath);

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


// Create directory, windows linux
int makedir(const char* name) 
{
#ifdef _WIN32	//  __linux__
	return _mkdir(name);
#else      
	return mkdir(name, 0700);  
#endif

}// 


// Remove directory, windows linux
int remdir(const char* name) 
{
#ifdef _WIN32	//  __linux__
	return _rmdir(name);
#else      
	return rmdir(name);	
#endif
}// 


// printtime
void printtime(clock_t ticks)
{
	float total = (float) ticks / CLOCKS_PER_SEC;

	int hours = (int)total / 60 / 60;
	int minutes = (int)total / 60 % 60;
	int seconds = (int)total % 60;

	int intpart = (int)total;
	float decpart = total - intpart;
	float milsec = decpart * 1000;

	printf("Time: %02d:%02d:%02d.%.0f \n", 
		hours, minutes, seconds, milsec);

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
	return strnicmp2(str, substr,  (int)strlen(substr) - 1);
}


// String ends with, case insensitive, 0 = false, 1 = true
int strendi(const char* str, const char* substr){
	int substrlen = (int)strlen(substr);
	return strnicmp2(str + strlen(str) - substrlen, substr, substrlen);
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
		"Overwrite empty space on disk, metadata and data. \n\n"
		"USAGE: \n"
		"overwrite [-h -v -test -one -rand -block:] (-dirs: &| -data:) -path: \n"
		"  -h \t\t Print help and usage message. \n"
		"  -v \t\t Print program version. \n"
		"  -test \t For test, don't delete written files. \n"
		"  -one \t\t Overwrite with 1, default with 0. \n"
		"  -rand \t Overwrite with random data, default with 0. \n"
		"  -block \t Block size, default 4096 bytes as NTFS and EXT4. \n"
		"  -dirs \t Number of directories to write, ex: 10, 100.\n"
		"  -data \t Quantity of data to write, ex: 1mb, 1gb, all. \n"
		"  -path \t Path to directory where to write files. \n\n"
		"EXAMPLE: \n"
		"Write 10 dirs and 10Mb data, on Windows \n"
		"overwrite -dirs:10 -data:10mb -path:c:\\ \n\n"
		"Write random data, block size 512 bytes on Linux \n"
		"overwrite -rand -block:512 -data:10 -path:/mnt/usbdisk/ \n\n"
		"Copyright GPLv3 http://github.com/ivoprogram/overwrite \n"

		);

}// 

