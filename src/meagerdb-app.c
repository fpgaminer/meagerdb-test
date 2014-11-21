/*
 * All the application specific functions meagerDB needs.
 */
#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <ctype.h>
#include <meagerdb/meagerdb.h>
#include <meagerdb/app.h>


int mdba_open (char const *path, int flags)
{
	return open (path, flags, 0600);
}


int mdba_close (int fd)
{
	return close (fd);
}


int mdba_read (int fd, void *buf, size_t count)
{
	if (read (fd, buf, count) != (ssize_t)count)
		return -1;
	return 0;
}


int mdba_write (int fd, void const *buf, size_t count)
{
	if (write (fd, buf, count) != (ssize_t)count)
		return -1;
	return 0;
}


int mdba_lseek (int fd, uint64_t offset, int whence)
{
	off_t off = (off_t)offset;

	if ((uint64_t)off != offset || off < 0)
		return -1;

	if (lseek (fd, off, whence) == -1)
		return -1;

	return 0;
}


int mdba_fsync (int fd)
{
	return fsync (fd);
}


void mdba_fatal_error (void)
{
	fprintf (stderr, "FATAL ERROR\n");
	exit (-1);
}


/* Misc */
void mdba_read_urandom (void *dst, size_t len)
{
	uint8_t *pdst = (uint8_t *)dst;
	int fd = open ("/dev/urandom", O_RDONLY);

	if (fd == -1)
		mdba_fatal_error ();
	
	while (len)
	{
		ssize_t bytes = read (fd, pdst, len);

		if (bytes < 0)
			mdba_fatal_error ();

		pdst += (uint32_t)bytes;
		len -= (uint32_t)bytes;
	}

	close (fd);
}
