#include <game/Filesystem.h>
#include <fatfs/ff.h>
#include <stdio.h>
#include <sdcard/sdcard.h>

#include <errno.h>
#include <sys/stat.h>
#include <fcntl.h>

static FIL* __get_file(int *fd);

static FATFS fatfs;
uint32_t filesystem_initialized = 0;

void InitializeFilesystem()
{
	if(filesystem_initialized)
		return;

	if(SD_Detect() != SD_PRESENT)
		return;

	FRESULT res = f_mount(0, &fatfs);
	
	if(res != FR_OK)
		return;

	filesystem_initialized = 1;
}

void DeinitializeFilesystem()
{
	if(f_mount(0, NULL) == FR_OK)
	{
		filesystem_initialized = 0;
		SD_DeInit();
	}
}

uint32_t IsFilesystemInitialized()
{
	return filesystem_initialized;
}

//Newlib filesystem glue
static FIL open_files[MAX_OPEN_FILES];
static int active_files[MAX_OPEN_FILES] = {0};

int _open(const char *name, int flags, int mode)
{
	int fd = 0;
	BYTE mode_flags;

	if(!filesystem_initialized)
	{
		errno = ENXIO;
		return -1;
	}

	//Search free fd
	for(; fd < MAX_OPEN_FILES; fd++)
	{
		if(!active_files[fd])
			break;
	}

	if(fd == MAX_OPEN_FILES)
	{
		errno = ENFILE;
		return -1;
	}

	if((flags & O_ACCMODE) == O_RDONLY)
		mode_flags = FA_READ;
	else if((flags & O_ACCMODE) == O_WRONLY)
		mode_flags = FA_WRITE;
	else if((flags & O_ACCMODE) == O_RDWR)
	{
		mode_flags = FA_READ | FA_WRITE;
		if((flags & O_APPEND) == O_APPEND)
		{
			errno = EINVAL;
			return -1;
		}
	}
	else
	{
		errno = EINVAL;
		return -1;
	}

	if((flags & O_CREAT) == O_CREAT)
		mode_flags |= FA_OPEN_ALWAYS;
	else
		mode_flags |= FA_OPEN_EXISTING;

	if((flags & O_TRUNC) == O_TRUNC)
	{
		mode_flags &= ~FA_OPEN_ALWAYS;
		mode_flags |= FA_CREATE_ALWAYS;
	}

	switch(f_open(&(open_files[fd]), name, mode_flags))
	{
		case FR_OK:
		{
			DWORD goal;

			active_files[fd] = 1;

			goal = 0;
			if((flags & O_APPEND) == O_APPEND)
				goal = open_files[fd].fsize;

			/* Always seek since FA_OPEN_ALWAYS requires a seek to write. */
			if(f_lseek(&open_files[fd], goal) != FR_OK)
			{
				errno = EBADF;
				return -1;
			}

			return fd + 3;
		}

		case FR_NO_FILE:
			errno = ENOENT;
			break;
		case FR_NO_PATH:
			errno = EFAULT;
			break;
		case FR_INVALID_NAME:
		case FR_INVALID_DRIVE:
		case FR_EXIST:
			errno = EEXIST;
			break;
		case FR_DENIED:
		case FR_NOT_READY:
		case FR_WRITE_PROTECTED:
			errno = EROFS;
			break;
		case FR_DISK_ERR:
		case FR_INT_ERR:
		case FR_NOT_ENABLED:
		case FR_NO_FILESYSTEM:
			errno = ENXIO;
			break;
		default:
			errno = EACCES;
			break;
	}

	return -1;
}

int _file_close(int fd)
{
	if(!filesystem_initialized)
	{
		errno = ENXIO;
		return -1;
	}

    FIL* file = __get_file(&fd);
    if(!file)
        return -1;

    active_files[fd] = 0;

    switch(f_close(file))
    {
        case FR_OK:
            return 0;

        case FR_INVALID_OBJECT:
            errno = EBADF;
            break;

        case FR_DISK_ERR:
        case FR_INT_ERR:
        case FR_NOT_READY:
        default:
            errno = EACCES;
            break;
    }

    return -1;
}

int _file_read(int fd, void *buf, size_t len)
{
	if(!filesystem_initialized)
	{
		errno = ENXIO;
		return -1;
	}

	UINT rc;
    FIL* file = __get_file(&fd);

    if(!file)
        return -1;

    switch(f_read(file, buf, len, &rc))
    {
        case FR_OK:
            return rc;

        case FR_DENIED:
        case FR_DISK_ERR:
        case FR_INT_ERR:
        case FR_NOT_READY:
        case FR_INVALID_OBJECT:
        default:
            break;
    }

    errno = EBADF;
    return -1;
}

int _file_write(int fd, void *buf, size_t len)
{
	if(!filesystem_initialized)
	{
		errno = ENXIO;
		return -1;
	}

    UINT wc;
    FIL* file = __get_file(&fd);

    if(!file)
        return -1;

    switch(f_write(file, buf, len, &wc))
    {
        case FR_OK:
            return wc;

        case FR_DENIED:
        case FR_NOT_READY:
        case FR_INVALID_OBJECT:
        default:
            break;
    }

    errno = EBADF;
    return -1;
}

int _file_fstat(int fd, struct stat *st)
{
	if(!filesystem_initialized)
	{
		errno = ENXIO;
		return -1;
	}

    FIL* file = __get_file(&fd);

    if(!file)
        return -1;

    st->st_size = file->fsize;
    st->st_mode = S_IFREG | S_IFBLK;
    st->st_blksize = 512;

    return 0;
}

off_t _file_lseek(int fd, off_t offset, int whence)
{
	if(!filesystem_initialized)
	{
		errno = ENXIO;
		return -1;
	}

    DWORD goal;

    FIL* file = __get_file(&fd);

    if(!file)
        return -1;

    if(whence == SEEK_CUR)
        goal = file->fptr + offset;
    else if(whence == SEEK_SET)
        goal = offset;
    else if(whence == SEEK_END)
        goal = file->fsize + offset;
    else
    {
        errno = EINVAL;
        return -1;
    }

    if(goal < 0)
    {
        errno = EINVAL;
        return -1;
    }

    switch(f_lseek(file, goal))
    {
        case FR_OK:
            return goal;

        case FR_DISK_ERR:
        case FR_INT_ERR:
        case FR_NOT_READY:
        case FR_INVALID_OBJECT:
        default:
            break;
    }

    errno = EBADF;
    return -1;
}

int _file_isatty(int fd)
{
	if(!filesystem_initialized)
	{
		errno = ENXIO;
		return -1;
	}

    fd -= 3;
    errno = EBADF;

    if((fd > 0) && (fd < MAX_OPEN_FILES))
        if(!active_files[fd])
        	errno = EINVAL;

    return 0;
}

static FIL* __get_file(int *fd)
{
	*fd -= 3;
	
	if((*fd < 0) || (*fd > MAX_OPEN_FILES)) {
		errno = EBADF;
		return NULL;
	}

	if(!active_files[*fd]) {
		errno = EBADF;
		return NULL;
	}

	return (FIL *)&open_files[*fd];
}
