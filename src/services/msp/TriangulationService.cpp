//**************************************************************************************************
//
//     OSSIM Open Source Geospatial Data Processing Library
//     See top level LICENSE.txt file for license information
//
//**************************************************************************************************

#include <isa/Config.h>
#if OSSIM_HAS_MSP

#include <isa/services/msp/TriangulationService.h>
#include <ossim/base/ossimException.h>
#include <PointExtraction/PointExtractionService.h>
#include <csmutil/CsmSensorModelList.h>
#include <isa/common/SessionManager.h>

using namespace std;

namespace ISA
{

TriangulationService::TriangulationService()
{
}

TriangulationService::~TriangulationService()
{
}

void TriangulationService::loadJSON(const Json::Value& queryRoot)
{
   ostringstream xmsg;

   if (queryRoot.isMember("sessionId"))
   {
      // TODO: SessionManager::getSession(sessionId) NOT YET IMPLEMENTED
      // Parse the JSON request and initialize TiePointFinder object with active session. This will
      // recall the session from the database along with all image models (from photoblock).
      string sessionId = queryRoot["sessionID"].asString();
      shared_ptr<Session> session = SessionManager::getSession(sessionId);
      if (!session)
      {
         xmsg << "Fatal: Null session returned trying to access with sessionId <"<<sessionId<<">!";
         throw ossimException(xmsg.str());
      }
      m_photoBlock.reset(session->getPhotoBlock().get());
   }
   else
   {
      // Load the active photoblock from JSON:
      const Json::Value& pbJson = queryRoot["photoblock"];
      if (!pbJson)
      {
         xmsg <<__FILE__<<":"<<__LINE__<<" Fatal: Null photoblock returned!";
         throw ossimException(xmsg.str());
      }
      m_photoBlock.reset(new PhotoBlock(pbJson));
   }
}

void TriangulationService::saveJSON(Json::Value& json) const
{
   Json::Value pbJson;
   m_photoBlock->saveJSON(pbJson);
   json["photoblock"] = pbJson;

   string results = m_triangulationResult->toString(true);
   json["triangulationResult"] = results;

   //clog << results << endl;
}

void TriangulationService::execute()
{
   ostringstream xmsg;

   try
   {
      // Instantiate the sensor models and assemble into an MSP list:
      MSP::CsmSensorModelList csmModelList;
      m_photoBlock->getCsmModels(csmModelList);

      // Assemple all ground control points and image points in the photoblock:
      MSP::GroundPointList mspGroundPts;
      fillGcpList(mspGroundPts);

      MSP::ImagePointList mspImagePts;
      fillTpList(mspImagePts);

      // Establish all auto and cross covariances for sensor models and GCPs:
      MSP::JointCovMatrix jcm = csmModelList.getJointCovMatrix();
      jcm.setObjects( csmModelList, mspGroundPts );
      jcm.validate(csmModelList, mspGroundPts);
      m_photoBlock->setJointCovariance(jcm);

      // Define a blunder strategy:
      MSP::PES::BlunderStrategy blunderStrategy;
      //clog<<"\nBlunderStrategy:\n"<<blunderStrategy.toString()<<endl;

      // Pass to MSP triangualtion service:
      MSP::PES::PointExtractionService pes;
      m_triangulationResult =
            shared_ptr<MSP::PES::TriangulationResult>(new MSP::PES::TriangulationResult);
      pes.triangulate(csmModelList, mspImagePts, jcm, blunderStrategy, *m_triangulationResult);
      clog<<"\n"<<m_triangulationResult->toString(true)<<endl;

      // Update photoblock with a posteriori values: SHOULD NOT BE NEEDED AS OBJECTS ARE SHARED
      //   m_photoBlock->setCsmModels(csmModelList);
      //   m_photoBlock->setJointCovariance(m_triangulationResult->getJointCov());
   }
   catch (exception& e)
   {
      ossimNotify(ossimNotifyLevel_FATAL)<<"TriangulationService::execute() -- "<<e.what()<<endl;
   }
}

void TriangulationService::fillGcpList(MSP::GroundPointList& mspGroundPts)
{
   MSP::Matrix mspCov(3,3);
   NEWMAT::SymmetricMatrix ossimCov;
   mspCov.zero();
   ossimEcefPoint ecf;
   const vector< shared_ptr<GroundControlPoint> >& gcpList = m_photoBlock->getGroundPointList();
   for (int g=0; g<gcpList.size(); ++g)
   {
      ecf = gcpList[g]->getECF();
      ossimCov = gcpList[g]->getCovariance();
      mspCov.setElement(0, 0, ossimCov(1,1));
      mspCov.setElement(1, 1, ossimCov(2,2));
      mspCov.setElement(2, 2, ossimCov(3,3));
      mspCov.setElement(0, 1, ossimCov(1,2));
      mspCov.setElement(0, 2, ossimCov(1,3));
      mspCov.setElement(1, 2, ossimCov(2,3));
      mspCov.setElement(1, 0, ossimCov(2,1));
      mspCov.setElement(2, 0, ossimCov(3,1));
      mspCov.setElement(2, 1, ossimCov(3,2));
      MSP::GroundPoint mspGpt (ecf.x(), ecf.y(), ecf.z(), gcpList[g]->getId());
      mspGpt.setCovariance(mspCov);
      mspGroundPts.push_back(mspGpt);
   }
}

void TriangulationService::fillTpList(MSP::ImagePointList& mspImagePts)
{
   MSP::Matrix mspCov(2,2);
   NEWMAT::SymmetricMatrix ossimCov;
   mspCov.zero(); // off-diagonals zeroed
   string imageId;
   string pointId;
   unsigned int imgCount=0;
   ossimDpt imagePt;
   const std::vector<std::shared_ptr<TiePoint> >&  tiePoints =
         m_photoBlock->getTiePointList();
   for (int t=0; t<tiePoints.size(); ++t)
   {
      // Fetch the photoblock's entry for this tiepoint:
      pointId = tiePoints[t]->getGcpId();
      if (pointId.empty())
         pointId = tiePoints[t]->getTiePointId();
      imgCount = tiePoints[t]->getImageCount();
      for (int i=0; i<imgCount; ++i)
      {
         tiePoints[t]->getImagePoint(i, imageId, imagePt, ossimCov);
         mspCov.setElement(0, 0, ossimCov(2,2)); // flip for x,y versus line sample
         mspCov.setElement(1, 1, ossimCov(1,1));
         MSP::ImagePoint mspImagePoint(imagePt.line, imagePt.samp, imageId, pointId, mspCov);
         //clog<<"mspImagePoint:\n"<<mspImagePoint.toString()<<endl;//###
         mspImagePts.push_back( mspImagePoint );
      }
   }
}

} // end O2REG namespace
#endif
