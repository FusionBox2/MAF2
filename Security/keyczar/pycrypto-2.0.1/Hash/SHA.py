
# Just use the SHA module from the Python standard library

__revision__ = "$Id: SHA.py,v 1.1.2.1 2008-10-29 12:58:32 ior01 Exp $"

from sha import *
import sha
if hasattr(sha, 'digestsize'):
    digest_size = digestsize
    del digestsize
del sha
