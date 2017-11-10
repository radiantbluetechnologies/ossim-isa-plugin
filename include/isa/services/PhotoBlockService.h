//**************************************************************************************************
//
//     OSSIM Open Source Geospatial Data Processing Library
//     See top level LICENSE.txt file for license information
//
//**************************************************************************************************
#ifndef PhotoBlockService_HEADER
#define PhotoBlockService_HEADER 1

#include <isa/services/ServiceBase.h>
#include <memory>
#include <iostream>

namespace ISA
{

/**
 * Top-level class for interfacing to MSP PhotoBlock from 3DISA client. The photoblock is accessed
 * when either images, GCPs or image tiepoints are added or removed from the solution.
 * TODO: This implies the photoblock's state is preserved server-side between service calls. This
 * capability is not yet supported.
 */
class PhotoBlockService : public ServiceBase
{
public:
   PhotoBlockService() {}
   ~PhotoBlockService() {}

   /*
   * Refer to <a href="https://docs.google.com/document/d/1DXekmYm7wyo-uveM7mEu80Q7hQv40fYbtwZq-g0uKBs/edit?usp=sharing">3DISA API document</a>
   * for JSON format used.
   */
   virtual void loadJSON(const Json::Value& json) {}

   /*
   * Refer to <a href="https://docs.google.com/document/d/1DXekmYm7wyo-uveM7mEu80Q7hQv40fYbtwZq-g0uKBs/edit?usp=sharing">3DISA API document</a>
   * for JSON format used.
   */
   virtual void saveJSON(Json::Value& json) const {}

   virtual void execute()
   {
      std::clog<<__FILE__<<" PhotoBlockService::execute() NOT YET IMPLEMENTED \n"<<std::endl;
   }
};

} // End namespace ISA

#endif
