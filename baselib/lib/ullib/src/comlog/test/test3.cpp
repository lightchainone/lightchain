
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

int main(int argc, char **argv)
{
	struct stat st;
	stat(argv[1], &st);

#if 0
	struct stat {
		dev_t         st_dev;      /* device */
		ino_t         st_ino;      /* inode */
		mode_t        st_mode;     /* protection */
		nlink_t       st_nlink;    /* number of hard links */
		uid_t         st_uid;      /* user ID of owner */
		gid_t         st_gid;      /* group ID of owner */
		dev_t         st_rdev;     /* device type (if inode device) */
		off_t         st_size;     /* total size, in bytes */
		blksize_t     st_blksize;  /* blocksize for filesystem I/O */
		blkcnt_t      st_blocks;   /* number of blocks allocated */
		time_t        st_atime;    /* time of last access */
		time_t        st_mtime;    /* time of last modification */
		time_t        st_ctime;    /* time of last status change */
	};
#endif
	printf("st_dev:%ld\n", (long)st.st_dev);
	printf("st_ino:%ld\n", (long)st.st_ino);
	printf("st_mode:%ld\n", (long)st.st_mode);
	printf("st_nlink:%ld\n", (long)st.st_nlink);
	printf("st_uid:%ld\n", (long)st.st_uid);
	printf("st_gid:%ld\n", (long)st.st_gid);
	printf("st_rdev:%ld\n", (long)st.st_rdev);
	printf("st_size:%ld\n", (long)st.st_size);
	printf("st_blksize:%ld\n", (long)st.st_blksize);
	printf("st_blocks:%ld\n", (long)st.st_blocks);
	return 0;
}



















/* vim: set ts=4 sw=4 sts=4 tw=100 */
