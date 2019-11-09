
/*
TITLE:			Overwrite Program
DESCRIPTION:	Overwrite empty space on disk data and metadata.
CODE:			github.com/ivoprogram/overwrite
LICENSE:		GNU General Public License v3.0 http://www.gnu.org/licenses/gpl.html
AUTHOR:			Ivo Gjorgjievski
WEBSITE:		ivoprogram.github.io
VERSION:		1.5 2019-11-09

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
//
//#include <fileapi.h>
#include <windows.h>
#include <io.h>
#include <direct.h>

#else

// mkdir, rmdir, linux
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#endif


// Definitions -----
#define BLOCK_SIZE 4096		// Default block size, used in NTFS EXT4
#define PATH_LENGTH 4096	// Path buffer size
#define FILE_SUFFIX "00000000000000000000000000000000"	// File suffix for temp dirs
#define VERSION "Version 1.5 2019-11-09"	// Program version


// Arguments structure
struct args {
	int res;				// Result from called function
	int test;				// For test, don't delete written files
	int one;				// Write one's
	int ran;				// Write random data
	int blocksize;			// Block size
	int dirs;				// Number of directories to write
	int data;				// Quantity of data to write,
	char *dataunit;			// Data unit mb, gb, all, default mb
	char *path;				// Path where to write 
	char *pathend;			// Slash at the end of path, if missing
	char pathbuf[PATH_LENGTH + 1024];		// Buffer path + dir/file + suffix 
	unsigned char *blockbuf;	// Block buffer init with calloc
};


// Headers -----
struct args parseargs(int argc, char *argv[]);
struct args writedirs(struct args argsv);
struct args writedata(struct args argsv);
struct args cleandirs(struct args argsv);
struct args cleandata(struct args argsv);
void freemem(struct args argsv);

void randbuf(unsigned char *buf, int bufsize);
void getpath(struct args *argsv, const char *prefix);
int makedir(const char* name);
int remdir(const char* name);
void syncfile(FILE *fp);
void printtime(clock_t ticks);
int strstart(const char *str, const char *sub);
int strend(const char *str, const char *sub);
int stricmp2(const char *str1, const char *str2, size_t count);

void errmsg(struct args argsv, char *message);
void exitmsg(struct args argsv, char *message);
void exiterr(struct args argsv, char *message);
void errcheckint(int res, struct args argsv, char *message);
void errcheckptr(void *res, struct args argsv, char *message);
void usage();


// Main function -----
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

	// Write dirs / metadata
	argsv = writedirs(argsv);
	if(!argsv.test){ cleandirs(argsv); }

	// Clean memory
	freemem(argsv);

	// Done
	ticks = clock() - ticks;
	printtime(ticks);
	printf("Done. \n");
	return 0;

}// 


// Parse arguments
struct args parseargs(int argc, char *argv[])
{
	// 
	int index = 0;			// Index
	int res = 0;			// Result
	struct args argsv;		// Arguments structure

	// Init arguments structure
	argsv.res = 0;
	argsv.test = 0;
	argsv.one = 0;
	argsv.ran = 0;
	argsv.blocksize = BLOCK_SIZE;
	argsv.dirs = 0;
	argsv.data = 0;
	argsv.dataunit = "";
	argsv.path = "";
	argsv.pathend = "";
	argsv.blockbuf = NULL;
	strcpy(argsv.pathbuf, "");

	// Usage
	if(argc < 2){ usage(); exit(0); }

	// Parse arguments
	for(index = 0; index < argc; index++)
	{
		// 
		if(strstart(argv[index], "-h") || strstart(argv[index], "--help") ){ 
			usage();
			exit(0);
		}
		else if(strstart(argv[index], "-v") || strstart(argv[index], "--version") ){ 
			printf(VERSION "\n");
			exit(0);
		}
		else if(strstart(argv[index], "-test")){ 
			argsv.test = 1; 
		}
		else if(strstart(argv[index], "-one")){ 
			argsv.one = 1; 
		}
		else if(strstart(argv[index], "-rand")){ 
			argsv.ran = 1; 
		}
		else if(strstart(argv[index], "-block:")){
			argsv.blocksize = atoi(argv[index] + 7);
			if(argsv.blocksize == 0){ argsv.blocksize = BLOCK_SIZE; }
		}
		else if(strstart(argv[index], "-meta:")){
			argsv.dirs = atoi(argv[index] + 6);
		}
		else if(strstart(argv[index], "-data:")){
			argsv.data = atoi(argv[index] + 6);
			argsv.dataunit = argv[index] + 6;
		}
		else if(strstart(argv[index], "-path:")){
			argsv.path = argv[index] + 6;
		}

	}// for

	// Check for space in args
	for(index = 0; index < argc; index++)
	{
		if(strstart(argv[index], "-block: ") ||
			strstart(argv[index], "-meta: ") ||
			strstart(argv[index], "-data: ") ||
			strstart(argv[index], "-path: ")
			){
				exitmsg(argsv, "Error, arguments contain space after :");
		}
	}

	// Dirs or data args are required
	if(argsv.dirs == 0 && argsv.dataunit == NULL)
	{
		exitmsg(argsv, "Error, arguments -meta: or -data: are missing.");
	}

	// Path arg is required
	if(argsv.path == NULL)
	{
		exitmsg(argsv, "Error, argument -path: is missing.");
	}

	// Check if path is less than PATH_LENGTH
	if(strlen(argsv.path) > PATH_LENGTH)
	{
		exitmsg(argsv, "Path is too long, parameter -path: \n");
	}

	// Add / at the end of path if missing, windows unix
#ifdef _WIN32	//  __linux__
	if(!strend(argsv.path, "/") && !strend(argsv.path, "\\")){
		argsv.pathend = "/";
	}
#else      
	if(!strend(argsv.path, "/")){
		argsv.pathend = "/";
	}
#endif

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
	char stri[64];		// Dir name


	// If dirs 0 return
	if(!argsv.dirs){ argsv.res = 0; return argsv; }

	// Write dirs
	printf("Writing metadata: \n");

	for(index = 1; index <= argsv.dirs; index++)
	{
		// Get path
		sprintf(stri, "%d", index);
		getpath(&argsv, stri);

		// Write dir
		res = makedir(argsv.pathbuf);

		// If error at first write exit application
		if(res && index == 1 && errno != 17)  // Error 17 File exists.
		{ errcheckint(res, argsv, "Error writing directories."); }

		// If error on next writes exit function
		if(res && errno != 17){ 
			printf("100%% \n");
			argsv.dirs = index;	// How many directories were created, for cleaning
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
	if(argsv.data < 1 && !strstart(argsv.dataunit, "all"))
	{ argsv.res = 0; return argsv; }

	// How many blocks to write, 1Mb / blocksize
	blocksmb = (1024 * 1000) / argsv.blocksize;
	blocksgb = (1024 * 1000 * 1000) / argsv.blocksize;
	blockscont = blocksgb;

	// Calculate blocks to write
	blocks = argsv.data * blocksmb; // Default mb
	if(strend(argsv.dataunit, "gb")) { blocks = argsv.data * blocksgb; }
	else if(strstart(argsv.dataunit, "all")) { blocks = INT_MAX; }

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
	strcat(argsv.pathbuf, "/0");
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
			syncfile(fp);
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
	syncfile(fp);
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
	char stri[64];				// Dir name

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
	getpath(&argsv, "0");
	strcat(argsv.pathbuf, "/0");
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
		printf("Error on free() memory: %d %s \n", errno, strerror(errno));
	}

}// 



// Util functions ---------------------


// Set buffer with random data
void randbuf(unsigned char *buf, int bufsize)
{
	int index;
	srand((unsigned)time(NULL));	// Init rand

	for(index = 0; index < bufsize - 1; index++)
	{
		buf[index] = rand() % 256; // 
	}
}


// Get path, slash, prefix, suffix
// Using struct pointer for performance, will be called in loop
void getpath(struct args *argsv, const char *prefix)
{
	// Add to buffer path, slash, prefix, suffix
	strcpy(argsv->pathbuf, argsv->path);
	strcat(argsv->pathbuf, argsv->pathend);
	strcat(argsv->pathbuf, prefix);
	strcat(argsv->pathbuf, FILE_SUFFIX);

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


// Sync file buffer to disk, windows linux
void syncfile(FILE *fp)
{
#ifdef _WIN32	//  __linux__
	HANDLE hd = (HANDLE) _get_osfhandle(_fileno(fp));
	FlushFileBuffers(hd);
#else      
	int fd = fileno(fp);
	fsync(fd);
#endif

}//


// Print time
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

	printf("\nTime: %02d:%02d:%02d.%.0f \n", 
		hours, minutes, seconds, milsec);

}// 


// String start with, 0 false, 1 true
int strstart(const char *str, const char *sub)
{
    return stricmp2(str, sub, strlen(sub)) == 0;
}

// String end with, 0 false, 1 true
int strend(const char *str, const char *sub)
{
	int sublen = (int)strlen(sub);
    return stricmp2(str + strlen(str) - sublen, sub, sublen) == 0;
}

// String compare case insensitive, windows linux
int stricmp2(const char *str1, const char *str2, size_t count)
{
#ifdef _WIN32	//  __linux__
	return _strnicmp(str1, str2, count);
#else      
	return strncasecmp(str1, str2, count);
#endif
}


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
		"Overwrite empty space on disk data and metadata.\n"
		VERSION "\n\n"
		"USAGE: \n"
		"overwrite [-h -v -test -one -rand -block:] (-meta: &| -data:) -path: \n"
		"  -h \t\t Print help and usage message. \n"
		"  -v \t\t Print program version. \n"
		"  -test \t For test, don't delete written files. \n"
		"  -one \t\t Overwrite with 1, default with 0. \n"
		"  -rand \t Overwrite with random data, default with 0. \n"
		"  -block \t Block size, default 4096 bytes as NTFS and EXT4. \n"
		"  -meta \t Number of metadata to write, ex: 1, 10.\n"
		"  -data \t Quantity of data to write, 1mb, 1gb, all, default mb. \n"
		"  -path \t Path to directory where to write. \n\n"
		"EXAMPLE: \n"
		"- Write 10 metadata and 10Mb data, on Windows \n"
		"overwrite -meta:10 -data:10 -path:c:\\ \n"
		"- Write random, block 512 bytes, 10Mb data, on Linux \n"
		"overwrite -rand -block:512 -data:10 -path:/mnt/usbdisk/ \n\n"
		"Copyright GPLv3 http://github.com/ivoprogram/overwrite \n"

		);

}// 

