//**************************************************************************************************
//
//     OSSIM Open Source Geospatial Data Processing Library
//     See top level LICENSE.txt file for license information
//
//**************************************************************************************************
#include <iostream>
#include <SensorModel/SensorModelService.h>
#include <csm/Plugin.h>
#include <utilities/MSPTypeDefs.h>
#include <SupportData/SupportDataService.h>

using namespace std;

int main(int argc, char** argv)
{
	clog << "MSP Test" << endl;

   MSP::SMS::NameList pluginList;

   try
   {
      MSP::SMS::SensorModelService sms;
      sms.getAllRegisteredPlugins(pluginList);
      if ( pluginList.size() == 0 )
      {
         std::clog << "There are no registered plug-ins.  Goodbye." << std::endl;
         return 0;
      }

      MSP::SMS::NameList::iterator plugin = pluginList.begin();
      while (plugin != pluginList.end())
      {
         clog << "PLUGIN: "<< *plugin << endl;
         ++plugin;
      }

      if (argc > 1)
      {
         csm::Model* model = sms.createModelFromFile(argv[1]);
         if (!model)
         {
            clog<<"Null model returned."<<endl;
            return 1;
         }
         clog<<"Model name: "<<model->getModelName()<<endl;
      }
   }
   catch(exception &mspError)
   {
       clog<<"Exception: "<<mspError.what()<<endl;;
   }

	return 0;
}
