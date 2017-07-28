from osgeo import ogr
import os
import sys

def save(shapePath, feature):
    'Save points in the given shapePath'
    # Get driver
    driver = ogr.GetDriverByName('ESRI Shapefile')
    # Create shapeData
    if os.path.exists(shapePath):
        os.remove(shapePath)
    shapeData = driver.CreateDataSource(shapePath)
    # Create spatialReference
    spatialReference = layer.GetSpatialRef()
    # Create layer
    layerName = shapePath
    newlayer = shapeData.CreateLayer(layerName, spatialReference, ogr.wkbPolygon)
    newlayer.CreateFeature(feature)
    shapeData.Destroy()

shapefile = ogr.Open("counties/counties.shp")
layer = shapefile.GetLayer(0)

for i in range(layer.GetFeatureCount()):
    feature = layer.GetFeature(i)
    name = feature.GetField("GEOID")
    if name == sys.argv[1]:
        save(name, feature)
