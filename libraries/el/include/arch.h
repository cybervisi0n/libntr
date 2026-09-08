#ifndef ARCH_H_
#define ARCH_H_

#define ARMAG "!<arch>\n" /* magic string */
#define SARMAG 8          /* length of magic string */

#define ARFMAG "\`\n"  /* header trailer string */
#define AR_NAME_LEN 16 /* ar_name size, includes `/' */

typedef struct /* archive file member header - printable ascii */
{
  char ar_name[16]; /* file member name - `/' terminated */
  char ar_date[12]; /* file member date - decimal */
  char ar_uid[6];   /* file member user id - decimal */
  char ar_gid[6];   /* file member group id - decimal */
  char ar_mode[8];  /* file member mode - octal */
  char ar_size[10]; /* file member size - decimal */
  char ar_fmag[2];  /* ARFMAG - string to end header */
} ArchHdr;          /* Total 60 (0x3C) bytes */

u32 AR_GetEntrySize(ArchHdr *ArHdr);

#endif /*ARCH_H_*/
