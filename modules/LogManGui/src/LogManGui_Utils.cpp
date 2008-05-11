
#include <e32def.h>
#include <aknquerydialog.h>
#include <LogManGui.rsg>

#include "LogManGui_Utils.h"

template <class Class, class TResponse>
TInt GenericQueryL( TInt aResourceId, const TDesC& aQuery, TResponse& aResponse)
{
    Class* dialog = Class::NewL(aResponse, CAknQueryDialog::ENoTone);
    dialog->PrepareLC(aResourceId); // CodeScanner false warning: low: codereview: neglected to put variable on cleanup stack

    dialog->SetPromptL(aQuery);
    TInt result = dialog->RunLD();

    return result;
}

TInt GenericTextQueryL(const TDesC& aQuery, TDes& aResponse)
{
    return GenericQueryL<CAknTextQueryDialog>( R_GENERIC_TEXT_QUERY_DIALOG, aQuery, aResponse );
}

TInt GenericNumberQueryL(const TDesC& aQuery, TInt& aResponse)
{
    return GenericQueryL<CAknNumberQueryDialog>( R_GENERIC_NUMERIC_QUERY_DIALOG, aQuery, aResponse );
}
