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
#include <isa/services/msp/SourceSelectionService.h>

using namespace std;
#ifndef JSONCPP_STRING
#define JSONCPP_STRING std::string
#endif

#define USE_LINKED_LIB true

int main(int argc, char** argv)
{

   clog << "ISA Source-select Test" << endl;

   ossimArgumentParser ap1 ("isa-plugin-test -i isa-plugin-test1.json");
   ap1.getApplicationUsage()->setApplicationName(argv[0]);
   ossimInit::instance()->initialize(ap1);

   if (argc < 2)
   {
      clog<<"\nUsage: "<<argv[0]<<" <input.json>\n"<<endl;
      return 0;
   }

   stringstream json_query;
   streambuf *rqBuf = json_query.rdbuf();
   ifstream s (argv[1]);
   if (s.fail())
   {
      ossimNotify(ossimNotifyLevel_FATAL)<<__FILE__<<" Could not open input file <"<<argv[1]<<">";
      return 1;
   }

   // Read/parse the input JSON:
   Json::Value queryRoot;
   Json::CharReaderBuilder rbuilder;
   rbuilder["collectComments"] = true;
   JSONCPP_STRING errs;
   bool ok = Json::parseFromStream(rbuilder, json_query, &queryRoot, &errs);
   if ( !errs.empty() )
   {
      ossimNotify(ossimNotifyLevel_FATAL)<<__FILE__<<" Failed to parse JSON query string. \n"
            << errs <<endl;
   }
   ISA::SourceSelectionService service;

   // Call actual service and get response:
   service.loadJSON(queryRoot);
   service.execute();
   Json::Value response;
   service.saveJSON(response);

   // Serialize JSON object for return:
   Json::StreamWriterBuilder wbuilder;
   wbuilder["commentStyle"] = "None";
   wbuilder["indentation"] = "  ";  // or whatever you like
   std::unique_ptr<Json::StreamWriter> writer(wbuilder.newStreamWriter());
   ostringstream json_response;
   writer->write(response, &json_response);
   clog<<"\nResponse:\n" << json_response;

	return 0;
}
