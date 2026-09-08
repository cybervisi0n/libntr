#ifndef TWL_DBGHOST_H_
#define TWL_DBGHOST_H_

#ifndef _IOREAD
#define _IOREAD 0x01
#define _IOWRITE 0x02
#define _IORW 0x04
#define _IOEOF 0x08
#define _IOFLUSH 0x10
#define _IOERR 0x20
#define _IOSTRING 0x40
#endif

#ifdef __cplusplus
extern "C" {
#endif

void dbghostInit(void);

struct ffblk {
  char ff_reserved[23];
  char ff_attrib;
  unsigned short ff_ftime;
  unsigned short ff_fdate;
  struct {
    unsigned short low, high;
  } ff_fsize;
  char ff_name[255 + 1];
};

typedef struct {
  unsigned year;
  unsigned mon;
  unsigned day;
  unsigned hour;
  unsigned min;
  unsigned sec;
  unsigned msec;
} Dbghost_dos_time;

int dbghost_dos_put_console(char c);
int dbghost_dos_putstring_console(char *str, int size);
int dbghost_dos_stat_console(void);
int dbghost_dos_get_console(void);
int dbghost_dos_open(char *fname, int mode);
int dbghost_dos_creat(char *fname, int mode);
int dbghost_dos_read(int fd, char *buf, int size);
int dbghost_dos_write(int fd, char *buf, int size);
int dbghost_dos_close(int fd);
int dbghost_dos_lseek(int fd, int ofs, int pos);
int dbghost_dos_file_mode(char *fname, int mode, int action);
int dbghost_dos_set_dta(struct ffblk *fbp);
int dbghost_dos_find_first(char *fname, unsigned attrib);
int dbghost_dos_find_next(void);
int dbghost_dos_find_close(void);
int dbghost_dos_get_ioctl(int fd);
int dbghost_dos_get_file_time(int fd, unsigned short *timep);
int dbghost_dos_get_time(Dbghost_dos_time *dtp);
int dbghost_dos_set_file_time(int fd, unsigned short *timep);
int dbghost_dos_mkdir(char *path);
int dbghost_dos_rmdir(char *path);
int dbghost_dos_remove(char *fname);
int dbghost_dos_rename(char *old, char *new);
int dbghost_dos_getcwd(char *path, int drvno);
int dbghost_dos_chdir(char *path);
int dbghost_dos_getcdrv(void);
int dbghost_dos_dup(int fd);
int dbghost_dos_dup2(int oldfd, int newfd);
int dbghost_dos_version(void);
int dbghost_dos_exit(int retcode);
int dbghost_dos_sleep(int ms);

int dbghostRead_VIO_pt(void *dramAddr, int nbytes);
int dbghostWrite_VIO_pt(void *dramAddr, int nbytes);

int i_dbghost_init_osemu(void);

void dbghost_dos_printf(const char *text, ...);
void dbghost_dos_fprintf(int fd, const char *text, ...);

#ifdef __cplusplus
}
#endif

#endif // TWL_DBGHOST_H_
