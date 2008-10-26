#ifndef MSOCKETSOCKETENGINEOBSERVER_H
#define MSOCKETSOCKETENGINEOBSERVER_H

/**
 * Interface for observing the socket engine.
 */
class MSocketEngineObserver
{
public:

	/**
	 * SocketState
	 * @param aNewState The new state of the socket engine.
	 */
	virtual void SocketState(TInt aNewState) = 0;

	/**
	 * SocketData
	 * @param aData The data from socket.
	 */
	virtual void SocketData(TDesC8& aData) = 0;
};

#endif // MSOCKETSOCKETENGINEOBSERVER_H
