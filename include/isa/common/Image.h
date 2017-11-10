//**************************************************************************************************
//
//     OSSIM Open Source Geospatial Data Processing Library
//     See top level LICENSE.txt file for license information
//
//**************************************************************************************************
#ifndef Image_HEADER
#define Image_HEADER 1

#include <isa/Config.h>
#include <map>
#include <memory>
#include <string>
#include <ossim/base/ossimGpt.h>
#include <ossim/base/ossimConstants.h>
#include <ossim/matrix/newmat.h>
//#include <ossim/imaging/ossimImageHandler.h>
#include <isa/common/JsonInterface.h>

#if OSSIM_HAS_MSP
#include <csm/RasterGM.h>
#else
#include <ossim/projection/ossimSensorModel.h>
#endif

namespace ISA
{

/**
 * Class representing an Image as used by ossim-msp services.
 */
class OSSIM_DLL Image : public JsonInterface,
                        public std::enable_shared_from_this<Image>
{
public:
   static unsigned int UNASSIGNED_PHOTOBLOCK_INDEX;

   Image(const std::string& imageId,
         const std::string& filename,
         const std::string& modelName="",
         unsigned int entryIndex=0,
         unsigned int band=1);

   Image(const Json::Value& image_json_node);

   ~Image();

   std::string getImageId() const { return m_imageId; }
   std::string getFilename() const { return m_filename; }
   std::string getModelName() const { return m_modelName; }
   unsigned int getEntryIndex() const { return m_entryIndex; }
   unsigned int getActiveBand() const { return m_activeBand; }

   void setImageId(const std::string& id) { m_imageId = id; }
   void setFilename(const std::string& f) { m_filename = f; }
   void setEntryIndex(unsigned int i) { m_entryIndex = i; }

   /**
    * Returns all available sensor model plugins and model names for this image:
    * @param availableModels List of <plugin-name, model-name> pairs.
    */
    void getAvailableModels(std::vector< pair<std::string, std::string> >& availableModels) const;

    /**
    * Refer to <a href="https://docs.google.com/document/d/1DXekmYm7wyo-uveM7mEu80Q7hQv40fYbtwZq-g0uKBs/edit?usp=sharing">3DISA API document</a>
    * for JSON format used.
    */
    virtual void loadJSON(const Json::Value& json);

    /**
    * Refer to <a href="https://docs.google.com/document/d/1DXekmYm7wyo-uveM7mEu80Q7hQv40fYbtwZq-g0uKBs/edit?usp=sharing">3DISA API document</a>
    * for JSON format used.
    */
    virtual void saveJSON(Json::Value& json) const;

#if OSSIM_HAS_MSP
    /**
     * Replaces the MSP CSM sensor model with updated model.
     */
    void setCsmSensorModel(const csm::RasterGM* model);

    /**
     * Returns the MSP CSM sensor model associated with the image. If the sensor model name is not
     * defined, the first (most accurate) model will be selected.
     */
    const csm::RasterGM* getCsmSensorModel();

    /**
     * Returns the ISD portion
     */
    static void getISD(std::string isd, const std::string& filename, unsigned int entryIndex=0)
    {
       // TODO
    }
#endif

private:
   std::string m_imageId;
   std::string m_filename;
   unsigned int m_entryIndex;
   unsigned int m_activeBand;
   std::string m_modelName;
   //ossimRefPtr<ossimImageHandler> m_handler;
   std::vector< pair<std::string, std::string> > m_availableModel;

#if OSSIM_HAS_MSP
   const csm::RasterGM* m_csmModel;
#else
   ossimRefPtr<ossimSensorModel> m_sensorModel;
#endif
};

} // End namespace ISA

#endif
