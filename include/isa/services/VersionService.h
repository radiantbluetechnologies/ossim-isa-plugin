//**************************************************************************************************
//
//     OSSIM Open Source Geospatial Data Processing Library
//     See top level LICENSE.txt file for license information
//
//**************************************************************************************************
#ifndef VersionService_HEADER
#define VersionService_HEADER 1

#include <isa/services/ServiceBase.h>
#include <vector>
#include <string>
#include <memory>


namespace ISA
{

/**
 * Fetches the current version/build for MSP services
 */
class VersionService : public ServiceBase
{
public:
   VersionService() {};
   virtual ~VersionService() {}

   virtual void execute() {};

   /*
   * Refer to <a href="https://docs.google.com/document/d/1DXekmYm7wyo-uveM7mEu80Q7hQv40fYbtwZq-g0uKBs/edit?usp=sharing">3DISA API document</a>
   * for JSON format used.
   */
   virtual void loadJSON(const Json::Value& dummy) {}

   /*
   * Refer to <a href="https://docs.google.com/document/d/1DXekmYm7wyo-uveM7mEu80Q7hQv40fYbtwZq-g0uKBs/edit?usp=sharing">3DISA API document</a>
   * for JSON format used.
   */
   virtual void saveJSON(Json::Value& json) const;

private:
};

} // End namespace ISA

#endif
