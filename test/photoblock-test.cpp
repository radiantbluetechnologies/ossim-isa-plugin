//**************************************************************************************************
//
//     OSSIM Open Source Geospatial Data Processing Library
//     See top level LICENSE.txt file for license information
//
//**************************************************************************************************
#include <isa/atp/AtpConfig.h>
#include <iostream>
#include <isa/atp/correlation/CorrelationAtpGenerator.h>
#include <isa/common/TiePoint.h>
#include <ossim/init/ossimInit.h>
#include <ossim/base/ossimKeywordlist.h>

using namespace std;

/***************************************************************************************************
Sample input KWL:

cmp_image: L.tif
ref_image: R.tif
geo_aoi: 0.0011466337551191, -0.0011466337551191, -0.00114663375511909, 0.00114663375511909
output_name: output
feature_based: true
numFeaturesPerTile: 10
peakThreshold: 0.80

// Must be odd number for centering on feature pixel
corr_window_size: 9

// Must be odd number for filling destination image about the center pixel. Only used when
// feature_based = false
corr_step_size: 1

// For rendering correlations as raster images (not feature-based)
res_color_lut: RGB_hot.lut

***************************************************************************************************/

int main(int argc, char** argv)
{
   clog << "ATP Test" << endl;
   if (argc < 2)
   {
      clog<<"Usage: "<<argv[0]<<" <input.kwl>"<<endl;
      return 0;
   }

   ossimInit::instance()->initialize(argc, argv);

   ossimKeywordlist kwl (argv[1]);

   ossimFilename refname = kwl.find("ref_image");
   if (refname.empty())
   {
      clog<<argv[0]<<" -- Error: missing keyword \"ref_image\" in input KWL file"<<endl;
      return 1;
   }

   ossimFilename cmpname = kwl.find("cmp_image");
   if (cmpname.empty())
   {
      clog<<argv[0]<<" -- Error: missing keyword \"cmp_image\" in input KWL file"<<endl;
      return 1;
   }

   ossimFilename outname = kwl.find("output_name");
   if (outname.empty())
   {
      clog<<argv[0]<<" -- Error: missing keyword \"output_name\" in input KWL file"<<endl;
      return 1;
   }

   ossimFilename lutname = kwl.find("res_color_lut");
   if (lutname.empty())
   {
      clog<<argv[0]<<" -- Error: missing keyword \"res_color_lut\" in input KWL file"<<endl;
      return 1;
   }

   ossimFilename atpConfigName = kwl.find("atp_config");

   // Initialize correlation parameters:
   ISA::AtpConfig* atpConfig = ISA::AtpConfig::instance();
   if (!atpConfigName.empty())
   {
      ossimKeywordlist atpConfigKwl (atpConfigName);
      atpConfig->loadState(atpConfigKwl);
      clog << *atpConfig << endl;
   }
   // Create the workhorse object:
   ISA::CorrelationAtpGenerator corAtpGen;
   corAtpGen.setRefImage(refname);
   corAtpGen.setCmpImage(cmpname);

   // Generate tie points and output to console:
   ISA::TiePointList tpList;
   corAtpGen.generateTiePointList(tpList);
   corAtpGen.writeTiePointList(clog, tpList);

   // Generate TP correlation image:
//   corAtpGen.renderTiePointDataAsImages(outname);

   delete atpConfig;
   return 0;
}
