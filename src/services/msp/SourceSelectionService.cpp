//**************************************************************************************************
//
//     OSSIM Open Source Geospatial Data Processing Library
//     See top level LICENSE.txt file for license information
//
//**************************************************************************************************



// TODO: Consider consolidating MSP and CSM plugins into one, and putting all
// MSP interface functionality there. Because I need CK's code to create CSM model from image
// filename.

#include <isa/Config.h>
#include <isa/services/msp/SourceSelectionService.h>
#include <ossim/base/ossimException.h>
#include <ossim/base/ossimGpt.h>
#include <ossim/base/ossimEcefPoint.h>
#include <ossim/base/ossimFilename.h>
#include <ossim/base/ossimDirectory.h>
#include <SourceSelection/SourceSelectionService.h>
#include <SourceSelection/CandidateImage.h>
#include <SourceSelection/SourceSelectionCriteria.h>
#include <csm/RasterGM.h>
#include <iostream>
#include <math.h>
#include <isa/common/PhotoBlock.h>
#include <isa/common/SessionManager.h>
#include <isa/common/Image.h>
#include <json/json.h>
#include <cstdlib>
#include <iostream>

#define DEBUG_ON false

using namespace std;
namespace ISA
{
SourceSelectionService::SourceSelectionService()
:  m_desiredCE90 (0),
   m_desiredLE90 (0),
   m_meetsCriteria (false),
   m_estimatedCE90 (0),
   m_estimatedLE90 (0)
{
}

SourceSelectionService::~SourceSelectionService()
{
}

void SourceSelectionService::execute()
{
   ostringstream xmsg;
   xmsg<<"SourceSelectionService::execute(): ";

   m_estimatedCE90 = -1;
   m_estimatedLE90 = -1;

   size_t ncands = m_candidateImages.size();
   if ((ncands == 0) || (m_mustUse.size() != ncands))
      return;

   // Need the reference ground point in ECEF coordinates:
   double x, y, z;
   csm::EcefCoord csmEcefPt (m_refPt.x(), m_refPt.y(), m_refPt.z());

   // Prepare MSP data structures:
   MSP::SS::CandidateImageList mspCandidates;
   MSP::GroundPointList mspGcpList; // Left empty
   MSP::JointCovMatrixList mspJcmList; // TODO: Left empty -- no way to model cross-correlation

   // Start a new session NOW:
   m_session = SessionManager::newSession();

   // Loop to add all images:
   for (size_t i=0; i<ncands; ++i)
   {
      shared_ptr<Image> image (m_candidateImages[i]);
      const char* fname = image->getFilename().c_str();
      const char* modelName =image->getModelName().c_str();
      const csm::RasterGM* model = image->getCsmSensorModel();
      if (!model)
      {
         xmsg<<"Could not instantiate sensor model from image file: <"<<fname<<">."<<endl;
         throw ossimException(xmsg.str());
      }

      // Construct MSP candidate image representing this file:
      MSP::SS::CandidateImage::Usage usage_req = MSP::SS::CandidateImage::CAN_USE;
      if (m_mustUse[i])
         usage_req = MSP::SS::CandidateImage::MUST_USE;
      mspCandidates.push_back(MSP::SS::CandidateImage(model, usage_req));
   }

   // Find best subset of images:
   MSP::SS::SourceSelectionCriteria mspCriteria;
   mspCriteria.setMinImages(2);
   mspCriteria.setDesiredErrorProp(m_desiredCE90, m_desiredLE90);
   MSP::SS::SourceSelectionService mspSSS;
   MSP::SS::SourceSelectionResult mspAbsResult;
   mspSSS.sourceSelect(mspCandidates, mspJcmList, mspCriteria, csmEcefPt, mspAbsResult);

   // A new photoblock is initialized to represent images and eventually tiepoints and GCPs used
   // in this session. The candidate images will be added to it:
   shared_ptr<PhotoBlock> photoblock (m_session->getPhotoBlock());

   // Process result. Need to correlate instance of sensor model with corresponding filename:
   m_meetsCriteria = mspAbsResult.meetsCriteria();
   const MSP::CsmSensorModelList& subsetModels = mspAbsResult.getSensorModelList();
   for (int i=0; i<subsetModels.size(); ++i)
   {
      string imageID1 = subsetModels[i]->getImageIdentifier();
      for (int j=0; j<m_candidateImages.size(); ++j)
      {
         string imageID2 = m_candidateImages[j]->getImageId();
         if (imageID1.compare(imageID2) == 0)
         {
            // found match. Need to instantiate the Image object and save to list:
#if DEBUG_ON
            clog<<"Image ID match: "<<imageID1<<endl;
#endif
            photoblock->addImage(m_candidateImages[j]);
            break;
         }
      }
   }

   // Need to compute CE/LE given covariance:
   computeAccuracy(mspAbsResult);

   // Finally, save it to the Db:
   //m_session->saveState();

   return;
}

void SourceSelectionService::loadJSON(const Json::Value& queryRoot)
{
   // Fetch reference ground point and desired accuracy:
   Json::Value point = queryRoot["point"];
   ossimGpt refGpt;
   refGpt.lat = point["lat"].asDouble();
   refGpt.lon = point["lon"].asDouble();
   refGpt.hgt = 0.0;
   m_refPt = ossimEcefPoint(refGpt);

   Json::Value desiredAccuracy = queryRoot["desiredAccuracy"];
   m_desiredCE90 = desiredAccuracy["ce90"].asDouble();
   m_desiredLE90 = desiredAccuracy["le90"].asDouble();

   // Fetch list of candidate images along with boolean must-use flags:
   Json::Value candidates = queryRoot["candidates"];
   int numCandidates = candidates.size();
   Json::Value defaultEntryIndex ((Json::UInt) 0);

   for ( int index = 0; index < numCandidates; ++index )
   {
      Json::Value candidate = candidates[index];
      m_candidateImages.push_back(shared_ptr<Image>(new Image(candidate)));
      m_mustUse.push_back(candidate["mustUse"].asBool());
   }
}

void SourceSelectionService::saveJSON(Json::Value& responseJson) const
{
   // Represent results in response JSON:
   double estimatedCE90, estimatedLE90;
   responseJson["predictedAccuracy"]["ce90"] = m_estimatedCE90;
   responseJson["predictedAccuracy"]["le90"] = m_estimatedLE90;
   responseJson["meetsCriteria"] = m_meetsCriteria;

   // Write the image information:
   shared_ptr<PhotoBlock> photoblock = m_session->getPhotoBlock();
   Json::Value pbJson;
   photoblock->saveJSON(pbJson);
   responseJson["photoblock"] = pbJson;
}

void SourceSelectionService::computeAccuracy(MSP::SS::SourceSelectionResult& mspAbsResult)
{
   const MSP::Matrix& cov = mspAbsResult.getEnuCovariance();

   // CE and LE need to be computed from the 3x3 ENU covaiance returned because MSP does not make
   // those values available. Take horizontal EC 2x2, and determine eigenvalues to be max and min
   // error ellipse axes, then compute CE90 given min/max ratio. Use single U variance for LE90:
   double a = 0.5 * (cov[0][0] + cov[1][1]);
   double b2 = (cov[0][0]-cov[1][1])*(cov[0][0]-cov[1][1]) + 4*cov[0][1]*cov[0][1];
   double b = 0.5 * sqrt(b2);
   double su = sqrt(a + b);
   double sv = sqrt(a - b);
   double c = su/sv;
   m_estimatedCE90 = (1.6545 - 0.13913*c + 0.6324*c*c) * su;
   m_estimatedLE90 = 1.6449 * sqrt(cov[2][2]);

#if DEBUG_ON
   // Report results to stdout:
   clog << "\nResults from MSP sourceSelect(): ";
   clog << mspAbsResult.toString() << endl << endl;
   clog<<"ENU Covariance: "
                     <<"\n\t"<<cov[0][0]<<"   "<<cov[0][1]<<"   "<<cov[0][2]
                     <<"\n\t"<<cov[1][0]<<"   "<<cov[1][1]<<"   "<<cov[1][2]
                     <<"\n\t"<<cov[2][0]<<"   "<<cov[2][1]<<"   "<<cov[2][2]<<"\n"<<endl;
   const std::vector<std::shared_ptr<Image> >& bestSubset =
         m_session->getPhotoBlock()->getImageList();
   clog << "Results from 3DISA sourceSelect(): \n  Image File Selection:"<<endl;
   for (int i=0; i<bestSubset.size(); ++i)
   {
      clog<<"    "<<bestSubset[i]<<endl;
   }
   clog<<"\n  Computed CE90: "<<m_estimatedCE90<<endl;
   clog<<"  Computed LE90: "<<m_estimatedLE90<<endl;
   clog<<"  meets criteria: "<<m_meetsCriteria<<"\n"<<endl;
#endif
}

} // end O2REG namespace

