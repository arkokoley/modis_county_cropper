# Constants
product=MOD09A1.006
user=arkokoley
pass=Blogging123

install:
	@echo "install gdal and pymodis"
	@echo "<path_to_conda>/conda install gdal"
	@echo "<path_to_conda>/conda install pymodis"
	@echo "If installation gives error, run this: "
	@echo ""
	@echo "<path_to_conda>/conda create --name gdalenv python=3.5 gdal"
	@echo ""
	@echo "source <path_to_conda>/activate gdalenv"

download: tiles.txt
	mkdir $(product)
	modis_download.py -P $(pass) -U $(user) -s MOLT -p $(product) -t `cat tiles.txt` -f $(from) -e $(end) $(product)/

stitch: listfile$(product).txt mrt/*
	@echo "stitching hdf tiles"
	modis_mosaic.py -s "1 1 1 1 1 1 1" -m mrt/ -v -o mosaic listfile$(product).txt
	@echo "generating GTiff files for all layers"
	modis_convert.py -s "( 1 1 1 1 1 1 1 )" -o mosaic -e 4326 mosaic.hdf

crop: listCounty.txt shape.py
	@echo "cropping by county"
	@cat listCounty.txt | while read line; do \
		echo "$$line"; \
		python shape.py "$$line"; \
		for i in `ls *.tif | cut -d'.' -f1`; do \
			basename "$${i}" tif | awk -F"mosaic" '{print $$NF}'; \
			gdalwarp -cutline "$$line"/"$$line".shp -crop_to_cutline "$${i}".tif "$$line"/"$$line"`basename "$${i}" tif | awk -F"mosaic" '{print $$NF}'`.tif; \
		done; \
	done;
