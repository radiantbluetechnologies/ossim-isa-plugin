//**************************************************************************************************
//
//     OSSIM Open Source Geospatial Data Processing Library
//     See top level LICENSE.txt file for license information
//
//**************************************************************************************************
#include <isa/common/SessionManager.h>
#include <ossim/base/ossimCommon.h>

using namespace std;
namespace ISA
{
std::map< std::string, shared_ptr<Session> > SessionManager::m_activeSessions;

SessionManager::SessionManager()
{
}

SessionManager::~SessionManager()
{

   m_activeSessions.clear();
}

shared_ptr<Session> SessionManager::newSession()
{
   shared_ptr<Session> session (new Session);
   m_activeSessions.emplace(session->getSessionId(), session);

   return session;
}

shared_ptr<Session> SessionManager::getSession(const std::string& sessionId)
{
   map< string, shared_ptr<Session> >::iterator result = m_activeSessions.find(sessionId);
   if (result == m_activeSessions.end())
   {
      // Need to search Db and load:
      // TODO: Implement
      clog<<__FILE__<<":"<<__LINE__<<" Method not fully implemented!"<<ends;
      return nullptr;
   }
   return result->second;
}

void SessionManager::saveSession(const shared_ptr<Session> session)
{
   // TODO: Implement
   clog<<__FILE__<<":"<<__LINE__<<" Method not yet implemented!"<<ends;
}

std::map< string, shared_ptr<Session> >& SessionManager::getSessionList()
{
   return m_activeSessions;
}
}
