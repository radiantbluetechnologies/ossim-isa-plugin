//**************************************************************************************************
//
//     OSSIM Open Source Geospatial Data Processing Library
//     See top level LICENSE.txt file for license information
//
//**************************************************************************************************



// TODO: Consider consolidating MSP and CSM plugins into one, and putting all
// MSP interface functionality there. Because I need CK's code to create CSM model from image
// filename.

#include <isa/common/Mensuration.h>

using namespace std;

namespace ISA
{

Mensuration::Mensuration()
{
}

Mensuration::Mensuration(const Json::Value& jsonNode)
{
   loadJSON(jsonNode);
}

Mensuration::Mensuration(const Mensuration& copy)
{
}

Mensuration::~Mensuration()
{
}

void Mensuration::loadJSON(const Json::Value& jsonNode)
{
}

void Mensuration::saveJSON(Json::Value& jsonNode) const
{
}



} // end O2 namespace

