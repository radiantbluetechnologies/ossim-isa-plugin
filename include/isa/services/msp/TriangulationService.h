//**************************************************************************************************
//
//     OSSIM Open Source Geospatial Data Processing Library
//     See top level LICENSE.txt file for license information
//
//**************************************************************************************************
#ifndef TriangulationService_HEADER
#define TriangulationService_HEADER 1

#include <geometry/ImagePoint.h>
#include <geometry/GroundPoint.h>
#include <isa/services/ServiceBase.h>
#include <isa/common/PhotoBlock.h>
#include <PointExtraction/TriangulationResult.h>
#include <memory>

namespace ISA
{

/**
 * Top-level class for interfacing to MSP triangulation from 3DISA client.
 * Limitation: Does not consider cross correlation between sensors.
 * Determines best subset of candidate images from set provided. Throws exception if input
 * vectors are not the same dimensions, or if underlying MSP service encounters error. This
 * version assumes all sensor models are a priori.
 * TODO: Will there be a need for source selection among an a posteriori photoblock?
 */
class TriangulationService : public ServiceBase
{
public:
   TriangulationService();
   ~TriangulationService();

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
   void fillGcpList(MSP::GroundPointList& mspGroundPts);
   void fillTpList(MSP::ImagePointList& mspImagePts);

   std::shared_ptr<PhotoBlock> m_photoBlock;
   std::shared_ptr<MSP::PES::TriangulationResult> m_triangulationResult;

};

} // End namespace ISA

#endif
