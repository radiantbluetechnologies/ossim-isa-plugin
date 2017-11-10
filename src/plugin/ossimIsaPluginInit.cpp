//**************************************************************************************************
//
//     OSSIM Open Source Geospatial Data Processing Library
//     See top level LICENSE.txt file for license information
//
//**************************************************************************************************

#include <isa/plugin/ossimIsaToolFactory.h>
#include <ossim/plugin/ossimSharedObjectBridge.h>
#include <ossim/plugin/ossimPluginConstants.h>
#include <ossim/base/ossimString.h>
#include <ossim/util/ossimToolRegistry.h>

extern "C"
{
ossimSharedObjectInfo  isaInfo;

const char* getIsaDescription()
{
   return "ossim-isa tool plugin\n\n";
}

int getIsaNumberOfClassNames()
{
   return 1;
}

const char* getIsaClassName(int idx)
{
   if (idx == 0)
   {
      return "ossimIsaTool";
   }
   return (const char*)0;
}

/* Note symbols need to be exported on windoze... */
OSSIM_PLUGINS_DLL void ossimSharedLibraryInitialize(ossimSharedObjectInfo** info)
{
   isaInfo.getDescription = getIsaDescription;
   isaInfo.getNumberOfClassNames = getIsaNumberOfClassNames;
   isaInfo.getClassName = getIsaClassName;

   *info = &isaInfo;

   /* Register the utility... */
   ossimToolRegistry::instance()->
         registerFactory(ISA::ossimIsaToolFactory::instance());

}

/* Note symbols need to be exported on windoze... */
OSSIM_PLUGINS_DLL void ossimSharedLibraryFinalize()
{
   ossimToolRegistry::instance()->unregisterFactory(ISA::ossimIsaToolFactory::instance());
}
}
