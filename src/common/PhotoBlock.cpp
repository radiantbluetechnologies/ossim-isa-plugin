//**************************************************************************************************
//
//     OSSIM Open Source Geospatial Data Processing Library
//     See top level LICENSE.txt file for license information
//
//**************************************************************************************************

#include <isa/common/PhotoBlock.h>
#include <ossim/base/ossimException.h>

namespace ISA
{
PhotoBlock::PhotoBlock()
{
}

PhotoBlock::PhotoBlock(const Json::Value& pb_json_node)
{
   loadJSON(pb_json_node);
}

PhotoBlock::PhotoBlock(const PhotoBlock& copyThis)
: m_imageList (copyThis.m_imageList),
  m_tiePointList (copyThis.m_tiePointList)
{
   *this = copyThis;
}

PhotoBlock::~PhotoBlock()
{
   m_imageList.clear();
   m_tiePointList.clear();
}

PhotoBlock& PhotoBlock::operator=(const PhotoBlock& copythis)
{
   m_imageList = copythis.m_imageList;
   m_tiePointList = copythis.m_tiePointList;
   m_gcpList = copythis.m_gcpList;
   return *this;
}

// TODO: Add of individual components not available until proper management of the JCM can be
//       provided
unsigned int  PhotoBlock::addImage(shared_ptr<Image> image)
{
   unsigned int last_idx = m_imageList.size();
   m_imageList.push_back(image);
   return last_idx;
}

unsigned int  PhotoBlock::addGroundPoint(shared_ptr<GroundControlPoint> gcp)
{
   unsigned int last_idx = m_gcpList.size();
   m_gcpList.push_back(gcp);
   return last_idx;
}

unsigned int PhotoBlock::addTiePoint(shared_ptr<TiePoint> tiepoint)
{
   unsigned int index = m_tiePointList.size();
   m_tiePointList.push_back(tiepoint);
   return index;
}

void PhotoBlock::addTiePoints(AtpList& tiepointList)
{
   for (size_t i=0; i<tiepointList.size(); ++i)
   {
      addTiePoint(dynamic_pointer_cast<TiePoint>(tiepointList[i]));
   }
}

shared_ptr<Image> PhotoBlock::getImage(const std::string& imageId)
{
   std::shared_ptr<Image> result;
   for (size_t i=0; i<m_imageList.size(); ++i)
   {
      if (m_imageList[i]->getImageId() == imageId)
      {
         result = m_imageList[i];
         break;
      }
   }
   return result;
}

shared_ptr<GroundControlPoint> PhotoBlock::getGroundPoint(const std::string& gcpId)
{
   std::shared_ptr<GroundControlPoint> result;
   for (size_t i=0; i<m_gcpList.size(); ++i)
   {
      if (m_gcpList[i]->getId() == gcpId)
      {
         result = m_gcpList[i];
         break;
      }
   }
   return result;
}

shared_ptr<TiePoint> PhotoBlock::getTiePoint(unsigned int tpId)
{
   std::shared_ptr<TiePoint> result;
   for (size_t i=0; i<m_tiePointList.size(); ++i)
   {
      if (m_tiePointList[i]->getTiePointId() == tpId)
      {
         result = m_tiePointList[i];
         break;
      }
   }
   return result;
}

void PhotoBlock::loadJSON(const Json::Value& pb_json_node)
{
   // Always do images first, as tiepoints will be using the image list to correct image ID:
   if (pb_json_node.isMember("images"))
   {
      const Json::Value& listJson = pb_json_node["images"];
      unsigned int count = listJson.size();
      for (unsigned int i=0; i<count; ++i)
      {
         const Json::Value& jsonItem = listJson[i];
         shared_ptr<Image> item (new Image(jsonItem));
         m_imageList.push_back(item);
      }
   }

   if (pb_json_node.isMember("groundPoints"))
   {
      const Json::Value& listJson = pb_json_node["groundPoints"];
      unsigned int count = listJson.size();
      for (unsigned int i=0; i<count; ++i)
      {
         const Json::Value& jsonItem = listJson[i];
         shared_ptr<GroundControlPoint> item (new GroundControlPoint(jsonItem));
         m_gcpList.push_back(item);
      }
   }

   if (pb_json_node.isMember("tiePoints"))
   {
      const Json::Value& listJson = pb_json_node["tiePoints"];
      unsigned int count = listJson.size();
      for (unsigned int i=0; i<count; ++i)
      {
         const Json::Value& jsonItem = listJson[i];
         shared_ptr<TiePoint> item (new TiePoint(jsonItem));
         m_tiePointList.push_back(item);
      }
   }

   fixTpImageIds();
}

void PhotoBlock::saveJSON(Json::Value& pbJSON) const
{
   Json::Value imageListJson (Json::arrayValue);
   unsigned int count = m_imageList.size();
   for (unsigned int i=0; i<count; ++i)
   {
      m_imageList[i]->saveJSON(imageListJson[i]);
   }
   pbJSON["images"] = imageListJson;

   Json::Value gcpListJson (Json::arrayValue);
   count = m_gcpList.size();
   for (unsigned int i=0; i<count; ++i)
   {
      m_gcpList[i]->saveJSON(gcpListJson[i]);
   }
   pbJSON["groundPoints"] = gcpListJson;

   Json::Value tpListJson (Json::arrayValue);
   count = m_tiePointList.size();
   for (unsigned int i=0; i<count; ++i)
   {
      m_tiePointList[i]->saveJSON(tpListJson[i]);
   }
   pbJSON["tiePoints"] = tpListJson;
}

void PhotoBlock::fixTpImageIds()
{
   unsigned int count = m_tiePointList.size();
   for (unsigned int t=0; t<count; ++t)
   {
      m_tiePointList[t]->fixTpImageIds(m_imageList);
      unsigned int num_images = m_tiePointList[t]->getImageCount();
      for (unsigned int i=0; i<num_images; ++i)
      {

      }
   }
}

#if OSSIM_HAS_MSP
void PhotoBlock::getCsmModels(MSP::CsmSensorModelList& csmModelList)
{
   csmModelList.clear();
   for (int i=0; i<m_imageList.size(); ++i)
   {
      const csm::RasterGM* model = m_imageList[i]->getCsmSensorModel();
      csmModelList.push_back(model);
      //clog<<"imageId: <"<<model->getImageIdentifier()<<">"<<endl;//###TODO
   }
}

void PhotoBlock::setCsmModels(MSP::CsmSensorModelList& csmModelList)
{
   ostringstream xmsg;
   xmsg<<__FILE__<<":"<<__LINE__<<" CSM models list provided is not synchronized with image list in"
         " photoblock. This should not happen.";

   if (m_imageList.size() != csmModelList.size())
      throw ossimException(xmsg.str());

   for (int i=0; i<m_imageList.size(); ++i)
   {
      if (m_imageList[i]->getImageId() != csmModelList[i]->getImageIdentifier())
         throw ossimException(xmsg.str());

      m_imageList[i]->setCsmSensorModel(csmModelList[i]);
   }
}

#endif


} // end namespace ISA
