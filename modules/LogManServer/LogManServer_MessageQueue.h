#ifndef __LOGGINGSERVERMESSAGEQUEUE_H__
#define __LOGGINGSERVERMESSAGEQUEUE_H__

#include <e32base.h>

#include "LogManServer_Defines.h"
class CLogManServer;
class CDesC8Array;

/// Message queue for asynchronous logging
/** Holds a list of messages and sends them asynchronously
  * from the client's(RLogMan) perspective.
 */
class CLoggingServerMessageQueue : public CActive
  {

  public:
    /** Constructor */
	CLoggingServerMessageQueue(CLogManServer* iLoggingServerServer);
	/** Destructor */
	~CLoggingServerMessageQueue();

	/** Second phase constructor */
	void ConstructL();

    /** Add message to Queue
      * @param aMessage Message to add
      */
    void AppendL( TDesC8& aMessage);

	/** Starts sending messages in Queue */
	void Start();

  protected:
    /** Message Queue */
    CDesC8Array* iMessages;

    /** Called by system when cancelling Active Object */
	void DoCancel();

	/** Sends the messages */
	void RunL();

	/** Handle to the LogMan service holding serial connection */
	CLogManServer *iLoggingServerServer;

  };
#endif


