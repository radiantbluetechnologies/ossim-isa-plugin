//**************************************************************************************************
//
//     OSSIM Open Source Geospatial Data Processing Library
//     See top level LICENSE.txt file for license information
//
//**************************************************************************************************
#ifndef Session_HEADER
#define Session_HEADER 1

#include <isa/common/JsonInterface.h>
#include <isa/common/PhotoBlock.h>
#include <ossim/base/ossimReferenced.h>
#include <string>
#include <vector>
#include <memory>

namespace ISA
{
/**
 * Class representing a 3DISA session. Object manages one instance of a photoblock
 * (either a priori or a posteriori), and any mensuration performed. A new session is needed if
 * either starting from scratch, or adding images, tiepoints or GCPs to the session (anything
 * that affects the results of triangulation). If only new mensuration measurements are made, they
 * can be added to the existing session.
 */
class Session : public JsonInterface,
                public std::enable_shared_from_this<Session>
{
public:
   /**
    * Default constructor invents a new sessionId and creates an empty photoblock
    */
   Session();
   Session(const Json::Value& json);

   /**
    * Copy constructor usefule when starting from prior existing session but adding new images,
    * tiepoints, and/or GCPs.
    */
   Session(const Session& copyThis);

   ~Session();

   void setDescription(const std::string& description) { m_description = description; }

   shared_ptr<PhotoBlock> getPhotoBlock();

   const std::string& getSessionId() const { return m_sessionId; }

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
   std::string m_sessionId;
   std::string m_description;
   std::shared_ptr<PhotoBlock> m_photoBlock;

};

} // end namespace ISA
#endif // Session_HEADER
