__docformat__ = "epytext"
__author__ = "Jussi Toivola"
__license__ = "BSD"
__copyright__ = "2008 Jussi Toivola"

import time

PKG_TEMPLATE= """
;Languages
&EN

;Localised package name
#{"%(APPNAME)s"},(%(UID)s),%(VERMAJ)s,%(VERMIN)s,%(BUILD)s,TYPE=SISSYSTEM

;Localised Vendor name
%%{"Jussi Toivola"}

;Unique Vendor name
:"JT"

[0x101F7961], 0, 0, 0, {"Series60ProductID"}
"""

VERMAJ = 1
def get_build_number():
    pkg_data = {}
    
    # Verminor is the last 2 digits of the year
    timedata = time.gmtime()
    year = timedata[0]
    year = str( year )[2:]
    
    # build is the current month and day
    month = timedata[1]
    month = str(month).zfill(2)
    day   = timedata[2]
    day   = str(day).zfill(2) 
    build = ( month + day ).zfill(5)
    
    return VERMAJ, year, build
    
def get_pkg_prefix( appname, uid ):
    pkg_data = {}
    vermaj, year, build = get_build_number()
    
    pkg_data["VERMAJ"]  = vermaj
    pkg_data["VERMIN"]  = year
    pkg_data["BUILD"]   = build    
    pkg_data["APPNAME"] = appname
    pkg_data["UID"]     = uid
    
    
    return PKG_TEMPLATE % pkg_data 
    
