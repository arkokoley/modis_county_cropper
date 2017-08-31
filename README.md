How to run:
===========

1. Register for a NASA EARTHDATA Account at [https://urs.earthdata.nasa.gov/users/new](https://urs.earthdata.nasa.gov/users/new)
2. Select the MODIS Remote Sensing Dataset you want to download from
 [https://lpdaac.usgs.gov/dataset_discovery/modis/modis_products_table](https://lpdaac.usgs.gov/dataset_discovery/modis/modis_products_table).
 For our purposes, the right dataset was: MOD09A1.006

3. Clone this Git repo to your system.
4. Run: `make install`.  
  This will list the tools you need installed to run the subsequent commands.
5. Create a `tiles.txt` file in the same folder as the makefile containing all the MODIS tiles you are interested in. There is a `tiles.txt` file in the folder containing all the MODIS tiles required for Mainland US.
6. Run:  
```
user=<earthdata username> pass=<earthdata pass> from=YYYY-MM-DD end=YYYY-MM-DD product=MOD09A1.006 make download
```  
Here __from__ and __end__ specify the time range for which you want to download the MODIS Dataset and __product__ is the product selected in step 2.

7. Find Counties of interest from [https://www2.census.gov/geo/docs/reference/codes/files/national_county.txt](https://www2.census.gov/geo/docs/reference/codes/files/national_county.txt) and add their FIPS number to `listCount.txt`.
8. Run: `make crop`  
 This will crop the datasets by counties and arrange them in date wise folders in the `data` folder.
