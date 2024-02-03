/* 	bin2hex.c
rewritten to hex2bin.c by Herb Johnson 2014
based on code provided by Bill Beech

DESCRIPTION
	converts  intel hex format file (*.hex)to binary file (*.bin)

MODIFICATION HISTORY
	07 Jan 89 -- original
	22 May 08 -- modified for uniformity with other tools
Removed	28 Sep 09 -- modified to allow any file name
	Nov 2014 - modified to read hex and write bin
    Feb 1 2017 - display hex start addr, test for discontinuous hex, fill with zeros.
			out of order hex, exit
    Jun 18 2020 - change fill to 0xFF, trap too-long filenames
    Jan 21 2021 - problem when hex record ends with spaces, fixed
*/

#include	<stdio.h>
#include	<string.h>
#include	<stdlib.h>

#define	LEN	120
#define FILNAMLEN 40
#define CR 0x0D
#define LF 0x0A
#define SP 0x20


int next(FILE *, char *);
int hexhexint(char []);

int main(int argc, char **argv)
    {
    char to[FILNAMLEN], from[FILNAMLEN], buf[LEN + 1];
    // char zeros[10];   zero fill
    int addr = 0, lastaddr = -1, chk, i, j, n, len, hexlen;
	int recnum = 0;
    FILE *fp1, *fp2;

    // strcpy(zeros,"000000");

    if (argc != 2) {
	printf("hex2bin: No file specified\n");
	exit(1);
    }

    if (strlen(argv[1]) >= FILNAMLEN) {
	printf("hex2bin: filename %s longer than %i chars\n",argv[1],sizeof(to));
	exit(1);
    }

    strcpy(from,argv[1]);
    strcat(from,".hex");
    if ((fp1 = fopen(from,"r")) == NULL) {
        printf("hex2bin: Source file %s not found\n", from);
  	exit(1);
    }
    strcpy(to,argv[1]);
    strcat(to,".bin");
    if ((fp2 = fopen(to,"wb")) == NULL) {
        printf("hex2bin: Could not create destination file %s\n", to);
  	exit(1);
    }

	printf("hex2bin: in and out files:%s %s\n",from, to);

    while ((len = next(fp1,buf)) != EOF) //HRJ read line of hex
	{
		if (len)
	    {
		  ++recnum;
          if (buf[0] != ':')  /* bad hex record, ignore */
             {
             printf("hex2bin: no colon at start of hex record #%i \n", recnum);
	         return (1);
             }
         if ((buf[7] != '0') || (buf[8] != '0')) /* not data record,ignore */
             {
              printf("hex2bin: not hex data record #%i\n", recnum);
	          break; //not exactly error condition, just address record
             }

         //ignore all but data & addr bytes, if records DIScontiguous memory
		 //  read record addr, create addr value, compare
		 addr = hexhexint(buf+3);
		 addr = (addr * 256) + hexhexint(buf+5);
		 if (lastaddr < 0) {
			 lastaddr = addr - 1; // first record sets start of binary
			 printf("hex2bin: first hex record starts at %X\n", addr);
			}
		 if (addr < 0) {
			 printf("hex2bin: negative hex addr in record #%i\n", recnum);
			 return(1);
			}
		 if (addr > (lastaddr+1)) { // discontinuous hex records but increasing
			 printf("hex2bin: gap in hex data, %X to %X, fill with FFH.\n", lastaddr+1, addr);
			 for (i = 0;i < (addr-lastaddr-1); i++) {
				 fputc(0xFF,fp2); //add FFH to binary
				}
			 lastaddr = lastaddr + i;
			}
		 else if (addr < (lastaddr+1)) { // next hex record at lower address than last
			 printf("hex2bin: hex data in record #%i at lower address %X, not at %X\n",
			    recnum, addr, lastaddr+1);
			 return (1);
			}
		 // otherwise addr is equal to oldaddr+1 as you'd expect in continuous data

		 // check Intel record length versus record length, in case line is padded w/text
		 // line length is (2*hexlen)+1 colon+2 length+4 addr+2 type+2 checksum
		 hexlen = 2 * hexhexint(buf+1);
		 if (len > hexlen+11) {
			 len = hexlen+11;
			 printf("hex2bin: chars after checksum in hex record #%i ignored\n", recnum);
			}


         //fprintf(fp2,"%s\n",buf+9); //print data record to file
	     for (i=9, j=0; i<(len-2); i+=2) //walk through hex bytes, put into file
			 {
			  n = hexhexint(buf+i);
			  if (n>=0) { fputc(n,fp2); j++; }
			  else printf("hex2bin: bad hex data in record #%i\n", recnum);
		     }
		 lastaddr = addr + j -1;
         }
         else {
			//printf("line length is 0\n");
			break;
		 }
    }
    printf("done, read %i hex records\n",recnum);
    fclose(fp1);
    fclose(fp2);
    return 0;
    }


int hexhexint(char ch[]) // two hex chars in array to int
	{
		int i,j,h; char c;

		i=0; h=0;

		for (j=0;j<2;j++)
			{
			c = ch[j];
			//printf("char >%c",c);
			if (c >= '0' && c <= '9') i = c - '0';
			else if (c >= 'A' && c <= 'F') i = c - 'A' + 10;
			else if (c >= 'a' && c <= 'f') i = c - 'a' + 10;
			else h = i = -1; // bad hex char, negative value
			if (h >= 0) h = (h * 16) + i;
		    //printf ("int %i\n",h);
		}
		return h; // will be negative if either hex char bad
	}


//Intel hex record is :llaaaa00ddddddd..cc<CR?><LF?>
//ll length, aaaa address, dddd hex data, cc checksum


int next(FILE *fp1,char *buf) //HRJ read line of hex
    {
    int i, hexlen;
	char c;

    hexlen = 0;
    if (!(fgets(buf, LEN, fp1))) return 0; //end of file
    /* get the hex record */
	//printf("%s\n",buf);
    for (i=0; i<LEN; i++) {
		c = buf[i];
    	if ((c == CR) || (c == LF) || (c == SP) || (c == EOF))  {
	      buf[i] = '\0'; /* eat the end of line */
	      hexlen = i;
	      break;
          }
		}
    return hexlen;
    }
