#include <ctype.h>
#include <stdio.h>
#include <string.h>
#include "sf2000_diag.h"

/* v022: Multi-disk detection helper for loading numbered disk sets */
/* Finds disk files like game1.adf, game2.adf and adds them to df1, df2, df3 */
static int parse_disk_number(const char* fname, char* base_out, char* ext_out)
{
    int len = strlen(fname);
    if (len < 5) return -1;

    /* Find last dot for extension */
    int dot_pos = -1;
    for (int i = len - 1; i >= 0; i--) {
        if (fname[i] == '.') {
            dot_pos = i;
            break;
        }
        if (fname[i] == '/' || fname[i] == '\\') break;
    }
    if (dot_pos < 2) return -1;

    /* Copy extension */
    strcpy(ext_out, fname + dot_pos);

    /* Find the number before the dot */
    int num_end = dot_pos;
    int num_start = num_end;
    while (num_start > 0 && fname[num_start - 1] >= '0' && fname[num_start - 1] <= '9') {
        num_start--;
    }
    if (num_start == num_end) return -1;

    /* Copy base name (everything before the number) */
    strncpy(base_out, fname, num_start);
    base_out[num_start] = '\0';

    /* Parse the number */
    char numstr[8];
    int numlen = num_end - num_start;
    if (numlen > 7) numlen = 7;
    strncpy(numstr, fname + num_start, numlen);
    numstr[numlen] = '\0';

    return atoi(numstr);
}

static int file_exists_check(const char* path)
{
    FILE* f = fopen(path, "rb");
    if (f) {
        fclose(f);
        return 1;
    }
    return 0;
}

//Args for experimental_cmdline
static char ARGUV[64][1024];
static unsigned char ARGUC=0;

// Args for Core
static char XARGV[64][1024];
static const char* xargv_cmd[64];
int PARAMCOUNT=0;

extern void real_main(int argc, char *argv[]);
void parse_cmdline( const char *argv );

void Add_Option(const char* option)
{
   static int first=0;

   if(first==0)
   {
      PARAMCOUNT=0;	
      first++;
   }

   sprintf(XARGV[PARAMCOUNT++],"%s\0",option);
}

int pre_main(const char *argv)
{
   DIAG("pre_main() start");
   int i;
   bool Only1Arg;

   DIAG("parse_cmdline()");
   parse_cmdline(argv); 

   Only1Arg = (strcmp(ARGUV[0],"uaearm") == 0) ? 0 : 1;

   for (i = 0; i<64; i++)
      xargv_cmd[i] = NULL;


   if(Only1Arg)

   {  
	char tmpstr[512];

	Add_Option("uae4all");

	if (strlen(RPATH) >= strlen("uae")){
		if(!strcasecmp(&RPATH[strlen(RPATH)-strlen("uae")], "uae"))
		{
			Add_Option("-f"); 
			Add_Option(RPATH);
		}
		else if(!strcasecmp(&RPATH[strlen(RPATH)-strlen("adf")], "adf"))
		{
			/* v022: Multi-disk auto-detection */
			char base[512], ext[32];
			int disk_num = parse_disk_number(RPATH, base, ext);

			if (disk_num >= 0) {
				/* Numbered disk detected - find all disks */
				char found_disks[10][512];
				int disk_exists[10] = {0};
				int num_found = 0;

				/* Search for disks 0-9 */
				for (int d = 0; d <= 9; d++) {
					sprintf(found_disks[d], "%s%d%s", base, d, ext);
					if (file_exists_check(found_disks[d])) {
						disk_exists[d] = 1;
						num_found++;
						LOGI("Multidisk: found %s\n", found_disks[d]);
					}
				}

				/* Build sorted list: 1,2,3,4... then 0 at the end (autosave) */
				char* disks_to_load[4];
				int num_disks = 0;

				/* First add disks 1-9 in order */
				for (int d = 1; d <= 9 && num_disks < 4; d++) {
					if (disk_exists[d]) {
						disks_to_load[num_disks++] = found_disks[d];
					}
				}

				/* Then add disk 0 (autosave) at the end if exists and we have room */
				if (disk_exists[0] && num_disks < 4) {
					disks_to_load[num_disks++] = found_disks[0];
					LOGI("Multidisk: disk 0 placed in DF%d (autosave)\n", num_disks - 1);
				}

				/* Load disks into drives */
				const char* df_opts[4] = {"-df0", "-df1", "-df2", "-df3"};
				for (int d = 0; d < num_disks; d++) {
					Add_Option(df_opts[d]);
					sprintf(tmpstr, "%s", disks_to_load[d]);
					Add_Option(tmpstr);
					LOGI("Multidisk: DF%d = %s\n", d, tmpstr);
				}
			} else {
				/* Not a numbered disk - load normally */
				Add_Option("-df0");
				sprintf(tmpstr,"%s\0",RPATH);
				Add_Option(tmpstr);
			}
		}
		else if(!strcasecmp(&RPATH[strlen(RPATH)-strlen("hdf")], "hdf"))
		{
			Add_Option("-s");
			sprintf(tmpstr,"hardfile=rw,32,1,2,512,%s\0",RPATH);
			Add_Option(tmpstr);
		}
		else if(!strcasecmp(&RPATH[strlen(RPATH)-strlen("lha")], "lha"))
		{
			// Will be handled later not through cmdline option...
		}
                else if( (!strcasecmp(&RPATH[strlen(RPATH)-strlen("iso")], "iso"))
                        || (!strcasecmp(&RPATH[strlen(RPATH)-strlen("cue")], "cue"))
                        || (!strcasecmp(&RPATH[strlen(RPATH)-strlen("ccd")], "ccd")) )
                {
                        Add_Option("-s");
                        sprintf(tmpstr,"cdimage0=%s\0",RPATH);
                        Add_Option(tmpstr);
                }
		else
		{
			Add_Option("-s");
			sprintf(tmpstr,"floppy0=%s\0",RPATH);
			Add_Option(tmpstr);
			//Add_Option(RPATH/*ARGUV[0]*/)

		}

	}
   }
   else
   { // Pass all cmdline args
      for(i = 0; i < ARGUC; i++)
         Add_Option(ARGUV[i]);
   }

   for (i = 0; i < PARAMCOUNT; i++)
   {
      xargv_cmd[i] = (char*)(XARGV[i]);
      LOGI("%2d  %s\n",i,XARGV[i]);
   }

   DIAG("calling real_main()");
   real_main(PARAMCOUNT,( char **)xargv_cmd);

   DIAG("pre_main() done");
   xargv_cmd[PARAMCOUNT - 2] = NULL;
   return 0;
}

void parse_cmdline(const char *argv)
{
	char *p,*p2,*start_of_word;
	int c,c2;
	static char buffer[512*4];
	enum states { DULL, IN_WORD, IN_STRING } state = DULL;
	
	strcpy(buffer,argv);
	strcat(buffer," \0");

	for (p = buffer; *p != '\0'; p++)
   {
      c = (unsigned char) *p; /* convert to unsigned char for is* functions */
      switch (state)
      {
         case DULL: /* not in a word, not in a double quoted string */
            if (isspace(c)) /* still not in a word, so ignore this char */
               continue;
            /* not a space -- if it's a double quote we go to IN_STRING, else to IN_WORD */
            if (c == '"')
            {
               state = IN_STRING;
               start_of_word = p + 1; /* word starts at *next* char, not this one */
               continue;
            }
            state = IN_WORD;
            start_of_word = p; /* word starts here */
            continue;
         case IN_STRING:
            /* we're in a double quoted string, so keep going until we hit a close " */
            if (c == '"')
            {
               /* word goes from start_of_word to p-1 */
               //... do something with the word ...
               for (c2 = 0,p2 = start_of_word; p2 < p; p2++, c2++)
                  ARGUV[ARGUC][c2] = (unsigned char) *p2;
               ARGUC++; 

               state = DULL; /* back to "not in word, not in string" state */
            }
            continue; /* either still IN_STRING or we handled the end above */
         case IN_WORD:
            /* we're in a word, so keep going until we get to a space */
            if (isspace(c))
            {
               /* word goes from start_of_word to p-1 */
               //... do something with the word ...
               for (c2 = 0,p2 = start_of_word; p2 <p; p2++,c2++)
                  ARGUV[ARGUC][c2] = (unsigned char) *p2;
               ARGUC++; 

               state = DULL; /* back to "not in word, not in string" state */
            }
            continue; /* either still IN_WORD or we handled the end above */
      }	
   }
}

