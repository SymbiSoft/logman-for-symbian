/**
    Defines utility functions for LogManGui
*/
#ifndef __LOGMANGUI_UTILS_H__
#define __LOGMANGUI_UTILS_H__

/** Utility function for querying data from user
    @param aResourceId  Resource id of the query dialog.
    @param aQuery       Text of the query.
    @param aResponse    Contains user's response.
*/
template <class Class, class TResponse>
TInt GenericQueryL( TInt aResourceId, const TDesC& aQuery, TResponse& aResponse);

/// Utility function for querying text from user
TInt GenericTextQueryL(const TDesC& aQuery, TDes& aResponse);
/// Utility function for querying numeric data from user
TInt GenericNumberQueryL(const TDesC& aQuery, TInt& aResponse);

_LIT( KEmptyStr, "");

#endif
