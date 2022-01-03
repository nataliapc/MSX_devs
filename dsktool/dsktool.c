#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <unistd.h>
#include <ctype.h>
#include <malloc.h>
#include <string.h>
#include <time.h>
#include <libgen.h>
#include <sys/stat.h>
#include "msxboot.h"

#ifdef WIN32
#   define localtime_r(T,Tm) (localtime_s(Tm,T) ? NULL : Tm)
#endif

//Format types
#define FORMAT_360		360
#define FORMAT_720		720
#define FORMAT_1440		1440
#define FORMAT_2880		2880

//Params for load_dsk(...)
#define NO_ERROR     0
#define ERROR        1
#define READ_ALL     0
#define READ_BOOTFAT 1

#define fat12odd_value(pos)		(((int)(fat[pos+2]))<<4)+(fat[pos+1]>>4)
#define fat12even_value(pos)	((((int)(fat[pos+1]&0xF))<<8)+fat[pos])

// MEDIA DESCRIPTOR TABLE
// FAT-ID             F8   F9   FA   FB   FC   FD   FE   FF
// Format code        891  892  881  882  491  492  481  482
// Directory entries  112  112  112  112  64   112  64   112
// Sectors / FAT      2    3    1    2    2    2    1    1
// Sectors / track    9    9    8    8    9    9    8    8
// Heads              1    2    1    2    1    2    1    2
// Sectors / head     80   80   80   80   40   40   40   40
// Sectors / cluster  2    2    2    2    1    2    1    2
// Total sectors      720  1440 640  1280 360  720  320  640
// Total clusters     360  720  320  640  360  360  320  320
// Total Kbytes       360  720  320  640  180  360  160  320

#pragma pack(push,1)

typedef struct {
	uint8_t   dummy[3];				// 0x000 [3]  Dummy jump instruction (e.g. 0xEB 0xFE 0x90)
	uint8_t   oemname[8];			// 0x003 [8]  OEM Name (padded with spaces 0x20)
	uint16_t  bytesPerSector;		// 0x00B [2]  Bytes per logical sector in powers of two (e.g. 512 0x0200)
	uint8_t   sectorsPerCluster;	// 0x00D [1]  Logical sectors per cluster (e.g. 2 0x02)
	uint16_t  reservedSectors;		// 0x00E [2]  Count of reserved logical sectors (e.g. 1 0x0001)
	uint8_t   numberOfFATs;			// 0x010 [1]  Number of File Allocation Tables (e.g. 2 0x02)
	uint16_t  maxDirectoryEntries;	// 0x011 [2]  Maximum number of FAT12 or FAT16 root directory entries (e.g. 112 0x0070)
	uint16_t  totalSectors;			// 0x013 [2]  Total logical sectors (e.g. 1440 0x05a0)
	uint8_t   mediaDescriptor;		// 0x015 [1]  Media descriptor: 0xf9:3.5"720Kb | 0xf8:3.5"360Kb (see previous table)
	uint16_t  sectorsPerFAT;		// 0x016 [2]  Logical sectors per FAT (e.g. 3 0x0003)
	uint16_t  sectorsPerTrack;		// 0x018 [2]  Physical sectors per track for disks with CHS geometry (e.g. 9 0x0009)
	uint16_t  numberOfHeads;		// 0x01A [2]  Number of heads (e.g. 2 0x0002)
	uint16_t  hiddenSectors;		// 0x01C [2]  Count of hidden sectors preceding the partition that contains this FAT volume (e.g. 0 0x0000)
	uint16_t  codeEntryPorint;		// 0x01E [2]  MSX-DOS 1 code entry point for Z80 processors into MSX boot code. This is where MSX-DOS 1 machines jump to when passing control to the boot sector.
	uint8_t   bootCode[482];		// 0x020 [-]  This location overlaps with BPB formats since DOS 3.2 or the x86 compatible boot sector code of IBM PC compatible boot sectors and will lead to a crash on the MSX machine unless special precautions have been taken such as catching the CPU in a tight loop here (opstring 0x18 0xFE for JR 0x01E).
} bootsec_t;

typedef struct {
	char      name[8];				// 0x000 [8]  Short file name (padded with spaces). First char '0xE5' for deleted files.
	char      ext[3];				// 0x008 [3]  Short file extension (padded with spaces)
	uint8_t   attr;					// 0x00B [1]  File Attributes. Mask: 0x01:ReadOnly | 0x02:Hidden | 0x04:System | 0x08:Volume | 0x10:Directory | 0x20:Archive
	uint8_t   unused1;				// 0x00C [1]  MSX-DOS 2: For a deleted file, the original first character of the filename
	uint8_t   unused2;				// 0x00D [1]
	uint16_t  ctime;				// 0x00E [2]  Create time: #0-4:Seconds/2 #5-10:Minuts #11-15:Hours
	uint16_t  cdate;				// 0x010 [2]  Create date: #0-4:Day #5-8:Month #9-15:Year(0=1980)
	uint16_t  unused3;				// 0x012 [2]
	uint16_t  unused4;				// 0x014 [2]
	uint16_t  mtime;				// 0x016 [2]  Last modified time: #0-4:Seconds/2 #5-10:Minuts #11-15:Hours
	uint16_t  mdate;				// 0x018 [2]  Last modified date: #0-4:Day #5-8:Month #9-15:Year(0=1980)
	uint16_t  cluini;				// 0x01A [2]  Initial cluster for this file
	uint32_t  fsize;				// 0x01C [4]  File size in bytes
} direntry_t;

typedef struct {
	char     name[9];
	char     ext[4];
	uint32_t size;
	uint16_t hour,min,sec;
	uint16_t day,month,year;
	uint32_t first;
	uint32_t pos;
	uint16_t attr;
} fileinfo_t;


/*
ADVH Format

   0 -  511	(1)	Boot sector
 512 - 3583	(6)	Root dir (16 bytes each entry)
3584 - 6655 (6)	???
6656 - ...		Data

FILENAME                EXT       SECINI SECSIZE

A  D  V  H              Z  8  0   
41 44 56 48 20 20 20 20 5A 38 30  12 00  12      00 00
E  Y  E  M  S  X        C  O  M
45 59 45 4D 53 58 20 20 43 4F 4D  24 00  05      00 00
K  A  N  J  I  6        F  N  T
4B 41 4E 4A 49 36 20 20 46 4E 54  29 00  20      00 00
0  0  0                 M  E  S
30 30 30 20 20 20 20 20 4D 45 53  49 00  04      00 00
0  0  1                 M  E  S
30 30 31 20 20 20 20 20 4D 45 53  4D 00  01      00 00
...
*/
typedef struct {
	uint8_t  name[8];
	uint8_t  ext[3];
	uint16_t secini;
	uint16_t secsize;
	uint8_t  reserved;
} advhDirentry_t;

#pragma pack(pop)

uint16_t    dskFormat = FORMAT_720;
uint8_t    *dskimage;
bootsec_t  *bootsec;

uint8_t    *fat;
direntry_t *rootdir;
uint8_t    *cluster;
uint32_t    disksize;
uint32_t    fatelements;
uint32_t    availsectors;
uint32_t    bytespercluster;

uint8_t     isADVH = 0;
advhDirentry_t *rootADVH;


// Create a disk in memory of specified format
void create_boot() {

	//Copy default boot sector
	bootsec = (bootsec_t *)malloc(512);
	memcpy(bootsec, msxboot720, sizeof(msxboot720));


	//Check format size & personalize boot params
	switch (dskFormat) {
		case 360:
			bootsec->totalSectors = 720;
			bootsec->mediaDescriptor = 0xF8;
			bootsec->sectorsPerFAT = 2;
			bootsec->numberOfHeads = 1;
			break;
		case 720:
			break;
		case 1440:
			bootsec->sectorsPerCluster = 1;
			bootsec->maxDirectoryEntries = 224;
			bootsec->totalSectors = 2880;
			bootsec->mediaDescriptor = 0xF0;
			bootsec->sectorsPerFAT = 9;
			bootsec->sectorsPerTrack = 18;
			break;
		case 2880:
			bootsec->maxDirectoryEntries = 224;
			bootsec->totalSectors = 5760;
			bootsec->mediaDescriptor = 0xF0;
			bootsec->sectorsPerFAT = 9;
			bootsec->sectorsPerTrack = 36;
			break;
		default:
			puts("ERROR bad format size. Only 360, 720, 1440, 2880 are supported!\n");
			exit(1);
	}
}


// Load the specified DSK file into memory
void load_dsk (char *name, uint8_t  onlybootfat, uint8_t  error) {
	FILE *file;

	file = fopen(name, "rb");

	//Boot sector
	if (file==NULL) {
		create_boot();
	} else {
		bootsec = (bootsec_t *)malloc(512);
		if (!fread(bootsec, 512, 1, file)) {
			printf("ERROR bad .DSK image\n");
			exit (2);
		}
		rewind(file);
	}
	disksize = bootsec->bytesPerSector * bootsec->totalSectors;
	bytespercluster = bootsec->bytesPerSector*bootsec->sectorsPerCluster;

	//Allocate memory for disk image
	dskimage = (uint8_t *) malloc(disksize);
	memset(dskimage, 0, disksize);

	fat = dskimage + bootsec->bytesPerSector * bootsec->reservedSectors;
	rootdir = (direntry_t*) (fat + bootsec->bytesPerSector * (bootsec->sectorsPerFAT * bootsec->numberOfFATs));
	cluster = (uint8_t *)&(rootdir[bootsec->maxDirectoryEntries]);
	availsectors = bootsec->totalSectors - bootsec->reservedSectors - bootsec->sectorsPerFAT * bootsec->numberOfFATs;
	availsectors -= bootsec->maxDirectoryEntries * sizeof(direntry_t) / bootsec->bytesPerSector;
	fatelements = availsectors / bootsec->sectorsPerCluster;

	if (file==NULL) {
		if (error==ERROR) {
			printf ("ERROR in .DSK file\n");
			exit (2);
		}
		memset(dskimage, 0, disksize);
		memcpy(dskimage, bootsec, 512);
		fat[0]=bootsec->mediaDescriptor;
		fat[1]=0xFF;
		fat[2]=0xFF;
	} else {
		uint64_t sizetoread = onlybootfat ? ((uint64_t)cluster-(uint64_t)dskimage) : disksize;
		if (!fread(dskimage, sizetoread, 1, file)) {
			printf("ERROR bad .DSK image\n");
			exit (2);
		}
		fclose (file);

		rootADVH = (advhDirentry_t*) (dskimage + 512);
	}
	bootsec = (bootsec_t*) dskimage;

	printf("Disk image size:  %uKb\n%s\n\n", disksize/1024, isADVH?"ADVH Format":"Standard format");
}

// Go to the next rootdirectory entry
int next_link (uint16_t link) {
	uint32_t pos;

	pos=(link>>1)*3;
	if (link&1)
		return fat12odd_value(pos);
	else 
		return fat12even_value(pos);
}

// Remove a directory entry
int remove_link (uint16_t link) {
	uint32_t pos;
	uint16_t current;

	pos=(link>>1)*3;
	if (link&1) {
		current = fat12odd_value(pos);
		fat[pos+2]=0;
		fat[pos+1]&=0xF;
		return current;
	} else {
		current = fat12even_value(pos);
		fat[pos]=0;
		fat[pos+1]&=0xF0;
		return current;
	}
}

// Store the fat table entry for a specified link
void store_fat (uint16_t link, uint16_t next) {
	uint32_t pos;

	pos=(link>>1)*3;
	if (link&1) {
		fat[pos+2]=next>>4;
		fat[pos+1]&=0xF;
		fat[pos+1]|=(next&0xF)<<4;
	} else {
		fat[pos]=next&0xFF;
		fat[pos+1]&=0xF0;
		fat[pos+1]|=next>>8;
	}
}

// Get the information for a specified directory entry
fileinfo_t *getfileinfo (uint16_t entrypos) {
	fileinfo_t *file;
	direntry_t *dir;
	uint32_t    aux;
	uint32_t    i;

	dir = &rootdir[entrypos];

	// Filter entries by name
	char *name = (char *)dir->name;
	for (i=0; i<11; i++) {
		if (*name < 0x20 || *name >= 0x80) return NULL;
		name++;
	}

	if (dir->cluini >= bootsec->totalSectors / bootsec->sectorsPerCluster) return NULL;
	if (dir->fsize >= disksize) return NULL;

	// Fill fileinfo struct
	file = (fileinfo_t*) malloc (sizeof(fileinfo_t));
	for (i=0; i<8; i++)
		file->name[i] = dir->name[i]==0x20?0:dir->name[i];
	file->name[8]=0;

	for (i=0; i<3; i++)
		file->ext[i] = dir->ext[i]==0x20?0:dir->ext[i];
	file->ext[3]=0;

	file->size = dir->fsize;

	aux = dir->mtime;
	file->sec = (aux & 0x1F)<<1;
	file->min = (aux >> 5)&0x3F;
	file->hour = (aux >> 11);

	aux = dir->mdate;
	file->day = (aux & 0x1F);
	file->month = (aux >> 5) & 0xF;
	file->year = 1980 + (aux >> 9);

	file->first = dir->cluini;
	file->pos=entrypos;
	file->attr = dir->attr;

	return file;
}

// Get the information for a specified ADVH directory entry
fileinfo_t *getfileinfoadvh(uint16_t entrypos) {
	fileinfo_t *file;
	advhDirentry_t *dir;
	uint32_t i;

	dir = &((advhDirentry_t*) &dskimage[512+16])[entrypos];
	if (dir->name[0]==0xff) return NULL;
	//Obtenemos datos
	file = (fileinfo_t*) malloc (sizeof(fileinfo_t));
	for (i=0; i<8; i++)
		file->name[i] = dir->name[i]==0x20?0:dir->name[i];
	file->name[8]=0;

	for (i=0; i<3; i++)
		file->ext[i] = dir->ext[i]==0x20?0:dir->ext[i];
	file->ext[3]=0;
	file->first = dir->secini * 512;
	file->size = dir->secsize * 512;
	file->pos = entrypos;
	
	return file;
}

// Calculate the available space on the DSK
uint32_t bytes_free (void) {
	uint32_t avail=0;
	uint32_t i;

	for (i=2; i<2+fatelements; i++) {
		if (!next_link(i)) avail++;
	}
	return avail*bytespercluster;
}

// List the root directory of a DSK
void list_dsk (void) {
	int i, num = 0;
	fileinfo_t *file;
	char name[20],date[30],time[30],size[30],attrib[5];

	// Print Disk Volume Name
	for (i=0; i<8; i++)
		name[i]=dskimage[3+i];
	name[8]=0;
	printf ("Volume Name:  %s\n\n",name);

	puts ("Name         Bytes    Date       Time     Attr\n"
		  "============ ======== ========== ======== ====");

	// Iteract all entries in the root directory table
	for (i=0; i<bootsec->maxDirectoryEntries; i++) {
		file = getfileinfo(i);
		if (file != NULL) {
			num++;
			if (file->ext[0]) 
				sprintf (name,"%s.%s",file->name,file->ext);
			else
				strcpy (name, file->name);
			sprintf (size,"%7u",file->size);
			if (file->attr&0x8) strcpy (size,"  <VOL>");
			if (file->attr&0x10) strcpy (size,"  <DIR>");
			sprintf (date,"%u/%02u/%u",file->day,file->month,file->year);
			sprintf (time,"%u:%02u:%02u",file->hour,file->min,file->sec);
			sprintf (attrib, "%c%c%c%c", file->attr&0x1?'R':'-', file->attr&0x2?'H':'-', file->attr&0x4?'S':'-', file->attr&0x20?'A':'-');
			printf ("%-13s %s %10s %8s %s\n", name, size, date, time, attrib);
			free (file);
		}
	}
	if (!num) {
		puts("*** Disk is empty ***");
	}
	puts("============ ======== ========== ======== ====");
	printf ("\n%u bytes free\n\n",bytes_free ());
}

// List the directory of a DSK ADVH
void list_advhdsk (void) {
	uint32_t i;
	fileinfo_t *file;
	char name[20];

	for (i=0; i<8; i++)
		name[i]=dskimage[3+i];
	name[8]=0;
	printf ("Name of volume:   %s\n\n",name);
	for (i=0; i<190; i++) {
		file = getfileinfoadvh(i);
		if (file->name[0]==0xFF) break;
		printf ("%-8s.%-3s   [Diskfile Offset:%7d]  %7u bytes\n", file->name, file->ext, file->first, file->size);
	}
	puts("");
}

// Find the directory entry matching the supplied filename
int match (fileinfo_t *file, char *name) {
	char *p=file->name;
	uint8_t  status=0;
	uint16_t i;

	//name (8 chars)
	for (i=0; i<8; i++) {
		if (!*name) break;
		if (*name=='*') {
			status=1;
			name++;
			break;
		}
		if (*name=='.')
			break;
		if (*name=='?' && *p) continue;
		if (toupper(*name++)!=toupper(*p++))
			return 0;
	}
	if (!status && i<8 && *p!=0) 
		return 0;

	//ext (3 chars)
	p = file->ext;
	if (!*name && !*p) return 1;
	if (*name++!='.') return 0;
	for (i=0; i<3; i++) {
		if (!*name) break;
		if (*name=='*')
			return 1;
		if (*name=='?' && *p) continue;
		if (toupper(*name++)!=toupper(*p++))
			return 0;
	}
	if (!*name && !*p) return 1;

	return 0;
}

// Work through the directory tree
void parse_tree (char *name, void (*action)(fileinfo_t *)) {
	uint32_t i;
	uint16_t max = isADVH ? 190 : bootsec->maxDirectoryEntries;
	fileinfo_t *file;

	for (i=0; i<max; i++) {
		file = isADVH ? getfileinfoadvh(i) : getfileinfo(i);
		if (file!=NULL) {
			if (match(file,name)) {
				action(file);
			}
			free(file);
		}
	}
}

// Search the directory for a specified file or a default wildcard search
void parse_dsk (int argc, char **argv, void (*action)(fileinfo_t *)) {
	int i;

	if (argc==3) {
		parse_tree((char *)"*.*", action);
	} else {
		for (i=3; i<argc; i++) {
			parse_tree(argv[i], action);
		}
	}
}

// Extract a file from the DSK
void extract (fileinfo_t *file) {
	uint8_t  *buffer,*p;
	FILE *fileid;
	char name[20];
	uint16_t current;

	printf ("extracting %s.%s\n",file->name,file->ext);
	buffer = (uint8_t *) malloc ((file->size+bytespercluster-1)&(~(bytespercluster-1)));
	memset (buffer,0x1a,file->size);
	if (file->ext[0]) 
		sprintf (name,"%s.%s",file->name,file->ext);
	else
		strcpy (name, file->name);
	fileid = fopen (name, "w+b");
	current=file->first;
	p=buffer;
	do {
		memcpy (p,cluster+(current-2)*bytespercluster, bytespercluster);
		p += bytespercluster;
		current=next_link (current);
	} while (current!=0xFFF);
	fwrite (buffer, file->size, 1, fileid);
	fclose (fileid);
	free (buffer);
}

// Extract a file from the ADVH DSK
void extract_advh (fileinfo_t *file) {
	FILE *fileid;
	char name[20];

	printf ("extracting %s.%s\n",file->name,file->ext);
	if (file->ext[0]) 
		sprintf (name,"%s.%s",file->name,file->ext);
	else
		strcpy (name, file->name);
	fileid = fopen (name, "w+b");
	fwrite (&dskimage[file->first], file->size, 1, fileid);
	fclose (fileid);
}

// Show file clusters info from the DSK
void file_clusters_info (fileinfo_t *file) {
	uint16_t current = file->first;
	long offset;

	printf ("File info for %s.%s (%d bytes)\n", file->name, file->ext, file->size);
	do {
		offset = cluster-dskimage+(current-2)*bytespercluster;
		printf("  Cluster: %04Xh (%d) | Diskfile Offset: %04lXh-%04lXh (%ld-%ld)\n", current, current, offset, offset+bytespercluster-1, offset, offset+bytespercluster-1);
		current=next_link (current);
	} while (current!=0xFFF);
	printf("\n");
}

// Wipe a DSK by clearing the directory
void wipe (fileinfo_t *file) {
	uint32_t current;

	current=file->first;
	do {
		current=remove_link (current);
	} while (current!=0xFFF);
	(rootdir[file->pos]).name[0] = 0xE5;
}

// Remove a file from the DSK
void deleted(fileinfo_t *file) {
	printf ("deleting %s.%s\n",file->name,file->ext);
	wipe (file);
}

// Write the in memory copy to the DSK file
void flush_dsk (char *name) {
	FILE *file;

	memcpy (fat + bootsec->bytesPerSector * bootsec->sectorsPerFAT, fat, bootsec->bytesPerSector * bootsec->sectorsPerFAT);
	file=fopen (name, "w+b");
	fwrite (dskimage, 1, disksize, file);
	fclose (file);
}

// Get the 1st free directory
int get_free (void) {
	uint32_t i;

	for (i=2; i<2+fatelements; i++) {
		if (!next_link (i)) return i;
	}
	printf ("Internal error\n");
	exit (5);
}

// Get the next free sector
int get_next_free (void) {
	uint32_t i;
	uint8_t  status=0;

	for (i=2; i<2+fatelements; i++) {
		if (!next_link (i)) {
			if (status) 
				return i;
			else
				status=1;
		}
	}
	printf ("Internal error\n");
	exit (5);
}

// Add a single file to the DSK
void add_single_file(char *name, char *pathname) {
	FILE       *fileid;
	uint32_t    i;
	uint32_t    total;
	uint8_t     found=0;
	fileinfo_t *file;
	direntry_t *dir;
	uint8_t    *buffer, *buffaux;
	uint32_t    size;
	struct stat attr;
	struct tm   ti;
	uint32_t    first;
	uint32_t    current;
	uint32_t    next;
	size_t      read;
	char       *p;
	char        fullname[250];

	sprintf(fullname, "%s/%s", pathname, name);

	stat(fullname, &attr);
	fileid = fopen (fullname, "rb");

	//Add new file or Update existing?
	for (i=0; i<bootsec->maxDirectoryEntries; i++) {
		if ((file=getfileinfo(i)) != NULL) {
			if (match(file, name)) {
				found = 1;
				wipe(file);
			}
			free(file);
		}
	}
	if (found)
		printf("updating ");
	else
		printf("  adding ");
	p = name;
	while (*p) {
		putchar(toupper(*p++));
	}
	putchar('\n');

	//Not enough space for the file
	if ((size=attr.st_size)>bytes_free()) {
		printf ("disk full\n");
		exit (4);
	}

	//Search first empty directory entry
	dir = rootdir;
	for (i=0; i<bootsec->maxDirectoryEntries; i++) {
		if (dir->name[0]<0x20 || dir->name[0]>=0x80) {
			break;
		}
		dir++;
	}
	//Directory entries list is full
	if (i==bootsec->maxDirectoryEntries) {
		printf ("Root directory full\n");
		exit (6);
	}

	//Reading data file
	uint32_t bufsize = (size+bytespercluster-1)&(~(bytespercluster-1));
	buffer = buffaux = (uint8_t *) malloc(bufsize);
	memset(buffer, 0, bufsize);
	read = fread (buffer, 1, size, fileid);
	if (read != size) {
		printf("ERROR reading file '%s'\n", name);
		exit(0);
	}
	fclose (fileid);

	total=(size+bytespercluster-1)/bytespercluster;
	current=first=get_free ();

	//Saving data to DSK clusters
	for (i=0; i<total;) {
		memcpy(cluster+(current-2)*bytespercluster, buffaux, bytespercluster);
		buffaux+=bytespercluster;
		if (++i==total)
			next=0xFFF;
		else
			next=get_next_free ();
		store_fat (current,next);
		current=next;
	}
	free(buffer);

	//Adding directory entry
	memset(dir, 0, 32);
	memset(dir, 0x20, 11);
	i=0;
	for (p=name;*p;p++) {
		if (*p=='.') {
			i=8;
			continue;
		}
		dir->name[i++] = toupper(*p);
	}
	dir->cluini = first;
	dir->fsize = size;

	localtime_r(&(attr.st_mtime), &ti);
	dir->mtime = (ti.tm_sec>>1)+(ti.tm_min<<5)+(ti.tm_hour<<11);
	dir->mdate = (ti.tm_mday)+(ti.tm_mon<<5)+((ti.tm_year+1900-1980)<<9);
	if (!found) {
		dir->ctime = dir->mtime;
		dir->cdate = dir->mdate;
	}
}

int globerr(const char *path, int errno)
{
	printf("ERROR '%s' reading files in: %s\n", strerror(errno), path);
	exit(0);
}

// Add files specified by a wildcard to the DSK
void add_files(char *name) {
	if (access(name, F_OK) == -1) {
		printf("ERROR reading '%s' file\n", name);
		exit(0);
	}
	add_single_file(basename(name), dirname(name));
}

// Add files from an argument list to the DSK
void add_to_dsk (int argc, char **argv) {
	int i;
  
	for (i=3; i<argc; i++) {
		add_files(argv[i]);
	}
}

// Show floppy disk info
void show_info() {
	printf("BOOT SECTOR INFO:\n");
	printf("    OEM Name...............   \"%8s\"\n", bootsec->oemname);
	printf("  BIOS PARAMETER BLOCK:\n");
	printf("    Bytes x Sector......... % 5d bytes\n", bootsec->bytesPerSector);
	printf("    Sectors x Cluster...... % 5d sectors\n", bootsec->sectorsPerCluster);
	printf("    Reserved Sectors....... % 5d sectors\n", bootsec->reservedSectors);
	printf("    Number of FATs......... % 5d\n", bootsec->numberOfFATs);
	printf("    Max root entries....... % 5d files\n", bootsec->maxDirectoryEntries);
	printf("    Total Sectors.......... % 5d sectors\n", bootsec->totalSectors);
	printf("    Media descriptor.......   %02Xh\n", bootsec->mediaDescriptor);
	printf("    Sectors x FAT.......... % 5d sectors\n", bootsec->sectorsPerFAT);
	printf("    Sectors x Track........ % 5d sectors\n", bootsec->sectorsPerTrack);
	printf("    Number of Heads........ % 5d heads\n", bootsec->numberOfHeads);
	printf("    Hidden Sectors......... % 5d sectors\n", bootsec->hiddenSectors);
	printf("\n");

	long fatini = fat - dskimage;
	long fatsize = bootsec->sectorsPerFAT * bootsec->bytesPerSector;

	long rootini = (uint8_t *)rootdir-dskimage;
	long clusterini = cluster - dskimage;

	printf("Boot sector offset.........       0 (size: 512 bytes)\n");
	printf("FAT#1 offset............... %7ld-%ld (size: %ld bytes)\n", fatini, fatini+fatsize-1, fatsize);

	if (bootsec->numberOfFATs > 1) {
		printf("FAT#2 offset............... %7ld-%ld (size: %ld bytes) ", fatini+fatsize, fatini+fatsize*2-1, fatsize);
		char fatfail = 0;
		for (int i=0; i<fatsize; i++) {
			if (dskimage[fatini+i] != dskimage[fatini+fatsize+i]) fatfail++;
		}
		if (fatfail)
			printf("[ERROR not equal FATs]\n");
		else
			printf("[OK identical FAT copy]\n");
	}
	printf("Root dir offset............ %7ld-%ld (size: %ld bytes)\n", rootini, clusterini-1, clusterini-rootini);
	printf("Clusters offset............ %7ld-%ld (size: %ld bytes)\n", clusterini, (long)disksize-1, disksize-clusterini);

	printf("\n%u bytes free\n\n",bytes_free());
}

// Application entry point
int main (int argc, char **argv) {
	puts("DskTool v1.40 (C) 1998 by Ricardo Bittencourt\n"
	     "Utility to manage MSX DOS 1.0 diskette images (3.5\"360/720Kb).\n"
	     "(2010) Updated by Tony Cruise\n"
	     "(2017-2019) Updated by NataliaPC\n"
	     "This file is under GNU GPL, read COPYING for details\n");

	if (argc<3) {
		puts("Usage: dsktool <command> [option] <DSK_file> [files]\n"
			 "\n"
		     "Commands:\n"
		     "\tc N   Create a floppy image [where N:360,720,1440,2880]\n"
		     "\ti     Show floppy info\n"
		     "\tl[h]  List contents of .DSK\n"
		     "\te[h]  Extract files from .DSK\n"
		     "\ta[h]  Add files to .DSK\n"
		     "\td     Delete files from .DSK\n"
		     "\tf     File clusters info\n"
		     "\to[h]  Get file info for a raw disk offset\n"
		     "\n"
		     "    Note: optional [H] suffix change to ADVH filesystem mode.\n"
		     "\n"
		     "Examples:\n"
		     "\tdsktool c 360 TALKING.DSK\n"
		     "\tdsktool i TALKING.DSK\n"
		     "\tdsktool l TALKING.DSK\n"
		     "\tdsktool lh DRAGON.DSK\n"
		     "\tdsktool e TALKING.DSK FUZZ*.*\n"
		     "\tdsktool a TALKING.DSK MSXDOS.SYS COMMAND.COM\n"
		     "\tdsktool ah DRAGON.DSK M*.COM\n"
		     "\tdsktool d TALKING.DSK *.BAS *.BIN\n"
		     "\tdsktool f TALKING.DSK FILE.EXT\n"
		     "\tdsktool o TALKING.DSK 307712\n"
		     "\n");
		exit (1);
	}
	isADVH = (toupper(argv[1][1])=='H');
	switch (toupper(argv[1][0])) {
		case 'C':
			if (isADVH) {
				puts("CH Not supported");
			} else {
				dskFormat = atoi(argv[2]);
				load_dsk(NULL, READ_ALL, NO_ERROR);
				flush_dsk(argv[3]);
			}
			puts("*** New Disk image created ***\n");
			break;
		case 'L':
			load_dsk(argv[2], READ_BOOTFAT, ERROR);
			if (isADVH) {
				list_advhdsk();
			} else {
				list_dsk();
			}
			break;
		case 'E':
			load_dsk(argv[2], READ_ALL, ERROR);
			if (isADVH) {
				parse_dsk(argc, argv, extract_advh);
			} else {
				parse_dsk(argc, argv, extract);
			}
			break;
		case 'D':
			load_dsk(argv[2], READ_ALL, ERROR);
			if (isADVH) {
				puts("DH Not supported!\n");
			} else {
				parse_dsk(argc, argv, deleted);
				flush_dsk(argv[2]);
			}
			break;
		case 'A':
			load_dsk(argv[2], READ_ALL, NO_ERROR);
			if (isADVH) {
				puts("AH Not implemented yet!\n");
			} else {
				add_to_dsk(argc, argv);
				flush_dsk(argv[2]);
			}
			break;
		case 'I':
			load_dsk(argv[2], READ_BOOTFAT, ERROR);
			if (isADVH) {
				puts("IH Not supported!\n");
			} else {
				show_info();
			}
			break;
		case 'F':
			load_dsk(argv[2], READ_BOOTFAT, ERROR);
			if (isADVH) {
				puts("FH Not supported!\n");
			} else {
				parse_dsk(argc, argv, file_clusters_info);
			}
			break;
		case 'O':
			load_dsk(argv[2], READ_BOOTFAT, ERROR);
			if (isADVH) {
				puts("OH Not implemented yet!\n");
			} else {
				puts("O Not implemented yet!\n");
			}
			break;
		default:
			printf("Command not supported\n");
			exit (3);
	}
	return 0;
}
