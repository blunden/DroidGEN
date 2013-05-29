
#include "shared.h"


/*
    Load a normal file, or ZIP/GZ archive.
    Returns NULL if an error occured.
*/
int filesize(FILE *fp)
{
int pos,end;
pos=ftell (fp);
fseek (fp, 0, SEEK_END);
end = ftell (fp);
fseek (fp, pos, SEEK_SET);
return end;
}

int check_zip(char *filename)
{
    uint8 buf[2];
    FILE *fd = NULL;
    fd = fopen(filename, "rb");
    if(!fd) return (0);
    fread(buf, 2, 1, fd);
    fclose(fd);
    if(memcmp(buf, "PK", 2) == 0) return (1);
    return (0);
}


uint8 *load_archive(char *filename, int *file_size)
{
		int size = 0;
		uint8 *buf = NULL;

		if(check_zip(filename))
		{
        FILE *gd = NULL;
		buf = (uint8 *)malloc(4*1024*1024);
		printf("Zip detected %s\n",filename);
        /* Open file */
        gd = fopen(filename, "rb");
        if(!gd)
			return (0);

		*file_size = UnZipBuffer(buf,gd);
        
		/* Close file */
        fclose(gd);
		}else{
        FILE *gd = NULL;
		printf("Non Zip File %s\n",filename);
        /* Open file */
        gd = fopen(filename, "rb");
        if(!gd) return (0);

        /* Get file size */
        size=filesize(gd);

        /* Allocate file data buffer */
        buf = malloc(size);
        if(!buf)
        {
            fclose(gd);
            return (0);
        }

        /* Read file data */
        fread(buf,size,1,gd);

        /* Close file */
        fclose(gd);

        /* Update file size and return pointer to file data */
        *file_size = size;
		}
		return (buf);
}