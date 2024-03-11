import sys
import os

def scan_directory(theDirectory = ".", scanSub=True, ext=".py", key="Test", haveKey=True, withPath=True, withExt=True):
    """ This function scan the given directory for files.
            ext = search for file with extension ext
            scanSub = search into subfolders
            Key = key of search for filename
            haevKey = True search for file name WITH the given key
                      False search for file name WITHOUT the given key
            withPath = return the filename with the full path """
            
    filenames = []
    
    for filename in os.listdir(theDirectory):

        (shortname, extension) = os.path.splitext(filename)            

        #look for python modules
        if extension == ext:

            if filename.count(key) and haveKey or not filename.count(key) and not haveKey:

                if not withExt:
                    filename = shortname
                
                if withPath:
                    s = os.path.join(theDirectory,filename)
                else:
                    s = filename

                filenames.append(s)
     
                
        elif extension == '' and scanSub:
            
                subdir = os.path.join(theDirectory,filename)

                if os.path.isdir(subdir):
                    sublines = scan_directory(subdir, scanSub, ext, key, haveKey, withPath)
                    for s in sublines:
                        filenames.append(s)
    return filenames


if __name__ == "__main__":

    print "A simple example: all Python module into current directory folder tree with full path"

    example = scan_directory(".",True, ".py","",True,True,True)

    for f in example:
        print f

    
    
