#include <stdio.h>
#include <string.h>
#include "hdf.h"
#ifndef _MIPS_SZLONG
#define _MIPS_SZLONG 32
#endif
#include "mfhdf.h"
#include "HdfEosDef.h"

int main ( int argc, char *argv[] )
{
    int i;
    int32 sd_id, sds_id;
    intn status;
    int32 nattr, nsds, attr_index;
    int32 data_type, n_values;
    char attr_name[1024], sds_name[1024], *buffer;
    int32 rank;
    int32 dims[10];

    /* The following line quiets the SGI compiler,  Remember to remove this
     * line if and when the status variable will actaully be used (not just
     * set.
     */
    status = 0;

    /* check usage */
    if ( argc != 2 )
    {
	fprintf( stdout, "Usage: %s file.hdf\n", argv[0] );
        fflush( stdout );
	return -1;
    }

    printf("opening %s\n", argv[1]);
    sd_id = SDstart( argv[1], DFACC_READ );
    if ( sd_id < 0 )
    {
	printf("Unable to open %s\n", argv[1]);
	return -1;
    }

    SDfileinfo( sd_id, &nsds, &nattr );
    printf("SDfileinfo: sd_id = %i, nsds = %i, nattr = %i\n", (int) sd_id,
        (int) nsds, (int) nattr);
    for ( attr_index = 0; attr_index < nattr; attr_index++ )
    {
        status = SDattrinfo( sd_id, attr_index, attr_name, &data_type,
            &n_values );
        printf("SDattrinfo: attr %i, %s, data_type = %i, n_values = %i\n",
            (int) attr_index, attr_name, (int) data_type, (int) n_values);
	if (n_values < 100)
	{
	    buffer = calloc(n_values, sizeof (data_type));
            status = SDreadattr( sd_id, attr_index, buffer );
            printf("    SDreadattr: %s\n", buffer);
	    free(buffer);
	}
    }

    printf("\nprocessing SDS's:\n");
    for ( i = 0; i < nsds; i++ )
    {
        sds_id = SDselect( sd_id, i );

	SDgetinfo( sds_id, sds_name, &rank, dims, &data_type, &nattr );
        printf("\nSDgetinfo: sds %i, %s, data_type = %i, nattr = %i\n", i,
            sds_name, (int) data_type, (int) nattr);
        printf("rank = %d, dims = %d x %d\n", (int) rank, (int) dims[0],
            (int) dims[1]);
        for ( attr_index = 0; attr_index < nattr; attr_index++ )
        {
            status = SDattrinfo( sds_id, attr_index, attr_name, &data_type,
                &n_values );
            printf("    SDattrinfo: attr %i, %s, data_type = %i, "
                "n_values = %i\n", (int) attr_index, attr_name,
                (int) data_type, (int) n_values);
	    buffer = calloc(n_values, 8);
            status = SDreadattr( sds_id, attr_index, buffer );
            printf("        SDreadattr: %s\n", buffer);
	    free(buffer);
        }

        status = SDendaccess( sds_id );
    }

    return 0;
}
