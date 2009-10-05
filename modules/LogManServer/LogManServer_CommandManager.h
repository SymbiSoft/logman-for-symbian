#ifndef __LOGGINGSERVER_COMMANDMANAGER_H__
#define __LOGGINGSERVER_COMMANDMANAGER_H__

#include <e32base.h>

#include "LogManServer_Defines.h"
#include "LogManServer_SocketEngineObserver.h"

class CLogManServer;
class CDesC8Array;
class RFs;
/// Handles commands sent from PC through serial connection
/**
 *
 */
class CLogManCommandManager: public CActive,
		public MSocketEngineObserver
{

public:
	/** Constructor */
	CLogManCommandManager(CLogManServer* iLoggingServerServer);
	/** Destructor */
	~CLogManCommandManager();

	/** Second phase constructor */
	void ConstructL();

	void Start();

	/**
	 * SocketState
	 * @param aNewState The new state of the socket engine.
	 */
	void SocketState(TInt aNewState);

	/**
	 * SocketData
	 * @param aData The data from socket.
	 */
	void SocketData(TDesC8& aData);

protected:	
	TInt HandleCmdCopyFilesL(RArray<RBuf8>& parameters, RFs& aFs);
	TInt HandleCmdDelL(RArray<RBuf8>& parameters, RFs& aFs);
	TInt HandleCmdExecL(RArray<RBuf8>& aParameters, RFs& aFs);
	TInt HandleCmdGetL(RArray<RBuf8>& aParameters, RFs& aFs);
	TInt HandleCmdKillAndFindL(RArray<RBuf8>& aParameters, TBool aDoKill);
	TInt HandleCmdListL();
	TInt HandleCmdLsL(RArray<RBuf8>& aPath, RFs& aFs);			
	TInt HandleCmdPutL(RArray<RBuf8>& aParameters, RFs& aFs);	

	/** Handle shell commands */
	TInt HandleCommand();
	/** Handle shell commands
	 * @param aArgs Reference to array used for storing command arguments. Is not closed.
	 */
	void HandleCommandL(RArray<RBuf8>& aArgs);

	/** Called by system when cancelling Active Object */
	void DoCancel();

	/** Read serial line and execute command */
	void RunL();

	/** Send error message back and leave on error */
	void LeaveIfFailedL(TInt aErr);

	/** Handle to the LogMan service holding serial connection */
	CLogManServer *iLogManServer;
	RTimer iTimer;
	RBuf8 iCommandBuffer;
	friend class CLogManServer;
};
#endif

