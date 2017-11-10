//**************************************************************************************************
//
//     OSSIM Open Source Geospatial Data Processing Library
//     See top level LICENSE.txt file for license information
//
//**************************************************************************************************
#ifndef SessionManager_HEADER
#define SessionManager_HEADER 1

#include <isa/common/Session.h>
#include <string>
#include <map>
#include <memory>
#include <ossim/base/ossimRefPtr.h>

namespace ISA
{
/**
 * Class for interfacing to 3DISA session management. Intended for use by all services
 * requiring information regarding a session, or logging activity to active session.
 *
 * TODO: Need to resolve if this is needed. Perhaps the Session class itself can load and save
 * itself to a database. Would only need a session manager service to instantiate the session.
 */
class SessionManager
{
public:
   ~SessionManager();

   static std::shared_ptr<Session> newSession();

   static std::shared_ptr<Session> getSession(const std::string& sessionId);

   static void saveSession(std::shared_ptr<Session> session);

   static std::map< std::string, std::shared_ptr<Session> >& getSessionList();

private:
   SessionManager();
   static std::map< std::string, std::shared_ptr<Session> > m_activeSessions;
};

} // end namespace ISA
#endif // SessionManager_HEADER
