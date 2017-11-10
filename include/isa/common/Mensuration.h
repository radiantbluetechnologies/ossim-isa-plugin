//**************************************************************************************************
//
//     OSSIM Open Source Geospatial Data Processing Library
//     See top level LICENSE.txt file for license information
//
//**************************************************************************************************
#ifndef Mensuration_HEADER
#define Mensuration_HEADER 1

#include <isa/common/JsonInterface.h>
#include <vector>
#include <string>
#include <memory>
#include <ossim/base/ossimGpt.h>
#include <ossim/base/ossimConstants.h>
#include <ossim/base/ossimReferenced.h>

namespace ISA
{

/**
 * Class representing set of measurements in a 3DISA session.
 */
class Mensuration : public JsonInterface,
                    public std::enable_shared_from_this<Mensuration>
{
public:
   Mensuration();
   Mensuration(const Json::Value& jsonNode);
   Mensuration(const Mensuration& copy);
   ~Mensuration();

   virtual void loadJSON(const Json::Value& jsonNode);
   virtual void saveJSON(Json::Value& jsonNode) const;
};

} // End namespace ISA

#endif
