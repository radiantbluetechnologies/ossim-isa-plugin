//**************************************************************************************************
//
//     OSSIM Open Source Geospatial Data Processing Library
//     See top level LICENSE.txt file for license information
//
//**************************************************************************************************

#include <isa/plugin/ossimIsaTool.h>
#include <ossim/base/ossimArgumentParser.h>
#include <ossim/base/ossimApplicationUsage.h>
#include <ossim/base/ossimKeywordNames.h>
#include <ossim/base/ossimException.h>
#include <ossim/base/ossimNotify.h>
#include <ossim/base/ossimException.h>
#include <isa/services/SensorModelService.h>
#include <isa/services/AutoTiePointService.h>
#include <isa/services/VersionService.h>
#include <json/json.h>

#if OSSIM_HAS_MSP
#include <isa/services/msp/SourceSelectionService.h>
#include <isa/services/msp/TriangulationService.h>
#endif

#include <iostream>
#include <memory>
#include <sstream>

using namespace std;

namespace ISA
{
const char* ossimIsaTool::DESCRIPTION =
      "Provides access to ISA functionality (source selection, auto-tiepoints, triangulation, mensuration).";

ossimIsaTool::ossimIsaTool()
: m_inputStream (0),
  m_outputStream (0),
  m_verbose (false)
{
}

ossimIsaTool::~ossimIsaTool()
{
}

void ossimIsaTool::setUsage(ossimArgumentParser& ap)
{
   // Add global usage options. Don't include ossimChipProcUtil options as not appropriate.
   ossimTool::setUsage(ap);

   // Set the general usage:
   ossimApplicationUsage* au = ap.getApplicationUsage();
   ossimString usageString = ap.getApplicationName();
   usageString += " isa [options] \n\n";
   usageString +=
         "Accesses all ossim-isa functionality given JSON request on stdin (or input file if\n"
         "-i specified). The response JSON is output to stdout unless -o option is specified.\n";
   au->setCommandLineUsage(usageString);

   // Set the command line options:
   au->addCommandLineOption("-i <filename>",
         "Reads request JSON from the input file specified instead of stdin.");
   au->addCommandLineOption("-o <filename>",
         "Outputs response JSON to the output file instead of stdout.");
   au->addCommandLineOption("-v",
         "Verbose. All non-response (debug) output to stdout is enabled.");
}

bool ossimIsaTool::initialize(ossimArgumentParser& ap)
{
   string ts1;
   ossimArgumentParser::ossimParameter sp1(ts1);

   if (!ossimTool::initialize(ap))
      return false;
   if (m_helpRequested)
      return true;

   if ( ap.read("-v"))
      m_verbose = true;

   if ( ap.read("-i", sp1))
   {
      ifstream* s = new ifstream (ts1);
      if (s->fail())
      {
         ossimNotify(ossimNotifyLevel_FATAL)<<__FILE__<<" Could not open input file <"<<ts1<<">";
         return false;
      }
      m_inputStream = s;
   }
   else
      m_inputStream = &cin;

   if ( ap.read("-o", sp1))
   {
      ofstream* s = new ofstream (ts1);
      if (s->fail())
      {
         ossimNotify(ossimNotifyLevel_FATAL)<<__FILE__<<" Could not open output file <"<<ts1<<">";
         return false;
      }
      m_outputStream = s;
   }
   else
      m_outputStream = &cout;

   ap.reportRemainingOptionsAsUnrecognized();
   if (ap.errors())
   {
      ap.writeErrorMessages(clog);
      setUsage(ap);
      ap.getApplicationUsage()->write(ossimNotify(ossimNotifyLevel_INFO));
      return false;
   }

   return true;
}

void ossimIsaTool::initialize(const ossimKeywordlist& kwl)
{
   ossimString value;
   ostringstream xmsg;

   // Don't copy KWL if member KWL passed in:
   if (&kwl != &m_kwl)
   {
      // Start with clean options keyword list.
      m_kwl.clear();
      m_kwl.addList( kwl, true );
   }

   ossimTool::initialize(kwl);
}

bool ossimIsaTool::execute()
{
   ostringstream xmsg;
   xmsg<<"ossimIsaTool::execute()  ";

   if (helpRequested())
      return true;
   if (!m_inputStream)
   {
      xmsg<<"No input stream assigned!"<<endl;
      throw ossimException(xmsg.str());
   }

   // Read input:
   stringstream json_query;
   streambuf *rqBuf = json_query.rdbuf();
   (*m_inputStream) >> rqBuf;
   try
   {
      Json::Value queryRoot;
      Json::Reader reader;
      bool parsingSuccessful = reader.parse( json_query.str(), queryRoot );
      if ( !parsingSuccessful )
      {
         xmsg<<"Failed to parse JSON query string. \n"
               << reader.getFormattedErrorMessages()<<endl;
         throw ossimException(xmsg.str());
      }

      // Fetch service name and route accordingly:
      string serviceName = queryRoot["service"].asString();
      shared_ptr<ServiceBase> service;
      if (serviceName      == "getVersion")
         service.reset(new VersionService);
      else if (serviceName == "sensorModels")
         service.reset(new SensorModelService);
      else if (serviceName == "autoTiePoint")
         service.reset(new AutoTiePointService);
#if OSSIM_HAS_MSP
      else if (serviceName == "sourceSelection")
         service.reset(new SourceSelectionService);
      else if (serviceName == "triangulation")
         service.reset(new TriangulationService);
      //   else if (serviceName == "mensuration")
      //      service.reset(new MensurationService);
#endif
      else
      {
         xmsg<<"Wrong service <"<<serviceName<<"> requested."<<endl;
         throw ossimException(xmsg.str());
      }

      // Call actual service and get response:
      service->loadJSON(queryRoot);
      service->execute();
      Json::Value response;
      service->saveJSON(response);

      // Serialize JSON object for return:
      Json::StyledWriter writer;
      string json_response = writer.write(response);
      (*m_outputStream) << json_response;
   }
   catch(ossimException &e)
   {
      cerr<<"Exception: "<<e.what()<<endl;
      *m_outputStream<<"{ \"ERROR\": \"" << e.what() << "\" }\n"<<endl;
   }

   // close any open file streams:
   ifstream* si = dynamic_cast<ifstream*>(m_inputStream);
   if (si)
   {
      si->close();
      delete si;
   }
   ofstream* so = dynamic_cast<ofstream*>(m_outputStream);
   if (so)
   {
      so->close();
      delete so;
   }

   return true;
}

void ossimIsaTool::getKwlTemplate(ossimKeywordlist& kwl)
{
}

}


