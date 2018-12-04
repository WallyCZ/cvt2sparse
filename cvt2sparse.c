#include <windows.h>
#include <stdio.h>
#include <inttypes.h>
#include "utils.h"

/*#include "bitfields.h"
#include "oassert.h"
#include "memutils.h"
#include "porg_utils.h"
#include "files.h"*/

BOOL verbose=FALSE, dry_run= FALSE;

void die_GetLastError(char *fname)
{
	DWORD err = GetLastError();
	exit(-1);
}

void set_sparse_range(char *fname, HANDLE h, LONGLONG start, LONGLONG size)
{
	FILE_ZERO_DATA_INFORMATION fzdi;
	fzdi.FileOffset.QuadPart = start;
	fzdi.BeyondFinalZero.QuadPart = start + size;    
	DWORD dw;
	BOOL b=DeviceIoControl(h, FSCTL_SET_ZERO_DATA, &fzdi, sizeof(fzdi), NULL, 0, &dw, NULL);
	if (b==FALSE)
	{
		printf ("Error while setting attributes for %s file:\n", fname);
		die_GetLastError ("DeviceIoControl(...FSCTL_SET_ZERO_DATA...) failed");
	};
}

void convert_file_to_sparse (char *fname)
{
	BOOL b;

	HANDLE f=CreateFile (fname, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING,
			FILE_ATTRIBUTE_NORMAL, NULL);

	if (f==INVALID_HANDLE_VALUE)
	{
		printf ("Error while opening %s file:\n", fname);
		die_GetLastError ("CreateFile()");
	};

	LARGE_INTEGER file_size;
	if (GetFileSizeEx (f, &file_size)==FALSE)
		die_GetLastError ("GetFileSizeEx()");

	printf ("%s...\n", fname);
	if (dry_run == FALSE)
	{
		DWORD dwTemp;
		b=DeviceIoControl(f, FSCTL_SET_SPARSE, NULL, 0, NULL, 0, &dwTemp, NULL);
		if (b==FALSE)
		{
			printf ("Error while setting attributes for %s file:\n", fname);
			die_GetLastError ("DeviceIoControl(...FSCTL_SET_SPARSE...) failed");
		};
	};

#define BLK_SIZE 0x100000
	BYTE* buf=(BYTE*)malloc(BLK_SIZE);
	//oassert(buf);

	int state=0; // 0 - initial, 1 - first zero block was seen
	LONGLONG zero_blk_begin=0;
	LONGLONG cur_pos=0;
	LONGLONG zero_bufs_size=0;

	for (;;)
	{
		DWORD was_read = 0;

		b = ReadFile (f, buf, BLK_SIZE, &was_read, NULL);

		BOOL is_zero_buf = is_blk_zero (buf, was_read);

		if (state == 0 && was_read > 0 && is_zero_buf)
		{
			zero_blk_begin=cur_pos;
			if (verbose)
			{
				printf("zero block. begin=0x%" PRIx64, zero_blk_begin);
			}
			state=1;
		}
		else if (state == 1 && is_zero_buf == FALSE)
		{
			// got zero block
			LONGLONG sz = cur_pos - zero_blk_begin;
			if (verbose)
			{
				printf (" end=0x%I64x, size=%" PRId64 "\n", cur_pos, sz);
				/*strbuf sb=STRBUF_INIT;
				strbuf_fancy_size (&sb, sz);
				strbuf_puts(&sb);
				strbuf_deinit(&sb);*/
			};

			if (dry_run == FALSE)
				set_sparse_range (fname, f, zero_blk_begin, sz);

			state = 0;
			zero_bufs_size += sz;
		};

		if (b == FALSE || was_read == 0)
		{
			break;
		}


		cur_pos += was_read;
	};

	free (buf);
	CloseHandle (f);

	printf ("Total size of zero blocks marked=%" PRId64 "\n", zero_bufs_size);
	/*strbuf sb=STRBUF_INIT;
	strbuf_fancy_size (&sb, zero_bufs_size);
	strbuf_puts(&sb);
	strbuf_deinit(&sb);*/
	printf ("Now the file occupies %02f%% of its size.\n", 
			100-(double)(((double)zero_bufs_size/(double)file_size.QuadPart)*100));
};

void convert_file_or_dir_to_sparse (char *fname)
{
	WIN32_FIND_DATA d;
	HANDLE h=FindFirstFile (fname, &d);
	if (h==INVALID_HANDLE_VALUE)
		die_GetLastError ("FindFirstFile()");

	do
	{
		if (IS_SET(d.dwFileAttributes, FILE_ATTRIBUTE_DIRECTORY))
			continue;

		/*strbuf sb=STRBUF_INIT;
		full_path_and_filename_to_path_only (&sb, fname);
		strbuf_addstr (&sb, d.cFileName);
		convert_file_to_sparse (sb.buf);
		strbuf_deinit (&sb);*/
		convert_file_to_sparse(d.cFileName);
	} while (FindNextFile (h, &d)!=FALSE);
};

int main()
{
	int argc = 0;
	PCHAR *argv = CommandLineToArgvA( GetCommandLineA(), &argc );
	printf ("Convert to NTFS sparse file <dennis(a)yurichev.com>, <vaclav.lipert(a)live.com> (%s)\n\n", __DATE__);

	if (argc==1)
	{
		printf ("Usage: cvt2parse [--verbose] [--dry-run] file_or_directory ...\n");
		printf ("Examples:\n");
		printf ("\tcvt2parse file1.vmdk file2.vmdk\n");
		printf ("\tcvt2parse *.vmdk\n");
		printf ("\tcvt2parse c:\\virtual_machine\\*.vmdk\n");
		return 1;
	};

	for (int arg=1; arg<argc; arg++)
	{
		//oassert (argv[arg]);
		if (stricmp (argv[arg], "--verbose")==0)
		{
			printf ("Setting verbose\n");
			verbose = TRUE;
		}
		else if (stricmp (argv[arg], "--dry-run")==0)
		{
			printf ("Setting dry run\n");
			dry_run = TRUE;
		}
		else
			convert_file_or_dir_to_sparse (argv[arg]);
	};
	return 0;
};

