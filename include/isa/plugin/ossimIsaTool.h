//**************************************************************************************************
//
//     OSSIM Open Source Geospatial Data Processing Library
//     See top level LICENSE.txt file for license information
//
//**************************************************************************************************

#ifndef ossimIsaTool_HEADER
#define ossimIsaTool_HEADER 1

#include <ossim/plugin/ossimPluginConstants.h>
#include <ossim/imaging/ossimImageHandler.h>
#include <ossim/util/ossimTool.h>
#include <iostream>

namespace ISA
{
class OSSIM_DLL ossimIsaTool : public ossimTool
{
public:
   static const char* DESCRIPTION;

   ossimIsaTool();

   virtual ~ossimIsaTool();

   virtual void setUsage(ossimArgumentParser& ap);

   virtual bool initialize(ossimArgumentParser& ap);

   virtual void initialize(const ossimKeywordlist& ap);

   virtual bool execute();

   virtual ossimString getClassName() const { return "ossimMspTool"; }

   virtual void getKwlTemplate(ossimKeywordlist& kwl);

private:
   std::istream* m_inputStream;
   std::ostream* m_outputStream;
   bool m_verbose;
};
}
#endif /* #ifndef ossimIsaTool_HEADER */
