
# Just use the MD5 module from the Python standard library

__revision__ = "$Id: MD5.py,v 1.1.2.1 2008-10-29 12:58:32 ior01 Exp $"

from md5 import *

import md5
if hasattr(md5, 'digestsize'):
    digest_size = digestsize
    del digestsize
del md5

