
#include <e32debug.h>
#include <badesca.h>

#include "LogManServer_MessageQueue.h"
#include "LogManServer_Server.h"

#include "loggingdefs.h"

CLoggingServerMessageQueue::CLoggingServerMessageQueue(CLogManServer* aLoggingServerServer)
        : CActive(0),iLoggingServerServer(aLoggingServerServer)
{

}

void CLoggingServerMessageQueue::ConstructL()
{
    CActiveScheduler::Add(this);
    iMessages = new (ELeave) CDesC8ArrayFlat(10);
}

void CLoggingServerMessageQueue::AppendL( TDesC8& aMessage) {
    iMessages->AppendL( aMessage );
}

void CLoggingServerMessageQueue::Start()
{
    PRINTF( "void CLoggingServerMessageQueue::Start()" )

    TRequestStatus* status = &iStatus;
    User::RequestComplete( status, KErrNone );
    SetActive();
}

void CLoggingServerMessageQueue::DoCancel() {

}

void CLoggingServerMessageQueue::RunL()
{
    PRINTF( "void CLoggingServerMessageQueue::RunL()" )

    if (iLoggingServerServer->IsClosing())
    {
        PRINTF("CLoggingServerMessageQueue stopping")
    }
    else
    {
        for ( TInt i = 0; i < iMessages->MdcaCount(); i++)
        {

            const TPtrC8& data = iMessages->MdcaPoint(0);
            TMessageBuffer8 buffer; buffer.Copy( data );

            PRINTF( "this->iLoggingServerServer->SendMessage( buffer );" )
            this->iLoggingServerServer->SendMessage( buffer );

            // Remove from Queue
            PRINTF( "iMessages->Delete(0);" )
            iMessages->Delete(0);
        }
    }
}

CLoggingServerMessageQueue::~CLoggingServerMessageQueue()
{
    Cancel();
    // Clear message array
    if (iMessages )
    {
        iMessages->Reset();
        delete iMessages;
    }
}

