//**************************************************************************************************
//
//     OSSIM Open Source Geospatial Data Processing Library
//     See top level LICENSE.txt file for license information
//
//**************************************************************************************************



// TODO: Consider consolidating MSP and CSM plugins into one, and putting all
// MSP interface functionality there. Because I need CK's code to create CSM model from image
// filename.


#include <isa/Config.h>
#include <isa/services/SensorModelService.h>
#include <cstdlib>
#if OSSIM_HAS_MSP
#include <SensorModel/SensorModelService.h>
#endif

using namespace std;

namespace ISA
{

SensorModelService::SensorModelService()
{
}

SensorModelService::~SensorModelService()
{
}

void SensorModelService::loadJSON(const Json::Value& json)
{
   m_method = json["method"].asString();
   m_application = json["application"].asString();

   try
   {
      m_image.reset(new Image(json));
   }
   catch (exception& e)
   {
      m_image.reset();
   }

}

void SensorModelService::saveJSON(Json::Value& json) const
{
   if (!m_image)
   {
#if OSSIM_HAS_MSP
      try
      {
         // Just return list of available plugins:
         shared_ptr<MSP::SMS::SensorModelService> sms (new MSP::SMS::SensorModelService());
         MSP::SMS::NameList pluginList;
         sms->getAllRegisteredPlugins(pluginList);
         if ( pluginList.size() == 0 )
         {
            std::clog << "CONSOLE -- There are no registered plug-ins." << std::endl;
            json["sensorModelPlugins"] = "none";
         }
         else
         {
            Json::Value pluginsArray;
            MSP::SMS::NameList::iterator plugin = pluginList.begin();
            while (plugin != pluginList.end())
            {
               pluginsArray.append(*plugin);
               clog << "CONSOLE -- PLUGIN: "<< *plugin << endl;
               ++plugin;
            }
         }
      }
      catch(exception &mspError)
      {
         clog<<"Exception: "<<mspError.what()<<endl;;
         json["sensorModelPlugins"] = "none";
      }
#endif
   }
   else
   {
      // Assemble response JSON header

      if (m_method.empty() || (m_method == "getAvailableModels"))
      {
         // Fetch the models:
         std::vector< pair<std::string, std::string> > models;
         m_image->getAvailableModels(models);

         // Write the image information:
         Json::Value modelsArray(Json::arrayValue);
         for (size_t i=0; i<models.size(); ++i)
         {
            Json::Value arrayEntry;
            arrayEntry["plugin"] = models[i].first;
            arrayEntry["model"] = models[i].second;
            modelsArray.append(arrayEntry);
         }
         json["sensorModels"] = modelsArray;
      }
      else if (m_method == "getModelState")
      {
         if (m_image->getCsmSensorModel())
         {
            Json::Value imageJson;
            m_image->saveJSON(imageJson);
            json["image"] =   imageJson;
         }
         else
         {
            ostringstream msg;
            msg<<"Model <"<<m_image->getModelName()<<"> not found or not supported.";
            json["error"] = msg.str();
         }
      }
   }

}

void SensorModelService::execute(void)
{
}

void SensorModelService::execute(const string& imageFile)
{
   m_image.reset(new Image("", imageFile, ""));
   Json::Value json;
   saveJSON(json);
   cout << json << endl;
}

} // end O2REG namespace

