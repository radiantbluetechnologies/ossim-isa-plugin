//**************************************************************************************************
//
//     OSSIM Open Source Geospatial Data Processing Library
//     See top level LICENSE.txt file for license information
//
//**************************************************************************************************

#include <isa/Config.h>
#if OSSIM_HAS_MSP

#include <isa/services/msp/MensurationService.h>
#include <ossim/base/ossimException.h>
#include <PointExtraction/PointExtractionService.h>
#include<geometry/GroundPointResult.h>
#include <csmutil/CsmSensorModelList.h>
#include <isa/common/SessionManager.h>

using namespace std;

namespace ISA
{

MensurationService::MensurationService()
:  m_resultsInEcf (false)
{
}

MensurationService::~MensurationService()
{
}

void MensurationService::loadJSON(const Json::Value& queryRoot)
{
   ostringstream xmsg;
   xmsg<<"MensurationService::loadJSON() EXCEPTION: ";

   if (queryRoot.isMember("sessionId"))
   {
      xmsg<<"Loading of existing session is not yet implemented!";
      throw ossimException(xmsg.str());

      // TODO: SessionManager::getSession(sessionId) NOT YET IMPLEMENTED
      // Parse the JSON request and initialize TiePointFinder object with active session. This will
      // recall the session from the database along with all image models (from photoblock).
      //string sessionId = queryRoot["sessionID"].asString();
      //shared_ptr<Session> session = SessionManager::getSession(sessionId);
      //if (!session)
      //{
      //   xmsg << "Fatal: Null session returned trying to access with sessionId <"<<sessionId<<">!";
      //   throw ossimException(xmsg.str());
      //}
      //std::shared_ptr<PhotoBlock> photoBlock (session->getPhotoBlock().get());
      //m_imageList = photoBlock->getImageList();
   }
   else if (queryRoot.isMember("photoblock"))
   {
      PhotoBlock pb (queryRoot["photoblock"]);

   }
   else
   {
      if (queryRoot.isMember("images"))
      {
         const Json::Value& listJson = queryRoot["images"];
         unsigned int count = listJson.size();
         for (unsigned int i=0; i<count; ++i)
         {
            const Json::Value& jsonItem = listJson[i];
            shared_ptr<Image> item (new Image(jsonItem));
            string imageId = item->getImageId();
            m_imageList.insert(pair<string, shared_ptr<Image> >(imageId, item));
         }
      }
   }

   string coordSystem = queryRoot["outputCoordinateSystem"].asString();
   if (coordSystem == "ecf")
      m_resultsInEcf = true;

   const Json::Value& observations = queryRoot["observations"];
   for (unsigned int p=0; p<observations.size(); ++p)
   {
      shared_ptr<PointObservation> obs (new PointObservation);
      obs->pointId = observations[p]["id"].asString();
      //cout<<"pointId: "<<obs->pointId<<endl;
      const Json::Value& imagePointList = observations[p]["imagePoints"];
      for (unsigned int i=0; i<imagePointList.size(); ++i)
      {
         shared_ptr<Measurement> measurement (new Measurement);
         measurement->imageId      = imagePointList[i]["imageId"].asString();
         measurement->imagePoint.x = imagePointList[i]["x"].asDouble();
         measurement->imagePoint.y = imagePointList[i]["y"].asDouble();
         //cout<<"imageId: "<<measurement->imageId<<endl;
         //cout<<"imagePoint: "<<measurement->imagePoint<<endl;

         const Json::Value& cov = imagePointList[i]["covariance"];
         measurement->covariance.setElement(0, 0, cov[0].asDouble());
         measurement->covariance.setElement(1, 1, cov[1].asDouble());
         measurement->covariance.setElement(0, 1, cov[2].asDouble());
         measurement->covariance.setElement(1, 0, cov[2].asDouble()); // symmetric

         obs->measurements.push_back(measurement);
      }
      m_observations.push_back(obs);
   }
}

void MensurationService::saveJSON(Json::Value& json) const
{
   Json::Value mensurationReport;

   for (unsigned int p=0; p<m_observations.size(); p++)
   {
      Json::Value observation;
      observation["pointId"] = m_observations[p]->pointId;
      observation["ce90"] = m_observations[p]->ce90;
      observation["le90"] = m_observations[p]->le90;

      if (m_resultsInEcf)
      {
         observation["ecfPoint"]["x"] = m_observations[p]->ecfPt.x();
         observation["ecfPoint"]["y"] = m_observations[p]->ecfPt.y();
         observation["ecfPoint"]["z"] = m_observations[p]->ecfPt.z();
      }
      else
      {
         ossimGpt geoPt (m_observations[p]->ecfPt);
         observation["geoPoint"]["lat"] = geoPt.lat;
         observation["geoPoint"]["lon"] = geoPt.lon;
         observation["geoPoint"]["hgt"] = geoPt.hgt;
      }
      mensurationReport[p] = observation;
   }
   json["mensurationReport"] = mensurationReport;
}

void MensurationService::execute()
{
   ostringstream xmsg;
   xmsg<<"MensurationService::execute() EXCEPTION: ";

   try
   {
      MSP::PES::PointExtractionService pes;

      // Loop over all point observations:
      for (int p=0; p<m_observations.size(); p++)
      {
         shared_ptr<PointObservation> observation (m_observations[p]);
         MSP::CsmSensorModelList csmModelList;
         MSP::ImagePointList imagePts;
         string pointId = observation->pointId;

         // Loop over each image in the observation:
         for( int i=0; i < observation->measurements.size(); i++ )
         {
            std::shared_ptr<Measurement> measurement = observation->measurements[i];

            // Establish image sensor model:
            string imageId = measurement->imageId;
            std::map<std::string, std::shared_ptr<Image> >::iterator imgPtr =
                  m_imageList.find(imageId);
            if (imgPtr == m_imageList.end())
            {
               xmsg<<"Bad image ID found in measurement list: <"<<imageId<<">";
               throw ossimException(xmsg.str());
            }

            shared_ptr<Image> image ( imgPtr->second );
            const csm::RasterGM *csm = image->getCsmSensorModel();
            if (csm == NULL)
            {
               xmsg<<"Null sensor model returned for image <"<<imageId<<">";
               throw ossimException(xmsg.str());
            }
            csmModelList.push_back( csm );

            // Establish image point measurement:
            ossimDpt ip = measurement->imagePoint;
            imagePts.push_back(MSP::ImagePoint(ip.line, ip.samp,
                                               imageId, pointId,
                                               measurement->covariance));
         }

         // Now do ray intersection:
         MSP::GroundPointResult groundPointResult;
         pes.computeIntersections( csmModelList, imagePts, NULL, groundPointResult);
         const MSP::GroundPoint& mspGpt = groundPointResult.getGroundPoint();
         observation->ecfPt = ossimEcefPoint(mspGpt.getX(), mspGpt.getY(), mspGpt.getZ());
         computeCELE(mspGpt, observation->ce90, observation->le90);
      }
   }
   catch (exception& e)
   {
      ossimNotify(ossimNotifyLevel_FATAL)<<"MensurationService::execute() -- "<<e.what()<<endl;
   }
}

void MensurationService::computeCELE(const MSP::GroundPoint& gp, double& ce90, double& le90)
{
   // First need a transform from ECF to ENU:
   double x = gp.getX();
   double y = gp.getY();
   double z = gp.getZ();
   ossimGpt gpt (ossimEcefPoint(x, y, z));

   double sin_lam = ossim::sind(gpt.lon);
   double sin_phi = ossim::sind(gpt.lat);
   double cos_lam = ossim::cosd(gpt.lon);
   double cos_phi = ossim::cosd(gpt.lat);

   MSP::Matrix rot (3,3);
   rot[0][0] = -sin_lam        ;    rot[0][1] =  cos_lam        ;   rot[0][2] = 0;
   rot[1][0] = -cos_lam*sin_phi;    rot[1][1] = -sin_lam*sin_phi;   rot[1][2] = cos_phi;
   rot[2][0] =  cos_lam*cos_phi;    rot[2][1] =  sin_lam*cos_phi;   rot[2][2] = sin_phi;
   MSP::Matrix rot_transpose (rot);
   rot_transpose.transpose();

   //cout<<"rot: "<<rot<<endl;
   const MSP::Matrix& ecfCov = gp.getCovariance();
   //cout<<"ecfCov: "<<ecfCov<<endl;
   MSP::Matrix enuCov = rot.multiply(ecfCov.multiply(rot_transpose));
   //cout<<"enuCov: "<<enuCov<<endl;
   double a = 0.5 * (enuCov[0][0] + enuCov[1][1]);
   double b2 = (enuCov[0][0]-enuCov[1][1])*(enuCov[0][0]-enuCov[1][1]) + 4*enuCov[0][1]*enuCov[0][1];
   double b = 0.5 * sqrt(b2);
   double su = sqrt(a + b);
   double sv = sqrt(a - b);
   double c = su/sv;
   ce90 = (1.6545 - 0.13913*c + 0.6324*c*c) * su;
   le90 = 1.6449*sqrt(enuCov[2][2]);

   if (0)
   {
      std::cout<<"ENU Covariance: "
            <<"\n\t"<<enuCov[0][0]<<"   "<<enuCov[0][1]<<"   "<<enuCov[0][2]
            <<"\n\t"<<enuCov[1][0]<<"   "<<enuCov[1][1]<<"   "<<enuCov[1][2]
            <<"\n\t"<<enuCov[2][0]<<"   "<<enuCov[2][1]<<"   "<<enuCov[2][2]<<"\n"<<endl;
      std::cout<<"Computed CE90: "<<ce90<<std::endl;
      std::cout<<"Computed LE90: "<<le90<<"\n"<<std::endl;
   }
}

} // end O2REG namespace
#endif
