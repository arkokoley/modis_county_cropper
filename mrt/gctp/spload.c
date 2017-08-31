/***************************************************************************
NAME:  spload

PURPOSE:
This program creates the binary lookup tables for the state plane
projection by reading the ascii nad1927.dat and nad1983.dat files 
and writing the binary nad27sp and nad83sp files.

DEVELOPMENT HISTORY:
  Adapted from the spload.f file for platforms that do not have a 
  FORTRAN compiler.


***************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define SP_ZONE_NAME_SIZE  32
#define SP_RECORD_SIZE     (SP_ZONE_NAME_SIZE * sizeof(char) + 1 * sizeof(long) + 9 * sizeof(double))
#define BUFSIZE 80
/* size of buffer for doing I/O */

#ifdef WIN32
/* For goodness sakes.... force windows to load up floating-point support.
 * Can't do it on the command line?
 */
double dummy;
#endif

int process_file
(
    const char *input_filename,
    const char *output_filename
)
{
    FILE *infile;   /* file pointer for the input file */
    FILE *outfile;  /* file pointer for the output file */
    char buf[BUFSIZE];
    char zone_name[SP_ZONE_NAME_SIZE+1];
    int  zone_id;
    long  zone_number;
    /* int  record_length = 108; */
    int  record_num;
    int  line;
    double parms[9];

    /* open the input file for reading */
    infile = fopen(input_filename,"rt");
    if (!infile)
    {
        printf("Error opening %s\n",input_filename);
        return 1;
    }

    /* open the output file for writing */
    outfile = fopen(output_filename,"wt");
    if (!outfile)
    {
        printf("Error opening %s\n",output_filename);
        fclose(infile);
        return 1;
    }

    /* process the lines from the input file */
    record_num = 0;
    line = 0;
    while (fgets(buf, BUFSIZE, infile))
    {
        char *ptr = buf;
        int index = (line - 1) * 3;
        switch (line)
        {
            case 0:
                /* first line of record, so get the zone name, id, and number */
                strncpy(zone_name,buf,SP_ZONE_NAME_SIZE);
                zone_name[SP_ZONE_NAME_SIZE] = '\0';
                if ((sscanf(&buf[39],"%d",&zone_id) != 1) ||
                    (sscanf(&buf[71],"%ld",&zone_number) != 1))
                {
                    printf("Error reading record %d from %s\n",record_num,
                           input_filename);
                    fclose(infile);
                    fclose(outfile);
                    return 1;
                }
                break;
            case 1: case 2: case 3:
                /* process the parameter lines of the record */
                
                /* convert 'D' characters to 'E' characters since the 
                   ascii file was created in FORTRAN floating point format */
                while (*ptr)
                {
                    if (*ptr=='D') 
                       *ptr = 'E';
                    ptr++;
                }

                /* read the three parameters from this line */
                if (sscanf(buf,"%lf%lf%lf",&parms[index],
                           &parms[index+1], &parms[index+2])!= 3)
                {
                    printf("Error reading record %d from %s\n",record_num,
                           input_filename);
                    fclose(infile);
                    fclose(outfile);
                    return 1;
                }
                break;
              
        }

        /* advance to the next line */
        line++;
        if (line == 4)
        {
            /* fourth line of record so write the completed record to the 
               output file */
            line = 0;
            
            /* seek the current record */
            if (fseek(outfile, SP_RECORD_SIZE * record_num, SEEK_SET) != 0)
            {
                printf("Error seeking record %d to %s\n",record_num,
                       output_filename);
                fclose(infile);
                fclose(outfile);
                return 1;
            }
            
            /* write the record */
            if ((fwrite(zone_name,sizeof(char),SP_ZONE_NAME_SIZE,outfile) !=
                      SP_ZONE_NAME_SIZE) ||
                (fwrite(&zone_id,sizeof(long),1,outfile) != 1) ||
                (fwrite(parms,sizeof(double),9,outfile) != 9)) 
            {
                printf("Error writing record %d to %s\n",record_num,
                       output_filename);
                fclose(infile);
                fclose(outfile);
                return 1;
            }

            /* advance to the next record */
            record_num++;

            /* output record information */
            printf (" %3d     %32s     %4ld     %1d\n",
                    record_num, zone_name, zone_number, zone_id);
        }
    }

    fclose(infile);
    if (fclose(outfile) != 0)
    {
        printf("Error closing output file %s\n",output_filename);
        return 1;
    }
    return 0;
}


int main(void)
{
    char nad27_source_file[] = "nad1927.dat";
    char nad27_dest_file[] = "nad27sp.lut";
    char nad83_source_file[] = "nad1983.dat";
    char nad83_dest_file[] = "nad83sp.lut";

#ifdef WIN32
    dummy = 1.0;
#endif

    /* process the nad 27 state plane file */
    if (process_file(nad27_source_file, nad27_dest_file))
    {
        printf("Error creating %s\n",nad27_dest_file);
        exit (1);
    }

    /* process the nad 27 state plane file */
    if (process_file(nad83_source_file, nad83_dest_file))
    {
        printf("Error creating %s\n",nad83_dest_file);
        exit (1);
    }

    return 0;
}
