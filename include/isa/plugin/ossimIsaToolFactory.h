//**************************************************************************************************
//
//     OSSIM Open Source Geospatial Data Processing Library
//     See top level LICENSE.txt file for license information
//
//**************************************************************************************************

#ifndef ossimIsaToolFactory_HEADER
#define ossimIsaToolFactory_HEADER 1

#include <ossim/plugin/ossimPluginConstants.h>
#include <ossim/util/ossimToolFactoryBase.h>

class ossimString;
class ossimFilename;
class ossimKeywordlist;

namespace ISA
{
class OSSIM_PLUGINS_DLL ossimIsaToolFactory: public ossimToolFactoryBase
{
public:
   static ossimIsaToolFactory* instance();

   virtual ~ossimIsaToolFactory();
   virtual ossimTool* createTool(const std::string& typeName) const;
   virtual void getTypeNameList(std::vector<ossimString>& typeList) const;
   virtual void getCapabilities(std::map<std::string, std::string>& capabilities) const;
   virtual std::map<std::string, std::string> getCapabilities() const;

protected:
   ossimIsaToolFactory();
   ossimIsaToolFactory(const ossimIsaToolFactory&);
   void operator=(const ossimIsaToolFactory&);

   /** static instance of this class */
   static ossimIsaToolFactory* s_instance;

};
}
#endif /* end of #ifndef ossimIsaToolFactory_HEADER */
