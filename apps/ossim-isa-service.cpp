//**************************************************************************************************
//
//  Application for running OSSIM-ISA services given JSON query file
//
//**************************************************************************************************

#include <isa/plugin/ossimIsaTool.h>
#include <ossim/base/ossimException.h>
#include <ossim/base/ossimApplicationUsage.h>
#include <ossim/base/ossimArgumentParser.h>
#include <ossim/init/ossimInit.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <unistd.h>
#include <iostream>

using namespace std;
using namespace ISA;

// This is a convenience entry point for the ossimMspTool that is implemented as an ossim tool
// plugin. The same functionality is available through ossim-cli assuming the ossim_msp_plugin
// library is listed in the plugin preferences.
int main(int argc, char** argv)
{
   ostringstream xmsg;
   xmsg<<"Exception in ossim-isa-service. ";

   ossimArgumentParser ap (&argc, argv);
   ap.getApplicationUsage()->setApplicationName(argv[0]);

   try
   {
      // Initialize ossim stuff, factories, plugin, etc.
      ossimInit::instance()->initialize(ap);

      ossimIsaTool isaTool;
      if (!isaTool.initialize(ap))
         throw ossimException(xmsg.str());

      if (!isaTool.helpRequested() && !isaTool.execute())
         throw ossimException(xmsg.str());
   }
   catch(ossimException &e)
   {
      cerr<<"Exception: "<<e.what()<<endl;
      return 1;
   }

   return 0;
}
