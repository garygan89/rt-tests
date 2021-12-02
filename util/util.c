/*
 * util.c
 *
 *  Created on: Apr 7, 2016
 *      Author: dsync
 */

#include "util.h"

#include <sys/types.h>
#include <dirent.h>

/**
 * Check whether a file exist. Return 1 if exist
 */
int _is_file_exist(char *path) {
	int fd;
	fd = open(path, O_CREAT | O_WRONLY | O_EXCL, S_IRUSR | S_IWUSR);
	if (fd < 0) {
		/* failure */
		if (errno == EEXIST) {
			return 1;
		}
	} else {
		return 0;
	}
}

int _is_dir_exist(char *path) {
	DIR* dir = opendir(path);
	if (dir)
	{
	    /* Directory exists. */
	    closedir(dir);
	    return 0;
	}
	else if (ENOENT == errno)
	{
	    /* Directory does not exist. */
		return -1;
	}
	else
	{
	    /* opendir() failed for some other reason. */
		return -2;
	}
}

int _mkdir_recursive(char *path) {
	char *subpath, *fullpath;

	fullpath = strdup(path);
	subpath = dirname(path);
	if (strlen(subpath) > 1)
		_mkdir_recursive(subpath);

	if (0 == _is_dir_exist(fullpath))
		printf("Dir: %s already exist!\n", fullpath);
	else {
		printf("Dir: %s not exist! Creating it now...\n", fullpath);
		mkdir(fullpath);
	}
	free(fullpath);
}

/**
 * Create a file. Intermediate dir will be created if not exist.
 */
int util_create_file(FileObj *fileObj) {
	char *file_path = strdup(fileObj->file_path);
	char *parent_dir = dirname(file_path);

	_mkdir_recursive(parent_dir);

	printf("Opening file: %s\n", fileObj->file_path);

	FILE *f;
	f = fopen(fileObj->file_path, "w");

	if (f == NULL) {
		return 1;
	}
	print_info("%s:\t File created: %s!\n", LOGTAG, fileObj->file_path);
	fileObj->fptr = f;

	return 0;
}


/**
 * filepath: e.g. /media/sdcard (no trailing /)
 * filename: e.g. test.csv
 */
int util_create_file_ori(FileObj *fileObj) {
	FILE *f;

	// create a duplicate of filepath since dirname() might modify its content
	char* ts1 = strdup(fileObj->file_path);
	char* dir = dirname(ts1);

	print_debug("%s:\t Creating parent dir: %s\n", LOGTAG, dir);
	mkdir(dir);

	f = fopen(fileObj->file_path, "w");

	if (f == NULL) {
		print_error("%s:\t Error opening file! Possibly dir not exist, making dir now...\n", LOGTAG);

		// create a duplicate of filepath since dirname() might modify its content
		char* ts1 = strdup(fileObj->file_path);
		char* dir = dirname(ts1);

		print_debug("%s:\t Creating parent dir: %s\n", LOGTAG, dir);
		mkdir(dir);

		util_create_file(fileObj);

//		return 1;
	}
	print_info("%s:\t File created: %s!\n", LOGTAG, fileObj->file_path);
	fileObj->fptr = f;

	return 0;
}

int util_close_file(FileObj *fileObj) {
	if (fileObj->fptr == NULL) {
		print_error("%s:\t File FD is null!\n", LOGTAG);
		return 1;
	}
	print_info("%s:\t File closed: %s\n", LOGTAG, fileObj->file_path);
	close(fileObj->fptr);

	return 0;
}

unsigned long util_read_file_buffer( char *filename, struct FileBuffer *pbuf, unsigned long maxlen )
{
  FILE *fp;
  int i;

  	print_debug("Opening file: %s\n", filename);
	if( (fp=fopen(filename, "rb")) == NULL )
	{
		print_error("Can't open file: %s\n", filename );
		return 0;
	}
	// find file size and allocate buffer for JSON file
	fseek(fp, 0L, SEEK_END);
	pbuf->length = ftell(fp);
	if( pbuf->length >= maxlen )
	{
		fclose(fp);
		return 0;
	}
	// rewind and read file
	fseek(fp, 0L, SEEK_SET);
	pbuf->data= (unsigned char *)malloc( pbuf->length + 1 );
	memset( pbuf->data, 0, pbuf->length+1 );	// +1 guarantees trailing \0

	i= fread( pbuf->data, pbuf->length, 1, fp );
	fclose( fp );
	if( i != 1 )
	{
		util_free_file_buffer( pbuf );
		return 0;
	}
	return pbuf->length;
}

// freeFileBuffer
// - free's buffer space and zeros it
//
void util_free_file_buffer( struct FileBuffer *buf )
{
	if( buf->data != NULL )
		free( buf->data );
	buf->data= 0;
	buf->length= 0;
}

// this will write an unknown code at the end of file ?
//void util_copy_file(char *from, char *to, int dont_write_if_file_exist) {
//	if (dont_write_if_file_exist) {
//		if (1 == _is_file_exist(to)) {
//			print_info("File: %s already exist! Not copying to it!\n", to);
//			return;
//		}
//	}
//
//	print_info("Copying: %s to %s\n", from, to);
//
//    FILE* copyFrom = fopen(from, "r");
//    FILE* copyTo = fopen(to, "w");
//    int caractereActuel = 0;
//    while(caractereActuel != EOF){
//        caractereActuel = fgetc(copyFrom);
//        fputc(caractereActuel,copyTo);
//    }
//    fclose(copyFrom);
//    fclose(copyTo);
//}

int copy_file(char *source, char  *destination, int dont_write_if_file_exist) {
	if (dont_write_if_file_exist) {
		if (1 == _is_file_exist(destination)) {
			print_info("File: %s already exist! Not copying to it!\n", destination);
			return -2;
		}
	}

    int input, output;
    if ((input = open(source, O_RDONLY)) == -1)
    {
        return -1;
    }
    if ((output = open(destination, O_RDWR | O_CREAT)) == -1)
    {
        close(input);
        return -1;
    }

	print_info("Copying: %s to %s\n", source, destination);

    //Here we use kernel-space copying for performance reasons
#if defined(__APPLE__) || defined(__FreeBSD__)
    //fcopyfile works on FreeBSD and OS X 10.5+
    int result = fcopyfile(input, output, 0, COPYFILE_ALL);
#else
    //sendfile will work with non-socket output (i.e. regular file) on Linux 2.6.33+
    off_t bytesCopied = 0;
    struct stat fileinfo = {0};
    fstat(input, &fileinfo);
    int result = sendfile(output, input, &bytesCopied, fileinfo.st_size);
#endif

    close(input);
    close(output);

    return result;
}

//void util_copy_channel_metadata(char *output_dir) {
//	char out[150];
//	sprintf(out, "%s/channel_metadata.json", output_dir);
//
//	// check whether file exist before copying
//	if (0 != _is_file_exist(out)) {
//		printf("Copying channel_metadata.json to: %s\n", out);
//		_copy_file("/media/sdcard/configs/channel_metadata.json", out);
//	}
//}

int util_create_pthread(pthread_t *pthread, void (*__start_routine) (void *), int scheduling_policy, int thread_priority, int thread_stack_size, int cpu_core) {
	pthread_attr_t attr;
	struct sched_param parm;
	pthread_attr_init(&attr);

	pthread_attr_getschedparam(&attr, &parm);
	parm.sched_priority = thread_priority;
	pthread_attr_setschedpolicy(&attr, scheduling_policy);
	pthread_attr_setschedparam(&attr, &parm);

	printf("%s:\t Setting thread stack size to %d\n", LOGTAG, thread_stack_size);
	/* Set the requested stacksize for this thread */
	if (pthread_attr_setstacksize(&attr, thread_stack_size))
		return 2;

	cpu_set_t mask;
	/* CPU_ZERO initializes all the bits in the mask to zero. */
	CPU_ZERO(&mask);
	/* CPU_SET sets only the bit corresponding to cpu. */
	CPU_SET(cpu_core, &mask);
	pthread_attr_setaffinity_np(&attr, sizeof(cpu_set_t), &mask);
	printf("%s:\t Assigned CPU%d to thread!\n", LOGTAG, cpu_core);

	int rc;
	rc = pthread_create(pthread, &attr, __start_routine, (void *)0);

	return rc;

}

void util_setup_sig_catcher(void (*__signal_handler_routine) (int)) {
	// setup sig action listener
	printf("%s:\t Setting up signal catcher... ", LOGTAG);
	struct sigaction action;
	memset(&action, 0, sizeof(struct sigaction));
//	action.sa_handler = sig_catch_handler;
	action.sa_handler = __signal_handler_routine;
	sigaction(SIGINT, &action, NULL);
	sigaction(SIGKILL, &action, NULL);
	sigaction(SIGTERM, &action, NULL);
}

