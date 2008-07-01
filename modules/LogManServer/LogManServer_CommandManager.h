#ifndef __LOGGINGSERVER_COMMANDMANAGER_H__
#define __LOGGINGSERVER_COMMANDMANAGER_H__

#include <e32base.h>

#include "LogManServer_Defines.h"
class CLoggingServerServer;
class CDesC8Array;

/// Handles commands sent from PC through serial connection
/**
  *
 */
class CLoggingServerCommandManager : public CActive
  {

  public:
    /** Constructor */
	CLoggingServerCommandManager(CLoggingServerServer* iLoggingServerServer);
	/** Destructor */
	~CLoggingServerCommandManager();

	/** Second phase constructor */
	void ConstructL();

  protected:

    /** Called by system when cancelling Active Object */
	void DoCancel();

	/** Read serial line and execute command */
	void RunL();

	/** Handle to the LogMan service holding serial connection */
	CLoggingServerServer *iLoggingServerServer;
    friend class CLoggingServerServer;
  };
#endif


