/*-----------------------------------------------------------------------------
main.c 
Handles Command line parsing and file i/o
-----------------------------------------------------------------------------*/

/*-----------------------------------------------------------------------------
Problem listed two requirements:
1. Output file contains the 32 largest values from the input file.
2. Output file should also contain the last 32 values read from the input file.

Design considerations
---------------------
1. 	For requirement 1, I have used a minheap (priority queue). 
	Minheap provides compact and fast implementation for storing 
	a small subset of ordered data. Minheap requires only one 
	comparison for each element. If Minheap is full, one comparison 
	with top of the heap is needed to decide if we need to add the 
	element to minheap. This speeds up the implementation considerably 
	for  data sets. Minheap also allows efficient way to sort the list 
	as per the requirements. Also, insertion and removal of data from 
	heap is very fast (through sink/swim operations). Minheap also has
	an advantage on space as it does not require additional data 
	structures for maintaining state 
	(as opposed to BST which requires left and right pointers).

2. 	For requirement 2, I have used a circular buffer to store the last 
	32 values read from the input file. I have also provided a compact 
	version of the circular buffer that packs the data. However, compact 
	version is little bit complicated (and slightly computation heavy) 
	as it need to pack and unpack data of 12-bits. A suitable data 
	structure can be used depending on the memory/performance tradeoff. 
	Compact version can be turned on by setting 
	USE_COMPRESSED_CIRC_BUF to 1 in config.h
 
3. 	I have directly used array of 32 elements for both minheap and 
	circular buffer implementations. This design can be easily extended 
	once we know more about the system requirements. For example, minheap 
	and circular buffer can hold up to 2^16 elements. Also data for these 
	data structures can be provided by the users.  Please note that, this 
	will have additional impact on memory size. 
 
 4.	For memory allocation, I have used global variables for this problem. 
	This is because on some systems local memory will be available for 
	storing relatively small amount of data. For example, compiler can 
	statically allocate global variable in device's local memory. Please 
	note that depending on the embedded system capabilities you can use 
	several methods to allocate/deallocate memory.

	Overall the current design/implementation takes 135 bytes of memory 
	(for non-compressed circular buffer) and 119 bytes of memory 
	(for compressed circular buffer). 
	
Files
-----
main.c
------
Starting program, handles IO and commandline arguments. Please note 
that this file will not be part of the embedded code.

acceldata.h/c
-------------
Main logic is here for handling the accelerometer data.  It allocates 
memory and uses other data structures to store data.

circbuf.h/c
-----------
Circular buffer implementation for 16-bit data

circbuf_12bit.h/c
-----------------
Circular buffer implemenation for 12-bit data (compact)

pq.h/c
------
Minheap (priority queue) implementation.

config.h
---------
configuration details
------------------------------------------------------------------------------ */

#include <stdio.h>
#include <unistd.h>
#include <getopt.h>
#include <sys/stat.h>
#include "config.h"
#include "acceldata.h"
#include "pq.h"
#include "cbuf.h"

#define NUM_ARGS 4
#define FULL_DATA_BYTES		3
#define DATA_BUF_SIZE		3000 

/*-----------------------------------------------------------------------------
Prints the usage
\param pname		Name of the Program
-----------------------------------------------------------------------------*/
void print_usage(const char* pname)
{
	fprintf(stdout, "usage: %s -i in_file -o out_file\n", pname);
}

/*-----------------------------------------------------------------------------
Parses the commandline arguments
\param argc      Number of arguments
\param argv      Array of arguments
\param pin		Pointer to input file name
\param pout		Pointer to output file name
\return          Status: 0 - success, otherwise falilure
-----------------------------------------------------------------------------*/
int parse_args(int argc, char *argv[], char **p_in, char **p_out)
{
	int c;

	if (argc != NUM_ARGS+1) /* +1 for program name */
	{
		return STATUS_ERR;
	}

	opterr = 0;

	while ((c = getopt(argc, argv, "i:o:")) != -1)
	{
		switch (c)
		{
		case 'i':
			*p_in = optarg;
			break;

		case 'o':
			*p_out = optarg;
			break;

		case '?':
			if (optopt == 'i' || optopt == 'o')
				fprintf(stderr, "Option -%c requires a value\n", optopt);
			else
				fprintf(stderr, "Unknown option character '\\x%x'\n", optopt);
			
		default:
			return STATUS_ERR;
		}
	}

	if ((NULL == *p_in) || (NULL == *p_out))
	{
		fprintf(stderr, "Invalid inputs\n");
		return STATUS_ERR;
	}

	return STATUS_OK;
}

/*-----------------------------------------------------------------------------
Processes input data read from the file
\param buf				Data buffer
\param size				Number of bytes to be processed
-----------------------------------------------------------------------------*/
void process_input_data(const uint8_t *buf, size_t size)
{
	size_t full_data		= size / FULL_DATA_BYTES;
	size_t full_data_bytes	= full_data * FULL_DATA_BYTES;
	size_t partial_data		= size % FULL_DATA_BYTES;
	uint32_t i;

	/* Handle full data */
	for (i = 0; i < full_data_bytes; i += FULL_DATA_BYTES)
	{
		accel_process_3_bytes(buf + i, FULL_DATA_BYTES);
	}

	/* Handle partial data */
	if (2 == partial_data)
	{
		accel_process_2_bytes(buf, 2);
	}
	/* Otherwise ignore the 1 byte data since it is incomplete data */
}

/*-----------------------------------------------------------------------------
Read and Write the files
\param fin          Input File handle
\param fout         Output File handle
-----------------------------------------------------------------------------*/
int read_write_data(FILE *fin, FILE *fout, const struct stat* in_stat)
{
    uint8_t buf[DATA_BUF_SIZE];
    size_t bytes_read = 0;
    size_t cur_read;
    struct PQ* pq;
    struct CircBuf* circBuf;
    
    ASSERT(fin);
    ASSERT(fout);

    if (STATUS_ERR == accel_init())
    {
        return STATUS_ERR;
    }

    while (bytes_read < (size_t)in_stat->st_size)
    {
        cur_read = fread(buf, sizeof(buf[0]), sizeof(buf), fin);
        if (0 == cur_read)
        {
            fprintf(stderr, "Error reading input file\n");
            return STATUS_ERR;
        }

		process_input_data(buf, cur_read);
        bytes_read += cur_read;
    }

    /* write output file */
	pq = accel_get_pq();
	circBuf = accel_get_circ_buf();
    
    fprintf(fout, "%s\r\n", "--Sorted Max 32 Values--");
    if (ferror(fout))
    {
        fprintf(stderr, "Unable to write to output file\n");
    }

    while (!pq_empty(pq))
        fprintf(fout, "%hu\r\n", pq_get_min(pq));

    fprintf(fout, "%s\r\n", "--Last 32 Values--");
    while (!circ_buf_empty(circBuf))
        fprintf(fout, "%hu\r\n", circ_buf_get(circBuf));

    return STATUS_OK;
}

/*-----------------------------------------------------------------------------
performs the action 
\param in_file      Input File path
\param out_file     Output File path
-----------------------------------------------------------------------------*/
int perform_action(const char *in_file, const char *out_file)
{
    FILE *fin = NULL;
    FILE *fout = NULL;
    int status = STATUS_OK;
    struct stat in_stat;

    ASSERT(in_file);
    ASSERT(out_file);

    if (0 != stat(in_file, &in_stat))
    {
        fprintf(stderr, "Unable to locate in_file %s\n", in_file);
        return STATUS_ERR;
    }

    if (in_stat.st_size < 2)
    {
        fprintf(stderr, "Invalid input file %s. File size is less than 2\n", in_file);
        return STATUS_ERR;
    }

    /* Open input file for reading */
    fin = fopen(in_file, "rb");
    if (NULL == fin)
    {
        fprintf(stderr, "unable to open file %s for reading\n", in_file);
        return STATUS_ERR;
    }
    fout = fopen(out_file, "w");
    if (NULL == fout)
    {
        fprintf(stderr, "unable to open file %s for writing\n", out_file);
        fclose(fin);
        return STATUS_ERR;
    }

    status = read_write_data(fin, fout, &in_stat);
	if (STATUS_OK == status)
	{
		fprintf(stdout, "Output written to %s\n\n", out_file);
	}

    fclose(fin);
    fclose(fout);

    return status;
}

/*-----------------------------------------------------------------------------
Starting function of the program
\param argc      Number of arguments
\param argv      Array of arguments
\return          Status: 0 - success, otherwise falilure
-----------------------------------------------------------------------------*/
int main(int argc, char* argv[])
{
	char *in_file   = NULL;
	char *out_file  = NULL;
	int status = STATUS_OK;

	status = parse_args(argc, argv, &in_file, &out_file);
	if (STATUS_ERR == status)
	{
		print_usage(argv[0]);
		return status;
	}
    status = perform_action(in_file, out_file);

	return status;
}

