
#include <e32debug.h>
#include <badesca.h>

#include "LogManServer_MessageQueue.h"
#include "LogManServer_Server.h"

#include "loggingdefs.h"

CLogManMessageQueue::CLogManMessageQueue(CLogManServer* aLogManServer)
        : CActive(0),iLogManServer(aLogManServer),iMessages(NULL)
{

}

void CLogManMessageQueue::ConstructL()
{
    CActiveScheduler::Add(this);
    iMessages = new (ELeave) CDesC8ArrayFlat(10);
}

void CLogManMessageQueue::AppendL( TDesC8& aMessage) {
    iMessages->AppendL( aMessage );
}

void CLogManMessageQueue::Start()
{
    PRINTF( "void CLoggingServerMessageQueue::Start()" )

    TRequestStatus* status = &iStatus;
    User::RequestComplete( status, KErrNone );
    SetActive();
}

void CLogManMessageQueue::DoCancel() {

}

void CLogManMessageQueue::RunL()
{
    if (!iLogManServer->IsClosing())
    {
        for ( TInt i = 0; i < iMessages->MdcaCount(); i++)
        {

            const TPtrC8& data = iMessages->MdcaPoint(0);
            TMessageBuffer8 buffer; buffer.Copy( data );

            this->iLogManServer->SendMessage( buffer );

            // Remove from Queue
            iMessages->Delete(0);
        }
    }
}

CLogManMessageQueue::~CLogManMessageQueue()
{
    Cancel();
    // Clear message array
    if (iMessages )
    {
        iMessages->Reset();
        delete iMessages;
    }
}

