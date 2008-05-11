#ifndef __LoggingServerGui_PAN__
#define __LoggingServerGui_PAN__

/** LoggingServerGui application panic codes */
enum TLoggingServerGuiPanics
    {
    ELoggingServerGuiUi = 1
    };

inline void Panic(TLoggingServerGuiPanics aReason)
    {
    _LIT(applicationName,"LogManGui");
    User::Panic(applicationName, aReason);
    }

#endif // __LoggingServerGui_PAN__
