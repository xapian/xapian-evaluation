/* gzip (GNU zip) -- compress files with zip algorithm and 'compress' interface
 * Copyright (C) 1992-1993 Jean-loup Gailly
 * The unzip code was written and put in the public domain by Mark Adler.
 * Portions of the lzw code are derived from the public domain 'compress'
 * written by Spencer Thomas, Joe Orost, James Woods, Jim McKie, Steve Davies,
 * Ken Turkowski, Dave Mack and Peter Jannesen.
 *
 * See the license_msg below and the file COPYING for the software license.
 * See the file algorithm.doc for the compression algorithms and file formats.
 */

static char  *license_msg[] = {
"   Copyright (C) 1992-1993 Jean-loup Gailly",
"   This program is free software; you can redistribute it and/or modify",
"   it under the terms of the GNU General Public License as published by",
"   the Free Software Foundation;\n"
" either version 2, or (at your option)",
"   any later version.",
"",
"   This program is distributed in the hope that it will be useful,",
"   but WITHOUT ANY WARRANTY; without even the implied warranty of",
"   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the",
"   GNU General Public License for more details.",
"",
"   You should have received a copy of the GNU General Public License",
"   along with this program; if not, write to the Free Software",
"   Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.",
0};


#ifdef RCSID
static char rcsid[] = "$Id: gzip.c,v 0.24 1993/06/24 10:52:07 jloup Exp $";
#endif

#include <ctype.h>
#include <sys/types.h>
#include <signal.h>
#include <sys/stat.h>
#include <errno.h>

#include "tailor.h"
#include "gzip.h"
#include "lzw.h"
#include "revision.h"
#include "getopt.h"

/* configuration */

#  include <time.h>

#  include <fcntl.h>

#  include <unistd.h>

#  include <stdlib.h>


#    include <utime.h>

#ifndef O_BINARY
#  define  O_BINARY  0  /* creation mode for open() */
#endif
#define RW_USER (S_IRUSR | S_IWUSR)  /* creation mode for open() */
 


#ifndef MAX_PATH_LEN
#  define MAX_PATH_LEN   1024 /* max pathname length */
#endif

#ifndef SEEK_END
#  define SEEK_END 2
#endif

#ifdef NO_OFF_T
  typedef long off_t;
  off_t lseek OF((int fd, off_t offset, int whence));
#endif


		/* global buffers */

DECLARE(uch, inbuf,  INBUFSIZ +INBUF_EXTRA);
/* DECLARE(uch, outbuf, OUTBUFSIZ+OUTBUF_EXTRA);  */
uch *outbuf;
DECLARE(ush, d_buf,  DIST_BUFSIZE);
DECLARE(uch, window, 2L*WSIZE);
DECLARE(ush, tab_prefix, 1L<<BITS);

		/* local variables */

int ascii = 0;        /* convert end-of-lines to local OS conventions */
int to_stdout = 0;    /* output to stdout (-c) */
int decompress = 0;   /* decompress (-d) */
int force = 0;        /* don't ask questions, compress links (-f) */
int no_name = -1;     /* don't save or restore the original file name */
int no_time = -1;     /* don't save or restore the original file time */
int recursive = 0;    /* recurse through directories (-r) */
int list = 0;         /* list the file contents (-l) */
int verbose = 0;      /* be verbose (-v) */
int quiet = 0;        /* be very quiet (-q) */
int do_lzw = 0;       /* generate output compatible with old compress (-Z) */
int test = 0;         /* test .gz file integrity */
int foreground;       /* set if program run in foreground */
char progname[] = "GUNZIP";       /* program name */
int maxbits = BITS;   /* max bits per code for LZW */
int method = DEFLATED;/* compression method */
int level = 6;        /* compression level */
int exit_code = OK;   /* program exit code */
int save_orig_name;   /* set if original name must be saved */
int last_member;      /* set for .zip and .Z files */
int part_nb;          /* number of parts in .gz file */
long time_stamp;      /* original time stamp (modification time) */
long ifile_size;      /* input file size, -1 for devices (debug only) */
char *env;            /* contents of GZIP env variable */
char **args = NULL;   /* argv pointer if GZIP env variable defined */
char z_suffix[MAX_SUFFIX+1]; /* default suffix (can be set with --suffix) */
int  z_len;           /* strlen(z_suffix) */

long bytes_in;             /* number of input bytes */
long bytes_out;            /* number of output bytes */
long total_in = 0;         /* input bytes for all files */
long total_out = 0;        /* output bytes for all files */
char ifname[MAX_PATH_LEN]; /* input file name */
char ofname[MAX_PATH_LEN]; /* output file name */
int  remove_ofname = 0;	   /* remove output file on error */
struct stat istat;         /* status for input file */
int  ifd;                  /* input file descriptor */
int  ofd;                  /* output file descriptor */
unsigned insize;           /* valid bytes in inbuf */
unsigned inptr;            /* index of next byte to be processed in inbuf */
unsigned outcnt;           /* bytes in output buffer */
unsigned outbuflen;        /* added for PADRE */
unsigned outbufptr;        /* bytes in  output buffer added for PADRE */
long header_bytes;

struct option longopts[] =
{
 /* { name  has_arg  *flag  val } */
    {"ascii",      0, 0, 'a'}, /* ascii text mode */
    {"to-stdout",  0, 0, 'c'}, /* write output on standard output */
    {"stdout",     0, 0, 'c'}, /* write output on standard output */
    {"decompress", 0, 0, 'd'}, /* decompress */
    {"uncompress", 0, 0, 'd'}, /* decompress */
 /* {"encrypt",    0, 0, 'e'},    encrypt */
    {"force",      0, 0, 'f'}, /* force overwrite of output file */
    {"help",       0, 0, 'h'}, /* give help */
 /* {"pkzip",      0, 0, 'k'},    force output in pkzip format */
    {"list",       0, 0, 'l'}, /* list .gz file contents */
    {"license",    0, 0, 'L'}, /* display software license */
    {"no-name",    0, 0, 'n'}, /* don't save or restore original name & time */
    {"name",       0, 0, 'N'}, /* save or restore original name & time */
    {"quiet",      0, 0, 'q'}, /* quiet mode */
    {"silent",     0, 0, 'q'}, /* quiet mode */
    {"recursive",  0, 0, 'r'}, /* recurse through directories */
    {"suffix",     1, 0, 'S'}, /* use given suffix instead of .gz */
    {"test",       0, 0, 't'}, /* test compressed file integrity */
    {"no-time",    0, 0, 'T'}, /* don't save or restore the time stamp */
    {"verbose",    0, 0, 'v'}, /* verbose mode */
    {"version",    0, 0, 'V'}, /* display version number */
    {"fast",       0, 0, '1'}, /* compress faster */
    {"best",       0, 0, '9'}, /* compress better */
    {"lzw",        0, 0, 'Z'}, /* make output compatible with old compress */
    {"bits",       1, 0, 'b'}, /* max number of bits per code (implies -Z) */
    { 0, 0, 0, 0 }
};

/* local functions */

/* local void usage        OF((void)); */
/* local void help         OF((void)); */
/* local void license      OF((void)); */
/* local void version      OF((void)); */
/* local void treat_stdin  OF((void)); */
/*local void treat_file   OF((char *iname)); */
local void treat_file   OF((uch *iname, uch *obuf, int obuflen));
/* local int create_outfile OF((void)); */
local int  do_stat      OF((char *name, struct stat *sbuf));
local char *get_suffix  OF((char *name));
local int  get_istat    OF((char *iname, struct stat *sbuf));
local int  make_ofname  OF((void));
local int  same_file    OF((struct stat *stat1, struct stat *stat2));
local int name_too_long OF((char *name, struct stat *statb));
local void shorten_name  OF((char *name));
local int  get_method   OF((int in));
local void do_list      OF((int ifd, int method));
local int  check_ofname OF((void));
/* local void copy_stat    OF((struct stat *ifstat));  */
local void do_exit      OF((int exitcode));
      int main          OF((int argc, char **argv));
int (*work) OF((int infile, int outfile)) = unzip; /* function to call */


#define strequ(s1, s2) (strcmp((s1),(s2)) == 0)

/* ======================================================================== */
int decompress_bundle(uch *cifname, uch *obuf, int obuflen)
{
    int i, proglen;        /* length of progname */

    proglen = strlen(progname);
    decompress = 1;

    /* Allocate all global buffers (for DYN_ALLOC option) */
    ALLOC(uch, inbuf,  INBUFSIZ +INBUF_EXTRA);
    /*    ALLOC(uch, outbuf, OUTBUFSIZ+OUTBUF_EXTRA); */
    outbuf = obuf;
    outbuflen = obuflen;
    outcnt = 0;
    outbufptr = 0;
    ALLOC(ush, d_buf,  DIST_BUFSIZE);
    ALLOC(uch, window, 2L*WSIZE);
    ALLOC(ush, tab_prefix, 1L<<BITS);

    /* And get to work */
    treat_file(cifname, obuf, obuflen);
    if (0) {
      printf("Decompressed file has %d characters\n", outbufptr);
      for (i = 0; i < 200; i++) if (i >= outbufptr) break; else putchar(outbuf[i]);
      printf("\n\n");
    }
    return outbufptr; 
}
#define STORED      0
#define COMPRESSED  1
#define PACKED      2
#define LZHED       3
/* methods 4 to 7 reserved */
#define DEFLATED    8
#define MAX_METHODS 9

uch *meth_name[] = {"STORED", "COMPRESSED", "PACKED", "LZHED", "RESERVED",
		      "RESERVED","RESERVED","RESERVED", "DEFLATED"};

/* ========================================================================
 * decompress the given file
 */
local void treat_file(uch *iname, uch *obuf, int obuflen)

{
    /* Check if the input file is present, set ifname and istat: */
    if (get_istat(iname, &istat) != OK) return;

    ifile_size = istat.st_size;

    /* Open the input file and determine compression method. The mode
     * parameter is ignored but required by some systems (VMS) and forbidden
     * on other systems (MacOS).
     */
    ifd = OPEN(ifname, ascii && !decompress ? O_RDONLY : O_RDONLY | O_BINARY,
	       RW_USER);
    if (ifd == -1) {
	fprintf(stderr, "%s: ", progname);
	perror(ifname);
	exit_code = ERROR;
	return;
    }
    clear_bufs(); /* clear input and output buffers */
    part_nb = 0;

    method = get_method(ifd); 
    /*if (method >= 0 && method < MAX_METHODS) 
      printf("  (%s)\n", meth_name[method]);
      else printf("  Compression method %d\n", method);*/
    if (method < 0) {
      close(ifd);
      return;               /* error message already emitted */
    }


    /* Actually do the compression/decompression. Loop over zipped members.
     */
    for (;;) {
	if ((*work)(ifd, ofd) != OK) {  /* ofd is ignored in PADRE version */
	    method = -1; /* force cleanup */
	    break;
	}
	if (last_member || inptr == insize) break;
	/* end of file */

	method = get_method(ifd);
	if (method < 0) break;    /* error message already emitted */
	bytes_out = 0;            /* required for length check */
    }

    close(ifd);
}


/* ========================================================================
 * Use lstat if available, except for -c or -f. Use stat otherwise.
 * This allows links when not removing the original file.
 */
local int do_stat(name, sbuf)
    char *name;
    struct stat *sbuf;
{
    errno = 0;
#if (defined(S_IFLNK) || defined (S_ISLNK)) && !defined(NO_SYMLINK)
    if (!to_stdout && !force) {
	return lstat(name, sbuf);
    }
#endif
    return stat(name, sbuf);
}

/* ========================================================================
 * Return a pointer to the 'z' suffix of a file name, or NULL. For all
 * systems, ".gz", ".z", ".Z", ".taz", ".tgz", "-gz", "-z" and "_z" are
 * accepted suffixes, in addition to the value of the --suffix option.
 * ".tgz" is a useful convention for tar.z files on systems limited
 * to 3 characters extensions. On such systems, ".?z" and ".??z" are
 * also accepted suffixes. For Unix, we do not want to accept any
 * .??z suffix as indicating a compressed file; some people use .xyz
 * to denote volume data.
 *   On systems allowing multiple versions of the same file (such as VMS),
 * this function removes any version suffix in the given name.
 */
local char *get_suffix(name)
    char *name;
{
    int nlen, slen;
    char suffix[MAX_SUFFIX+3]; /* last chars of name, forced to lower case */
    static char *known_suffixes[] =
       {z_suffix, ".gz", ".z", ".taz", ".tgz", "-gz", "-z", "_z",
#ifdef MAX_EXT_CHARS
          "z",
#endif
          NULL};
    char **suf = known_suffixes;

    if (strequ(z_suffix, "z")) suf++; /* check long suffixes first */

#ifdef SUFFIX_SEP
    /* strip a version number from the file name */
    {
	char *v = strrchr(name, SUFFIX_SEP);
 	if (v != NULL) *v = '\0';
    }
#endif
    nlen = strlen(name);
    if (nlen <= MAX_SUFFIX+2) {
        strcpy(suffix, name);
    } else {
        strcpy(suffix, name+nlen-MAX_SUFFIX-2);
    }
    strlwr(suffix);
    slen = strlen(suffix);
    do {
       int s = strlen(*suf);
       if (slen > s && suffix[slen-s-1] != PATH_SEP
           && strequ(suffix + slen - s, *suf)) {
           return name+nlen-s;
       }
    } while (*++suf != NULL);

    return NULL;
}


/* ========================================================================
 * Set ifname to the input file name (with a suffix appended if necessary)
 * and istat to its stats. For decompression, if no file exists with the
 * original name, try adding successively z_suffix, .gz, .z, -z and .Z.
 * For MSDOS, we try only z_suffix and z.
 * Return OK or ERROR.
 */
local int get_istat(iname, sbuf)
    char *iname;
    struct stat *sbuf;
{
    int ilen;  /* strlen(ifname) */
    static char *suffixes[] = {z_suffix, ".gz", ".z", "-z", ".Z", NULL};
    char **suf = suffixes;
    char *s;
#ifdef NO_MULTIPLE_DOTS
    char *dot; /* pointer to ifname extension, or NULL */
#endif

    strcpy(ifname, iname);

    /* If input file exists, return OK. */
    if (do_stat(ifname, sbuf) == 0) return OK;

    if (!decompress || errno != ENOENT) {
	perror(ifname);
	exit_code = ERROR;
	return ERROR;
    }
    /* file.ext doesn't exist, try adding a suffix (after removing any
     * version number for VMS).
     */
    s = get_suffix(ifname);
    if (s != NULL) {
	perror(ifname); /* ifname already has z suffix and does not exist */
	exit_code = ERROR;
	return ERROR;
    }
#ifdef NO_MULTIPLE_DOTS
    dot = strrchr(ifname, '.');
    if (dot == NULL) {
        strcat(ifname, ".");
        dot = strrchr(ifname, '.');
    }
#endif
    ilen = strlen(ifname);
    if (strequ(z_suffix, ".gz")) suf++;

    /* Search for all suffixes */
    do {
        s = *suf;
#ifdef NO_MULTIPLE_DOTS
        if (*s == '.') s++;
#endif
#ifdef MAX_EXT_CHARS
        strcpy(ifname, iname);
        /* Needed if the suffixes are not sorted by increasing length */

        if (*dot == '\0') strcpy(dot, ".");
        dot[MAX_EXT_CHARS+1-strlen(s)] = '\0';
#endif
        strcat(ifname, s);
        if (do_stat(ifname, sbuf) == 0) return OK;
	ifname[ilen] = '\0';
    } while (*++suf != NULL);

    /* No suffix found, complain using z_suffix: */
#ifdef MAX_EXT_CHARS
    strcpy(ifname, iname);
    if (*dot == '\0') strcpy(dot, ".");
    dot[MAX_EXT_CHARS+1-z_len] = '\0';
#endif
    strcat(ifname, z_suffix);
    perror(ifname);
    exit_code = ERROR;
    return ERROR;
}


/* ========================================================================
 * Check the magic number of the input file and update ofname if an
 * original name was given and to_stdout is not set.
 * Return the compression method, -1 for error, -2 for warning.
 * Set inptr to the offset of the next byte to be processed.
 * Updates time_stamp if there is one and --no-time is not used.
 * This function may be called repeatedly for an input file consisting
 * of several contiguous gzip'ed members.
 * IN assertions: there is at least one remaining compressed member.
 *   If the member is a zip file, it must be the only one.
 */
local int get_method(in)
    int in;        /* input file descriptor */
{
    uch flags;     /* compression flags */
    char magic[2]; /* magic header */
    ulg stamp;     /* time stamp */


    magic[0] = (char)get_byte();
    magic[1] = (char)get_byte();
    method = -1;                 /* unknown yet */
    part_nb++;                   /* number of parts in gzip file */
    header_bytes = 0;
    last_member = RECORD_IO;
    /* assume multiple members in gzip file except for record oriented I/O */

    if (memcmp(magic, GZIP_MAGIC, 2) == 0
        || memcmp(magic, OLD_GZIP_MAGIC, 2) == 0) {

	method = (int)get_byte();
	if (method != DEFLATED) {
	    fprintf(stderr,
		    "%s: %s: unknown method %d -- get newer version of gzip\n",
		    progname, ifname, method);
	    exit_code = ERROR;
	    return -1;
	}
	work = unzip;
	flags  = (uch)get_byte();

	if ((flags & ENCRYPTED) != 0) {
	    fprintf(stderr,
		    "%s: %s is encrypted -- get newer version of gzip\n",
		    progname, ifname);
	    exit_code = ERROR;
	    return -1;
	}
	if ((flags & CONTINUATION) != 0) {
	    fprintf(stderr,
	   "%s: %s is a a multi-part gzip file -- get newer version of gzip\n",
		    progname, ifname);
	    exit_code = ERROR;
	    if (force <= 1) return -1;
	}
	if ((flags & RESERVED) != 0) {
	    fprintf(stderr,
		    "%s: %s has flags 0x%x -- get newer version of gzip\n",
		    progname, ifname, flags);
	    exit_code = ERROR;
	    if (force <= 1) return -1;
	}
	stamp  = (ulg)get_byte();
	stamp |= ((ulg)get_byte()) << 8;
	stamp |= ((ulg)get_byte()) << 16;
	stamp |= ((ulg)get_byte()) << 24;
	if (stamp != 0 && !no_time) time_stamp = stamp;

	(void)get_byte();  /* Ignore extra flags for the moment */
	(void)get_byte();  /* Ignore OS type for the moment */

	if ((flags & CONTINUATION) != 0) {
	    unsigned part = (unsigned)get_byte();
	    part |= ((unsigned)get_byte())<<8;
	    if (verbose) {
		fprintf(stderr,"%s: %s: part number %u\n",
			progname, ifname, part);
	    }
	}
	if ((flags & EXTRA_FIELD) != 0) {
	    unsigned len = (unsigned)get_byte();
	    len |= ((unsigned)get_byte())<<8;
	    if (verbose) {
		fprintf(stderr,"%s: %s: extra field of %u bytes ignored\n",
			progname, ifname, len);
	    }
	    while (len--) (void)get_byte();
	}

	/* Get original file name if it was truncated */
	if ((flags & ORIG_NAME) != 0) {
	  /* Discard the old name */
	  char c; /* dummy used for NeXTstep 3.0 cc optimizer bug */
	  do {c=get_byte();} while (c != 0);

	} /* ORIG_NAME */

	/* Discard file comment if any */
	if ((flags & COMMENT) != 0) {
	    while (get_char() != 0) /* null */ ;
	}
	if (part_nb == 1) {
	    header_bytes = inptr + 2*sizeof(long); /* include crc and size */
	}

    } else if (memcmp(magic, PKZIP_MAGIC, 2) == 0 && inptr == 2
	    && memcmp((char*)inbuf, PKZIP_MAGIC, 4) == 0) {
	/* To simplify the code, we support a zip file when alone only.
         * We are thus guaranteed that the entire local header fits in inbuf.
         */
        inptr = 0;
	work = unzip;
	if (check_zipfile(in) != OK) return -1;
	/* check_zipfile may get ofname from the local header */
	last_member = 1;

    } else if (memcmp(magic, PACK_MAGIC, 2) == 0) {
	work = unpack;
	method = PACKED;

    } else if (memcmp(magic, LZW_MAGIC, 2) == 0) {
	work = unlzw;
	method = COMPRESSED;
	last_member = 1;

    } else if (memcmp(magic, LZH_MAGIC, 2) == 0) {
	work = unlzh;
	method = LZHED;
	last_member = 1;

    } else  { /* pass input unchanged */
      /*fprintf(stderr, "Assuming %s is already decompressed.\n", ifname);*/
      method = STORED;
      work = copy;
      inptr = 0;
      last_member = 1;
    }
    if (method >= 0) return method;

    if (part_nb == 1) {
	fprintf(stderr, "\n%s: %s: not in gzip format\n", progname, ifname);
	fprintf(stderr, "Assuming %s is already decompressed.\n", ifname);
	method = STORED;
	work = copy;
	inptr = 0;
	last_member = 1;
	return -1;
    } else {
	WARN((stderr, "\n%s: %s: decompression OK, trailing garbage ignored\n",
	      progname, ifname));
	return -2;
    }
}

/* ========================================================================
 * Display the characteristics of the compressed file.
 * If the given method is < 0, display the accumulated totals.
 * IN assertions: time_stamp, header_bytes and ifile_size are initialized.
 */
local void do_list(ifd, method)
    int ifd;     /* input file descriptor */
    int method;  /* compression method */
{
    ulg crc;  /* original crc */
    static int first_time = 1;
    static char* methods[MAX_METHODS] = {
        "store",  /* 0 */
        "compr",  /* 1 */
        "pack ",  /* 2 */
        "lzh  ",  /* 3 */
        "", "", "", "", /* 4 to 7 reserved */
        "defla"}; /* 8 */
    char *date;

    if (first_time && method >= 0) {
	first_time = 0;
	if (verbose)  {
	    printf("method  crc     date  time  ");
	}
	if (!quiet) {
	    printf("compressed  uncompr. ratio uncompressed_name\n");
	}
    } else if (method < 0) {
	if (total_in <= 0 || total_out <= 0) return;
	if (verbose) {
	    printf("                            %9lu %9lu ",
		   total_in, total_out);
	} else if (!quiet) {
	    printf("%9ld %9ld ", total_in, total_out);
	}
	display_ratio(total_out-(total_in-header_bytes), total_out, stdout);
	/* header_bytes is not meaningful but used to ensure the same
	 * ratio if there is a single file.
	 */
	printf(" (totals)\n");
	return;
    }
    crc = (ulg)~0; /* unknown */
    bytes_out = -1L;
    bytes_in = ifile_size;

#if RECORD_IO == 0
    if (method == DEFLATED && !last_member) {
        /* Get the crc and uncompressed size for gzip'ed (not zip'ed) files.
         * If the lseek fails, we could use read() to get to the end, but
         * --list is used to get quick results.
         * Use "gunzip < foo.gz | wc -c" to get the uncompressed size if
         * you are not concerned about speed.
         */
        bytes_in = (long)lseek(ifd, (off_t)(-8), SEEK_END);
        if (bytes_in != -1L) {
            uch buf[8];
            bytes_in += 8L;
            if (read(ifd, (char*)buf, sizeof(buf)) != sizeof(buf)) {
                read_error();
            }
            crc       = LG(buf);
	    bytes_out = LG(buf+4);
	}
    }
#endif /* RECORD_IO */
    date = ctime((time_t*)&time_stamp) + 4; /* skip the day of the week */
    date[12] = '\0';               /* suppress the 1/100sec and the year */
    if (verbose) {
        printf("%5s %08lx %11s ", methods[method], crc, date);
    }
    printf("%9ld %9ld ", bytes_in, bytes_out);
    if (bytes_in  == -1L) {
	total_in = -1L;
	bytes_in = bytes_out = header_bytes = 0;
    } else if (total_in >= 0) {
	total_in  += bytes_in;
    }
    if (bytes_out == -1L) {
	total_out = -1L;
	bytes_in = bytes_out = header_bytes = 0;
    } else if (total_out >= 0) {
	total_out += bytes_out;
    }
    display_ratio(bytes_out-(bytes_in-header_bytes), bytes_out, stdout);
    printf(" %s\n", ofname);
}

/* ========================================================================
 * Return true if the two stat structures correspond to the same file.
 */
local int same_file(stat1, stat2)
    struct stat *stat1;
    struct stat *stat2;
{
    return stat1->st_ino   == stat2->st_ino
	&& stat1->st_dev   == stat2->st_dev
#ifdef NO_ST_INO
        /* Can't rely on st_ino and st_dev, use other fields: */
	&& stat1->st_mode  == stat2->st_mode
	&& stat1->st_uid   == stat2->st_uid
	&& stat1->st_gid   == stat2->st_gid
	&& stat1->st_size  == stat2->st_size
	&& stat1->st_atime == stat2->st_atime
	&& stat1->st_mtime == stat2->st_mtime
	&& stat1->st_ctime == stat2->st_ctime
#endif
	    ;
}

/* ========================================================================
 * Return true if a file name is ambiguous because the operating system
 * truncates file names.
 */
local int name_too_long(name, statb)
    char *name;           /* file name to check */
    struct stat *statb;   /* stat buf for this file name */
{
    int s = strlen(name);
    char c = name[s-1];
    struct stat	tstat; /* stat for truncated name */
    int res;

    tstat = *statb;      /* Just in case OS does not fill all fields */
    name[s-1] = '\0';
    res = stat(name, &tstat) == 0 && same_file(statb, &tstat);
    name[s-1] = c;
    Trace((stderr, " too_long(%s) => %d\n", name, res));
    return res;
}

/* ========================================================================
 * Shorten the given name by one character, or replace a .tar extension
 * with .tgz. Truncate the last part of the name which is longer than
 * MIN_PART characters: 1234.678.012.gz -> 123.678.012.gz. If the name
 * has only parts shorter than MIN_PART truncate the longest part.
 * For decompression, just remove the last character of the name.
 *
 * IN assertion: for compression, the suffix of the given name is z_suffix.
 */
local void shorten_name(name)
    char *name;
{
  return;
}


/* ========================================================================
 * Free all dynamically allocated variables and exit with the given code.
 */
local void do_exit(exitcode)
    int exitcode;
{
    static int in_exit = 0;

    if (in_exit) exit(exitcode);
    in_exit = 1;
    if (env != NULL)  free(env),  env  = NULL;
    if (args != NULL) free((char*)args), args = NULL;
    FREE(inbuf);
    /* FREE(outbuf);  */
    FREE(d_buf);
    FREE(window);
    FREE(tab_prefix);
    exit(exitcode);
}

