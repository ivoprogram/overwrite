
/*
TITLE:			Overwrite
DESCRIPTION:	Overwrite empty space on disk data and metadata.
CODE:			github.com/ivoprogram/overwrite
LICENSE:		GNU General Public License v3.0 http://www.gnu.org/licenses/gpl.html
AUTHOR:			Ivo Gjorgjievski
WEBSITE:		ivoprogram.github.io
VERSION:		1.4 2019-11-02

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
#define PATH_LENGTH 4096	// Path buffer size
#define FILE_PREFIX "0"		// File prefix for temporary files.
#define VERSION "Version 1.4 2019-11-02"	// Program version


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
	char pathbuf[PATH_LENGTH + 1024];		// Path buffer + prefix + dir/file 
	unsigned char *blockbuf;	// Block buffer init with calloc
};


// Headers -----
struct args parseargs(int argc, char *argv[]);
struct args writedirs(struct args argsv);
struct args writedata(struct args argsv);
struct args cleandirs(struct args argsv);
struct args cleandata(struct args argsv);
void freemem(struct args argsv);

void getpath(struct args *argsv, const char *suffix);
int makedir(const char* name);
int remdir(const char* name);
void printtime(clock_t ticks);
void randbuf(unsigned char *buf, int bufsize);
int strstarti(const char *str, const char *substr);
int strendi(const char* str, const char* substr);
int strnicmp2(const char *str1, const char *str2, int len);

void errmsg(struct args argsv, char *message);
void exitmsg(struct args argsv, char *message);
void exitmsg2(struct args *argsv, char *message);
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

	// Parse arguments
	argsv = parseargs(argc, argv);

	// Write data
	argsv = writedata(argsv);
	if(!argsv.test){ cleandata(argsv); }

	// Write dirs
	argsv = writedirs(argsv);
	if(!argsv.test){ cleandirs(argsv); }

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
	strcpy(argsv.pathbuf, "");

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

	// Initialize blockbuf with calloc, set with 0.
	argsv.blockbuf = (unsigned char*) calloc(argsv.blocksize, sizeof (unsigned char));
	errcheckptr(argsv.blockbuf, argsv, "Block buffer initialization error. ");

	// If one, set buffer with one's
	if(argsv.one){ memset(argsv.blockbuf, 255, argsv.blocksize);  }

	// If random set buffer with random data
	if(argsv.ran){ randbuf(argsv.blockbuf, argsv.blocksize); }

	argsv.res = 0;
	return argsv;

}//


// writedirs
struct args writedirs(struct args argsv)
{
	// 
	int index = 0;		// Index
	int res = 0;		// Response
	float prog = 0.1F;	// Progress
	char stri[16];		// Dir name


	// If dirs 0 return
	if(!argsv.dirs){ argsv.res = 0; return argsv; }

	// Write dirs
	printf("Writing directories: \n");

	for(index = 1; index <= argsv.dirs; index++)
	{
		// Get path
		sprintf(stri, "%d", index);
		getpath(&argsv, stri);

		// Write dir
		res = makedir(argsv.pathbuf);

		// Check for error
		if(res && index == 1)
		{ errcheckint(res, argsv, "Error writing directories."); }

		if(res){ 
			printf("100%% \n");
			argsv.dirs = index;
			argsv.res = -1;
			return argsv; 
		}

		// Statistics
		if(argsv.dirs != INT_MAX && index > argsv.dirs * prog)
		{
			printf("%.0f%% ", prog * 100 );
			prog += 0.1F;

		}// if

	}// for

	printf("100%% \n");
	argsv.res = 0;
	return argsv;

}//


// writedata
struct args writedata(struct args argsv)
{
	int blocks = 0;			// Number of blocks to write
	int blocksmb = 0;		// Blocks per megabyte
	int blocksgb = 0;		// Blocks per gigabyte
	int blockscont = 0;		// Blocks count for statistics
	int blockscont2 = 0;	// Blocks count 2 for statistics

	int index = 0;			// Index
	int resi = 0;			// Response int
	size_t rest = 0;		// Response size_t
	double prog = 0.1;		// Progress
	FILE *fp;				// File pointer

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
	getpath(&argsv, "0");
	resi = makedir(argsv.pathbuf);
	if(resi != 0 && errno != 17) // Error 17 File exists.
	{
		errcheckint(resi, argsv, "Error creating data directory.");
		//argsv.res = -1; 
		//return argsv;
	}

	// Open file
	strcat(argsv.pathbuf, "\\0");
	fp = fopen(argsv.pathbuf, "wb");
	if(fp == NULL){ argsv.res = -1; return argsv; }

	// Write data
	printf("Writing data: \n");
	for(index = 0; index < blocks; index++)
	{
		// write block
		rest = fwrite(argsv.blockbuf, argsv.blocksize, 1, fp);

		// Check for error
		if(rest != 1){ 
			printf("100%% \n");
			fflush(fp);
			fclose(fp);
			argsv.res = -1; 
			return argsv;
		}

		// Statistics
		if(blocks != INT_MAX && index > blocks * prog)
		{
			printf("%.0f%% ", prog * 100 );
			prog += 0.1;

		}// if

		// Statistics for -data:all argument, up to 10gb
		if(blocks == INT_MAX && index >= blockscont && blockscont2 < 10)
		{
			blockscont += blocksgb;
			blockscont2++;
			printf("%dGb ", blockscont2 );
		}

	}// for

	// Done
	printf("100%% \n");
	fflush(fp);
	fclose(fp);
	argsv.res = 0;
	return argsv;

}// 


// Clean directories
struct args cleandirs(struct args argsv)
{
	// 
	int index;					// Index
	int res;					// Response
	float prog = 0.1F;			// Progress
	char stri[16];				// Dir name

	//
	//printf("Cleaning: \n");

	// Delete directories
	for(index = 1; index <= argsv.dirs; index++)
	{
		// File path
		sprintf(stri, "%d", index);
		getpath(&argsv, stri);

		// Delete dir
		res = remdir(argsv.pathbuf);
		if(res != 0){ argsv.res = -1; return argsv; }
		//errcheckint(res, argsv, "Error deleting file.");

		//// Statistics
		//if(index > argsv.dirs * prog)
		//{
		//	printf("%.0f%% ", prog * 100 );
		//	prog += 0.1F;

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
	int res = 0;	// Response

	// Delete data file
	getpath(&argsv, "0\\0");
	res = remove(argsv.pathbuf);
	if(res != 0){ argsv.res = -1; return argsv; }

	// Delete temp dir of data file
	getpath(&argsv, "0");
	res = remdir(argsv.pathbuf);
	if(res != 0){ argsv.res = -1; return argsv; }

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


// Get path + FILE_PREFIX + suffix
// Using struct pointer for performance, will be called in loop
void getpath(struct args *argsv, const char *suffix)
{
	// Check if path is less than PATH_LENGTH
	if(strlen(argsv->path) > PATH_LENGTH)
	{
		exitmsg2(argsv, "Path is too long, parameter -path: \n");
	}

	// Add path to buffer, slash, prefix, suffix
	strcpy(argsv->pathbuf, argsv->path);
	if(!strendi(argsv->pathbuf, "/") && !strendi(argsv->pathbuf, "\\")){
		strcat(argsv->pathbuf, "/");
	}
	strcat(argsv->pathbuf, FILE_PREFIX);
	strcat(argsv->pathbuf, suffix);

}// 


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
	// Total seconds
	float total = (float) ticks / CLOCKS_PER_SEC;

	// Hours, minutes, seconds
	int hours = (int)total / 60 / 60;
	int minutes = (int)total / 60 % 60;
	int seconds = (int)total % 60;

	// Milliseconds
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


// Exit with message
void exitmsg2(struct args *argsv, char *message)
{
	struct args argsv2 = *argsv;
	exitmsg(argsv2, message);

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
		"Overwrite empty space on disk data and metadata.\n"
		VERSION "\n\n"
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
		"  -path \t Path to directory where to write. \n\n"
		"EXAMPLE: \n"
		"- Write 10 dirs and 10Mb data, on Windows \n"
		"overwrite -dirs:10 -data:10mb -path:c:\\ \n"
		"- Write random data, block size 512 bytes on Linux \n"
		"overwrite -rand -block:512 -data:10mb -path:/mnt/usbdisk/ \n\n"
		"Copyright GPLv3 http://github.com/ivoprogram/overwrite \n"

		);

}// 

