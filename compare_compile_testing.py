#mwccsym2.exe -g -O0 -inline off -wchar_t off -align 4 -warnings on -w nohidevirtual,nounusedexpr -msgstyle gcc -enum int -str pool -exc ms -trigraphs on  -nostdinc -d _DEBUG -d _UNICODE -d "__SYMBIAN32__" -d "__SERIES60_30__" -d "__SERIES60_3X__" -d "__CW32__" -d "__WINS__" -d "__WINSCW__" -d "__EXE__" -d "__DEBUG_LOGGING__" -d "__SUPPORT_CPP_EXCEPTIONS__"  -cwd source -i- -i "\Projects\LoggingServer\LoggingServer\inc" -i "\Projects\LoggingServer\LoggingServer\Common" -i "\EPOC32\include" -i "\epoc32\include\variant" -i "\epoc32\include\variant\ " -include "Symbian_OS_v9.1.hrh" -o "\EPOC32\BUILD\Projects\LoggingServer\LoggingServer\group\LOGGINGSERVER\WINSCW\UDEB\LogMan_UID_.o" -c "\EPOC32\BUILD\Projects\LoggingServer\LoggingServer\group\LOGGINGSERVER\WINSCW\LogMan.UID.CPP"

#mwldsym2.exe -msgstyle gcc -stdlib "\EPOC32\RELEASE\WINSCW\UDEB\EEXE.LIB" -m "?_E32Bootstrap@@YGXXZ" -subsystem windows -g \EPOC32\RELEASE\WINSCW\UDEB\euser.lib \EPOC32\RELEASE\WINSCW\UDEB\efsrv.lib \EPOC32\RELEASE\WINSCW\UDEB\c32.lib \EPOC32\RELEASE\WINSCW\UDEB\BAFL.lib -o "\EPOC32\RELEASE\WINSCW\UDEB\LogMan.exe" -noimplib -l \EPOC32\BUILD\Projects\LoggingServer\LoggingServer\group\LOGGINGSERVER\WINSCW\UDEB -search LoggingServerServer.o LoggingServerStartupCode.o LoggingServerSession.o LoggingServerMessageQueue.o LogMan_UID_.o


####SCONS

abld_cc = r'mwccsym2.exe -g -O0 -inline off -wchar_t off -align 4 -warnings on -w nohidevirtual,nounusedexpr -msgstyle gcc -enum int -str pool -exc ms -trigraphs on  -nostdinc -d _DEBUG -d _UNICODE -d "__SYMBIAN32__" -d "__SERIES60_30__" -d "__SERIES60_3X__" -d "__CW32__" -d "__WINS__" -d "__WINSCW__" -d "__EXE__" -d "__DEBUG_LOGGING__" -d "__SUPPORT_CPP_EXCEPTIONS__"  -cwd source -i- -i "\Projects\LoggingServer\LoggingServer\inc" -i "\Projects\LoggingServer\LoggingServer\Common" -i "\EPOC32\include" -i "\epoc32\include\variant" -i "\epoc32\include\variant\ " -include "Symbian_OS_v9.1.hrh" -o "\EPOC32\BUILD\Projects\LoggingServer\LoggingServer\group\LOGGINGSERVER\WINSCW\UDEB\LoggingServerSession.o" -c "\Projects\LoggingServer\LoggingServer\src\LoggingServerSession.cpp"' 
abld_cc = abld_cc.split()


scons_cc = r'mwccsym2.exe -o WINSCW\LogMan.UID.o -c -g -O0 -inline off -wchar_t off -align 4 -warnings on -w nohidevirtual,nounusedexpr -msgstyle gcc -enum int -str pool -exc ms -trigraphs on -nostdinc -cwd source -i- -include "Symbian_OS_v9.1.hrh" -d "__SYMBIAN32__" -d "_UNICODE" -d "__SERIES60_30__" -d "__SERIES60_3X__" -d "__SUPPORT_CPP_EXCEPTIONS__" -d "_DEBUG" -d "__CW32__" -d "__WINS__" -d "__WINSCW__" -d "__EXE__" -i J:\Epoc32\include -i J:\Epoc32\include\variant -i LoggingServer\inc -i LoggingServer\client -i LoggingServer\Common WINSCW\LogMan.UID.cpp'
scons_cc = scons_cc.split() 

scons_cmd = r'mwldsym2.exe -o LogMan.EXE -msgstyle gcc -stdlib "J:\EPOC32\RELEASE\WINSCW\UDEB\EEXE.LIB" -m "?_E32Bootstrap@@YGXXZ" -subsystem windows -g -noimplib LoggingServer\src\LoggingServerServer.o LoggingServer\src\LoggingServerSession.o LoggingServer\src\LoggingServerStartupCode.o LoggingServer\src\LoggingServerMessageQueue.o WINSCW\LogMan.UID.o J:\EPOC32\RELEASE\WINSCW\UDEB\EUSER.LIB J:\EPOC32\RELEASE\WINSCW\UDEB\EFSRV.LIB J:\EPOC32\RELEASE\WINSCW\UDEB\C32.LIB J:\EPOC32\RELEASE\WINSCW\UDEB\BAFL.LIB'
scons_cmd = scons_cmd.split()

abld_cmd  = r'mwldsym2.exe -msgstyle gcc -stdlib "\EPOC32\RELEASE\WINSCW\UDEB\EEXE.LIB" -m "?_E32Bootstrap@@YGXXZ" -subsystem windows -g \EPOC32\RELEASE\WINSCW\UDEB\euser.lib \EPOC32\RELEASE\WINSCW\UDEB\efsrv.lib \EPOC32\RELEASE\WINSCW\UDEB\c32.lib \EPOC32\RELEASE\WINSCW\UDEB\BAFL.lib -o "\EPOC32\RELEASE\WINSCW\UDEB\LogMan.exe" -noimplib -l \EPOC32\BUILD\Projects\LoggingServer\LoggingServer\group\LOGGINGSERVER\WINSCW\UDEB -search LoggingServerServer.o LoggingServerStartupCode.o LoggingServerSession.o LoggingServerMessageQueue.o LogMan_UID_.o'
abld_cmd  = abld_cmd.split()


def test( list1, list2):
    list1 = [ x.replace('"', '').replace("J:", "") for x in list1 ]
    list2 = [ x.replace('"', '').replace("J:", "") for x in list2 ]

    print "-" * 10
    for x in list1:
        if x.strip() == "": continue
        if x not in list2:
            print x
    print "-" * 10


print "Scons mwccsym2 in abld"
test( scons_cc, abld_cc )
print "abld mwccsym2 in scons"
test( abld_cc, scons_cc )


print "Scons mwldsym2 in abld"
test( scons_cmd, abld_cmd )
print "abld mwldsym2 in scons"
test( abld_cmd, scons_cmd )
