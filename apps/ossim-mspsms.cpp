//**************************************************************************************************
//
//  Application for running OSSIM-ISA services given JSON query file
//
//**************************************************************************************************

#include <isa/services/SensorModelService.h>
#include <ossim/base/ossimException.h>
#include <ossim/base/ossimApplicationUsage.h>
#include <ossim/base/ossimArgumentParser.h>
#include <ossim/init/ossimInit.h>
#include <iostream>

using namespace std;
using namespace ISA;

int main(int argc, char** argv)
{
   ostringstream xmsg;
   xmsg<<"Exception in ossim-mspsms. ";

   ossimArgumentParser ap (&argc, argv);
   ap.getApplicationUsage()->setApplicationName(argv[0]);

   try
   {
      // Initialize ossim stuff, factories, plugin, etc.
      ossimInit::instance()->initialize(ap);

      // Must have a filename:
      if (ap.argc() < 2)
      {
         cout<<"\nUsage: "<<argv[0]<<" <image_filename>\n"<<endl;
         return 0;
      }

      string filename = argv[1];
      SensorModelService sms;
      sms.execute(filename);
   }
   catch(ossimException &e)
   {
      cerr<<"Exception: "<<e.what()<<endl;
      return 1;
   }

   return 0;
}
