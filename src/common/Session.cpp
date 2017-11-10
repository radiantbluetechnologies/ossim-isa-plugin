//**************************************************************************************************
//
//     OSSIM Open Source Geospatial Data Processing Library
//     See top level LICENSE.txt file for license information
//
//**************************************************************************************************
#include <isa/common/PhotoBlock.h>
#include <isa/common/Session.h>
#include <ossim/base/ossimCommon.h>

using namespace std;

namespace ISA
{
Session::Session()
:  m_photoBlock (new PhotoBlock),
   m_mensuration (new Mensuration)
{
   const string format = "%Y%m%d-%H%Mh%Ss";
   ossim::getFormattedTime(format, true, m_sessionId);
}

Session::Session(const Json::Value& json)
:  m_photoBlock (new PhotoBlock),
   m_mensuration (new Mensuration)
{
   loadJSON(json);
}

Session::Session(const Session& copyThis)
:  m_sessionId (copyThis.m_sessionId),
   m_description (copyThis.m_description),
   m_photoBlock (copyThis.m_photoBlock),
   m_mensuration (copyThis.m_mensuration)
{
   m_sessionId += "_COPY";
}

Session::~Session()
{
   m_photoBlock = 0;
   m_mensuration = 0;
}

shared_ptr<PhotoBlock> Session::getPhotoBlock()
{
   return m_photoBlock;
}

shared_ptr<Mensuration> Session::getMeasurements()
{
   return m_mensuration;

}
void Session::addMeasurement(Mensuration* measurement)
{
   // TODO: Implement
   clog<<__FILE__<<": addMeasurement() Method not yet implemented!"<<endl;
}

void Session::removeMeasurement(unsigned int measurementId)
{
   // TODO: Implement
   clog<<__FILE__<<": removeMeasurement() Method not yet implemented!"<<endl;
}

void Session:: saveJSON(Json::Value& jsonNode) const
{
   jsonNode["sessionId"] = m_sessionId;
   jsonNode["description"] = m_description;

   Json::Value pbNode;
   m_photoBlock->saveJSON(pbNode);
   jsonNode["photoblock"] = pbNode;

   Json::Value mensurationNode;
   m_mensuration->saveJSON(mensurationNode);
   jsonNode["mensuration"] = mensurationNode;
}

void Session::loadJSON(const Json::Value& jsonNode)
{
   m_sessionId = jsonNode["sessionId"].asString();
   m_description = jsonNode["description"].asString();

   const Json::Value& pbNode = jsonNode["photoblock"];
   m_photoBlock.reset(new PhotoBlock (pbNode));

   const Json::Value& mensurationNode = jsonNode["mensuration"];
   m_mensuration.reset(new Mensuration (mensurationNode));
}
}
