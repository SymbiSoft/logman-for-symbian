#ifndef __LoggingServerSession_h__
#define __LoggingServerSession_h__

#include <e32base.h>
#include "LogManServer_Server.h"
class CLogManServer;

/// LogMan session class
/** Handle client's session with the server */
class CLoggingServerSession : public CSession2
  {
  public:
    /** Construct CLoggingServerSession */
	CLoggingServerSession();
	/** Destroy CLoggingServerSession */
	virtual ~CLoggingServerSession();

	/** First in handling the client's message. Passes to DispatchMessageL
	  * @param aMessage Message to handle.
	*/
	virtual void ServiceL(const RMessage2& aMessage);

    /** Handle client's message
      * @param aMessage Message to handle
     */
	void HandleMessageSendingL(const RMessage2& aMessage);
    /** @return logging server instance */
	CLogManServer& LoggingServer();

    protected:
    /** Pass the message to suitable function based on the message id
      * @param aMessage Message to handle.
    */
	void DispatchMessageL(const RMessage2& aMessage);

	/** Panic the client on error situations.
	  * @param aMessage Client's message used to send panic code.
	  * @param aPanic   ID of the panic.
	*/
	void PanicClient(const RMessage2& aMessage,TInt aPanic) const;

    /** Getter for: Name of the port to open. */
    TPortName PortName( );
    /** Setter for: Name of the port to open.
    *   @param aPortName Name of the port to set
    */
    TInt SetPortName( TPortName& aPortName );

    /** Getter for: Port number to open. */
    TInt Port( );
    /** Setter for: Port number to open.
    *   @param aPort Port to set
    */
    TInt SetPort( TInt aPort );

  };


#endif

