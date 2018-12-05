 /*
  * myio.c
  * Trey Oehmler
  * CS315 Assignment 2 Fall 2018
  * 
  */

#include "myio.h"

struct myfile *myopen(const char *path, int flags)
{
	/* calls open() and initalizes a new myfile struct 
	 * returns NULL on error, ptr to struct otherwise */
	struct myfile *ret_file = malloc(sizeof(struct myfile));

	if (!ret_file) 
	{
		perror("malloc");
		return NULL; // malloc error
	}

	// initalize new file
	ret_file->fd = open(path, flags, 0666);
	ret_file->rdwr = 0;
	ret_file->rend = 0;
	ret_file->offset = 0;
	ret_file->r_off = 0;
	ret_file->rcount = 0;
	ret_file->wcount = 0;

	if (ret_file->fd == -1) 
	{
		perror("open");
		return NULL;
	}

	return ret_file;
}

ssize_t myread(struct myfile *file, void *usrbuf, size_t count)
{
	/* copies 'count' bytes from rbuf of file to usrbuf
	 * calls read() if not enough bytes in rbuf
	 * returns -1 on err, 0 for EOF, and bytes read otherwise */
	
	if (file->rdwr == 2)
	{
		myflush(file);
	}

	file->rdwr = 1;

	if (!count || file->rend)
	{
		return 0; // end of file or 0 bytes requested
	}

	char *r_ptr = file->rbuf;
	size_t r_off = file->r_off;

	if (count <= (file->rcount - r_off)) // enough bytes in rbuf
	{
		memcpy(usrbuf, r_ptr + r_off, count);
		file->offset += count;
		file->r_off += count;
		return count;
	}

	ssize_t ret_bytes = 0;
	if ((file->rcount - r_off) > 0) // copy remaining bytes from rbuf 
	{
		ret_bytes += file->rcount - r_off;
		memcpy(usrbuf, r_ptr + r_off, ret_bytes);
		file->offset += ret_bytes;
		file->r_off = file->rcount;
		count -= ret_bytes;
	}

	// call read and copy from rbuf until sufficent count or EOF
	while (count > 0 && !file->rend)
	{
		ssize_t bytes_read = read(file->fd, r_ptr, BUFFER_SIZE);
		if (bytes_read < 0)
		{
			perror("read");
			return -1;
		}
		else if (bytes_read == 0) // EOF reached
		{
			file->rend = 1;
			file->r_off = 0;
			file->rcount = 0;
		}
		else 
		{
			file->rcount = bytes_read;
			file->r_off = 0;
			if (file->rcount > count) 
			{
				memcpy((char *)usrbuf + ret_bytes, r_ptr, count);
				ret_bytes += count;
				file->offset += count;
				file->r_off += count;
				count = 0;
			}
			else
			{
				memcpy((char *)usrbuf + ret_bytes, r_ptr, file->rcount);
				ret_bytes += file->rcount;
				file->offset += file->rcount;
				file->r_off += file->rcount;
				count -= file->rcount;
			}
		}
	}

	return ret_bytes;
}


ssize_t mywrite(struct myfile *file, void *usrbuf, size_t count)
{
	/* copies 'count' bytes from usrbuf to wbuf of file
	 * writes to file once wbuf is full
	 * returns -1 on err, number of bytes written otherwise */

	if (file->rdwr == 1)
	{
		lseek(file->fd, (off_t) file->offset, SEEK_SET);
	}
	file->rdwr = 2;

	char *w_ptr = file->wbuf;

	if (!count)
	{
		return 0;
	}

	if ((file->wcount + count) <= BUFFER_SIZE) // enough space in wbuf
	{
		memcpy(w_ptr + file->wcount, usrbuf, count);
		file->wcount += count;
		return count;
	}

	ssize_t ret_bytes = (BUFFER_SIZE - file->wcount);
	if (ret_bytes > 0) // copy into remaining wbuf space
	{
		memcpy(w_ptr + file->wcount, usrbuf, ret_bytes);
		count -= ret_bytes;
		file->wcount += ret_bytes;
	}
	
	
	while (count > 0)
	/* clear wbuf by calling write()
	 * copy remaining bytes from usrbuf into wbuf */
	{
		ssize_t written_bytes = write(file->fd, w_ptr, BUFFER_SIZE);
		file->offset += BUFFER_SIZE;
		file->wcount = 0;

		if (written_bytes < 0) // check for write() err
		{
			perror("write");
			return -1;
		}
	
		if (count > BUFFER_SIZE)
		{
			written_bytes = BUFFER_SIZE;
			count -= written_bytes;
		}
		else
		{
			written_bytes = count;
			count = 0;
		}
		
		memcpy(w_ptr, (char *)usrbuf + ret_bytes, count);
		ret_bytes += written_bytes;
		file->wcount += written_bytes;
	}

	return ret_bytes;
}

int myclose(struct myfile *file)
{
	/* clears wbuf of file by calling myflush
	 * frees memory assoc w file
	 * returns -1 on error, 0 on success */

	myflush(file); 
	int ret_val = close(file->fd);
	if (ret_val < 0) 
	{
		perror("close");
		return -1;
	}
	free(file);
	return 0;
}

off_t myseek(struct myfile *file, off_t offset, int whence)
{
	/* changes file offset based on 'whence' 
	 * SEEK_SET: absolute, SEEK_CUT: relative
	 * clears neccesary fields for file & calls lseek() */

	myflush(file);

	if (whence == SEEK_SET)
	{
		file->offset = offset;
	}
	else if (whence == SEEK_CUR)
	{
		file->offset += offset;
	}

	file->rcount = 0;
	file->rdwr = 0;
	file->rend = 0;
	file->r_off = 0;
	file->wcount = 0;
	
	return lseek(file->fd, (off_t) file->offset, SEEK_SET);
}

void myflush(struct myfile *file)
{
	/* clears wbuf by calling write()
	 * updates offset and wcount of accordingly */

	if (file->wcount > 0) // prevents write() to rdonly file
	{
		ssize_t written_bytes = write(file->fd, file->wbuf, file->wcount);
		if (written_bytes < 0)
		{
			perror("write");
			return;
		}
		file->offset += file->wcount;
		file->wcount = 0;
	}
}

