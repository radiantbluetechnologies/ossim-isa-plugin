//**************************************************************************************************
//
//     OSSIM Open Source Geospatial Data Processing Library
//     See top level LICENSE.txt file for license information
//
//**************************************************************************************************
#ifndef SourceSelectionService_HEADER
#define SourceSelectionService_HEADER 1

#include <SourceSelection/SourceSelectionResult.h>
#include <isa/services/ServiceBase.h>
#include <isa/common/Image.h>
#include <isa/common/Session.h>
#include <vector>
#include <memory>
#include <string>


namespace ISA
{

/**
 * Top-level class for interfacing to MSP source selection from 3DISA client.
 * Limitation: Does not consider cross correlation between sensors.
 * Determines best subset of candidate images from set provided. Throws exception if input
 * vectors are not the same dimensions, or if underlying MSP service encounters error. This
 * version assumes all sensor models are a priori.
 * TODO: Will there be a need for source selection among an a posteriori photoblock?
 */
class SourceSelectionService : public ServiceBase
{
public:
   SourceSelectionService();
   ~SourceSelectionService();

   virtual void execute();

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

private:
   void computeAccuracy(MSP::SS::SourceSelectionResult& mspResults);

   ossimEcefPoint m_refPt;
   std::shared_ptr<Session> m_session;
   std::vector< std::shared_ptr<Image> > m_candidateImages;
   std::vector<bool> m_mustUse;
   double m_desiredCE90;
   double m_desiredLE90;
   bool m_meetsCriteria;
   double m_estimatedCE90;
   double m_estimatedLE90;
};

} // End namespace ISA

#endif
