//**************************************************************************************************
//
//     OSSIM Open Source Geospatial Data Processing Library
//     See top level LICENSE.txt file for license information
//
//**************************************************************************************************



// TODO: Consider consolidating MSP and CSM plugins into one, and putting all
// MSP interface functionality there. Because I need CK's code to create CSM model from image
// filename.


#include <isa/services/VersionService.h>
#include <isa/Config.h>
#include <ossim/ossimVersion.h>

using namespace std;

namespace ISA
{
void VersionService::saveJSON(Json::Value& responseJson) const
{
   // Represent results in response JSON:
   responseJson["ossimVersion"] = OSSIM_VERSION;
   responseJson["ossimGitRevision"] = OSSIM_REVISION;
   responseJson["ossimBuildDate"] = OSSIM_BUILD_DATE;
   responseJson["ossimIsaGitRevision"] = OSSIM_ISA_GIT_REVISION;
   responseJson["ossimIsaBuildDate"] = OSSIM_ISA_BUILD_DATE;

}
}

