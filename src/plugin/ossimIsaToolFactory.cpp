//**************************************************************************************************
//
//     OSSIM Open Source Geospatial Data Processing Library
//     See top level LICENSE.txt file for license information
//
//**************************************************************************************************

#include <isa/plugin/ossimIsaToolFactory.h>
#include <isa/plugin/ossimIsaTool.h>
#include <isa/plugin/ossimAtpTool.h>
#include <ossim/util/ossimToolRegistry.h>

using namespace std;

namespace ISA
{
ossimIsaToolFactory* ossimIsaToolFactory::s_instance = 0;

ossimIsaToolFactory* ossimIsaToolFactory::instance()
{
   if (!s_instance)
      s_instance = new ossimIsaToolFactory;
   return s_instance;
}

ossimIsaToolFactory::ossimIsaToolFactory()
{
}

ossimIsaToolFactory::~ossimIsaToolFactory()
{
   ossimToolRegistry::instance()->unregisterFactory(this);
}

ossimTool* ossimIsaToolFactory::createTool(const std::string& argName) const
{
   ossimString utilName (argName);
   utilName.downcase();

   if ((utilName == "isa") || (argName == "ossimIsaTool"))
      return new ossimIsaTool;
   if ((utilName == "atp") || (argName == "ossimAtpTool"))
      return new ossimIsaTool;

   return 0;
}

void ossimIsaToolFactory::getCapabilities(std::map<std::string, std::string>& capabilities) const
{
   capabilities.insert(pair<string, string>("isa", ossimIsaTool::DESCRIPTION));
   capabilities.insert(pair<string, string>("atp", ossimAtpTool::DESCRIPTION));
}

std::map<std::string, std::string> ossimIsaToolFactory::getCapabilities() const
{
   std::map<std::string, std::string> result;
   getCapabilities(result);
   return result;
}

void ossimIsaToolFactory::getTypeNameList(vector<ossimString>& typeList) const
{
   typeList.push_back("ossimIsaTool");
   typeList.push_back("ossimAtpTool");
}
}
