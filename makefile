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
	mkdir -p $(product)
	modis_download.py -P $(pass) -U $(user) -s MOLT -p $(product) -t `cat tiles.txt` -f $(from) -e $(end) $(product)/

stitch: $(product)/listfile$(product).txt mrt/*
	@echo "stitching hdf tiles"
	cd $(product); \
	ls *.hdf | cut -f 2 -d. | cut -d. -f1 | uniq | sort | while read line; do \
		ls *.hdf *.xml | grep "$$line" > listfile."$$line".$(product).txt; \
		modis_mosaic.py -s "1 1 1 1 1 1 1" -m ../mrt/ -v -o mosaic."$$line" listfile."$$line".$(product).txt; \
		echo "generating GTiff files for all layers"; \
		modis_convert.py -s "( 1 1 1 1 1 1 1 )" -o mosaic."$$line" -e 4326 mosaic."$$line".hdf; \
	done; \
	cd .. ;

crop: listCounty.txt shape.py
	@echo "cropping by county"
	mkdir -p data
	@cat listCounty.txt | while read line; do \
		echo "$$line"; \
		python shape.py "$$line"; \
		cd $(product); \
		for i in `ls *.tif | cut -d'.' -f2`; do \
			mkdir -p ../data/`basename "$${i}" tif | cut -d'_' -f1`; \
			gdalwarp -cutline ../"$$line"/"$$line".shp -crop_to_cutline mosaic."$${i}".tif ../data/`basename "$${i}" tif | cut -d'_' -f1`/"$$line".`basename "$${i}" tif | cut -d'_' -f2-`.tif; \
		done; \
		cd .. ; \
	done;
