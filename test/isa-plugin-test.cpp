//**************************************************************************************************
//
//     OSSIM Open Source Geospatial Data Processing Library
//     See top level LICENSE.txt file for license information
//
//**************************************************************************************************
#include <iostream>
#include <memory>
#include <ossim/base/ossimApplicationUsage.h>
#include <ossim/init/ossimInit.h>
#include <ossim/base/ossimArgumentParser.h>
#include <ossim/util/ossimToolRegistry.h>
#include <ossim/base/ossimException.h>
#include <isa/plugin/ossimIsaTool.h>

using namespace std;

#define USE_LINKED_LIB true

int runTest(ossimRefPtr<ossimTool> isaTool,  ossimArgumentParser& ap)
{
   if (!isaTool)
   {
      clog<<"ossim-isa-tool -- Null ptr returned from tool factory."<<endl;
      return 1;
   }
   if (!isaTool->initialize(ap))
   {
      clog<<"ossim-isa-tool -- Error returned from ISA tool initialization."<<endl;
      return 1;
   }
   if (!isaTool->execute())
   {
      clog<<"ossim-isa-tool -- Error returned from ISA tool execute()."<<endl;
      return 1;
   }
   return 0;
}

int main(int argc, char** argv)
{
   clog << "ISA Plugin Test" << endl;

   ossimArgumentParser ap1 ("isa-plugin-test -i isa-plugin-test1.json");
   ap1.getApplicationUsage()->setApplicationName(argv[0]);
   ossimInit::instance()->initialize(ap1);
   ossimRefPtr<ossimTool> isaTool (ossimToolRegistry::instance()->createTool("ossimIsaTool"));
   runTest(isaTool, ap1);

#if USE_LINKED_LIB
   ossimArgumentParser ap2 ("isa-plugin-test -i isa-plugin-test1.json");
   ap2.getApplicationUsage()->setApplicationName(argv[0]);
   isaTool = new ISA::ossimIsaTool;
   runTest(isaTool, ap2);
#endif

	return 0;
}
