//**************************************************************************************************
//
//     OSSIM Open Source Geospatial Data Processing Library
//     See top level LICENSE.txt file for license information
//
//**************************************************************************************************
#ifndef MensurationService_HEADER
#define MensurationService_HEADER 1

#include<geometry/GroundPoint.h>
#include <isa/services/ServiceBase.h>
#include <isa/common/Image.h>
#include <ossim/base/ossimDpt.h>
#include <ossim/base/ossimGpt.h>
#include <memory>

namespace ISA
{

/**
 * Top-level class for interfacing to MSP mensuration.
 */
class MensurationService : public ServiceBase
{
   struct Measurement
   {
      std::string imageId;
      ossimDpt imagePoint;
      ossimDpt sigmas;
      double rho;
   };

   struct PointObservation
   {
      PointObservation() : ce90(0), le90(0) {}

      std::string pointId;
      ossimEcefPoint ecfPt;
      vector< std::shared_ptr<Measurement> > measurements;
      double ce90;
      double le90;
   };

public:
   MensurationService();
   ~MensurationService();

   /*
   * Refer to <a href="https://docs.google.com/document/d/1DXekmYm7wyo-uveM7mEu80Q7hQv40fYbtwZq-g0uKBs/edit?usp=sharing">3DISA API document</a>
   * for JSON format used.
   */
   virtual void loadJSON(const Json::Value& json);

   /*
   * Refer to <a href="https://docs.google.com/document/d/1DXekmYm7wyo-uveM7mEu80Q7hQv40fYbtwZq-g0uKBs/edit?usp=sharing">3DISA API document</a>
   * for JSON format used.
   */
   virtual void saveJSON(Json::Value& json) const;

   virtual void execute();

private:
   void computeCELE(const MSP::GroundPoint& gpr, double& ce90, double& le90);

   std::map<std::string, std::shared_ptr<Image> > m_imageList; // pair:(imageId, Image)
   std::vector<std::shared_ptr<PointObservation> > m_observations;
   bool m_resultsInEcf;
};

} // End namespace ISA

#endif
