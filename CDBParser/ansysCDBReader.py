#-----------------------------------------------------------------------------
# BEWARE!!! This is mostly a prototype!!!
# code is changing very fast so don't rely on it :P
# author: Daniele Giunchi <d.giunchi@scsolutions.it>
#-----------------------------------------------------------------------------

# creating template for general rader
# this one is using regular expressions, grammars and exception handling to perform parsing
# code is quite clean but I guess it could be improved by using a finite state machine
# experimenting...

from Debug import Debug
import progressBar
import os
import sys, string
import math

from cStringIO import StringIO

import unittest
import shutil
import re
from pyparsing import Word, alphas, nums, ZeroOrMore, ParseException, Group, delimitedList,\
     alphanums, Literal,Dict, Suppress
import pprint
import fileUtilities
import AnsysCDBGrammar

class ansysReader:
    """
    Parse ansys cdb files and extract geometry (points), connectivity (cells) and materials. 
    nodes, elements and materials output files are written in maf fem meshes cdb format  to be
    used by standard maf fem reader components.
    """
    
    def __init__(self):                       
        
        self.CDBAnsysFileName = "No ansys CDB file defined"
        self.CacheFolderName = "AnsysReaderCacheFolder"
        self.NodesOutputFileName = "nodes.lis"
        self.ElementsOutputFileName = "elements.lis"
        self.ComponentsOutputFileName = "components.lis"
        self.MaterialsOutputFileName = "materials.lis"
        self.NodesMatrix = []
        self.ElementsMatrix = []       
        self.MaterialData = []
        self.ComponentsData = []
        self.ProgressBar = progressBar.progressBar()
        self.CurrentMatTemp =  0.0  # undefined
        
    def Read(self):
        return self.Parse()
        
    def Parse(self):
        
        self.ElementsMatrix = []
        
        # create the cache folder directory
        fileUtilities._mkdir(self.CacheFolderName)
        os.chdir(self.CacheFolderName)
          
        diskFile = open(self.CDBAnsysFileName)
        file = StringIO()
        self.FileLinesNumber = 0
        for curr in diskFile.readlines() :
            self.FileLinesNumber += 1
            file.write(curr)
        
        print "Input files contains: " + str(self.FileLinesNumber) + " lines" 
        file.seek(0)
        
        self.ParsedLineNumber = 0
        
        elementTypeSectionsNumber = 0
        materialSectionNumber     = 0
        typeSectionNumber         = 0
        realSectionNumber         = 0
        
        nodeSectionsNumber        = 0
        
        elementsSectionNumber     = 0
        
        componentSectionNumbers   = 0
        
        materialsMPDATASectionsNumber = 0
        materialsMPTEMPSectionsNumber = 0
        
        # file first-line
        line = file.readline() 
        self.ParsedLineNumber += 1
        
        # parse the file structure and gather informations

        while 1:
            
            if Debug:
                print "main loop is parsing: " + str(self.ParsedLineNumber) + " > " + line

            # # Elements type section
            # find the elements type section
            if re.search("^(ET,)", line):
                print "parsing Elements section...", line
                if Debug:
                    print "Found elements type section at line " + str(self.ParsedLineNumber)
                elementTypeSectionsNumber += 1
                # self.ReadElementsType()  
            
            # # Materials type section
            # find the material type section
            if re.search("^(MAT\s,)", line):
                print "parsing Material Type section...",line
                if Debug:
                    print "Found material type section at line " + str(self.ParsedLineNumber)
                materialSectionNumber += 1
                # self.ReadElementsType()
            
            # # Type section
            # find the  type section
            if re.search("^(TYPE,)", line):
                print "parsing type section...", line
                if Debug:
                    print "Found  type section at line " + str(self.ParsedLineNumber)
                typeSectionNumber += 1
                # self.ReadElementsType()
                
            # # Real section
            # find the real section
            if re.search("^(REAL,)", line):
                print "parsing type section...", line
                if Debug:
                    print "Found real section at line " + str(self.ParsedLineNumber)
                realSectionNumber += 1
                # self.ReadElementsType()       
            
            # # NODES section
            # Find the NODES section
            if re.search("^(NBLOCK,)", line):
                print "parsing Nodes section...", line
                if Debug:
                    print "Found NODES section at line " + str(self.ParsedLineNumber)                
                nodeSectionsNumber += 1
                
                # create the NODES NodesMatrix
                
                cargo = file, line
                cargo = self.ReadNodes(cargo)
                file, line = cargo
                continue
                
                
            # # Elements declaration section
            # find the elements declaration section
            if re.search("^(EBLOCK,)", line):
                print "Found elements declaration at line " + str(self.ParsedLineNumber), line
                elementsSectionNumber += 1
                cargo = file, line
                cargo = self.ReadElements(cargo)
                file, line = cargo
                continue
                
            
            # # Component  section
            # find the  component section
            if re.search("^(CMBLOCK,)", line):
                print "Found component at line " + str(self.ParsedLineNumber), line
                componentSectionNumbers += 1
                cargo = file, line
                cargo = self.ReadComponents(cargo)
                file, line = cargo
                continue
              
            # Materials section MPTEMP format
            """if re.search("^(MPTEMP,)", line):
                # print "parsing MPTEMP materials section..."
                print "Found material temp section at line " + str(self.ParsedLineNumber), line
                materialsMPTEMPSectionsNumber += 1
                
                # create the materials NodesMatrix
                cargo = file, line
                #cargo = self.ReadMaterialsMPTEMP(cargo)
                file, line = cargo"""
           
            # Materials section: MPDATA format
            if re.search("^(MPDATA,)", line):
                # print "parsing MPDATA materials section..."
                print "Found materials section at line " + str(self.ParsedLineNumber), line
                materialsMPDATASectionsNumber += 1
                
                # create the materials NodesMatrix
                cargo = file, line
                cargo = self.ReadMaterialsMPDATA(cargo)
            
           
            # exit when end of file is found
            if not line: break
                
            # read next line
            line = file.readline() 
            self.ParsedLineNumber += 1
            self.PrintProgress()
            
            
        if Debug:
                
            print "printing NodesMatrix..."
            for raw in self.NodesMatrix:
                print raw
                
            print "printing ElementsMatrix..."
            for raw in self.ElementsMatrix:
                print raw
                
            print "printing ComponentsData..."
            for raw in self.ComponentsData:
                print raw
           
            print "materials Data..."
            for raw in self.MaterialData:
                print raw
            
        print "Creating nodes cache file..."
        self.StoreNodesToFile()       
        
        print "Creating elements cache file..."
        self.StoreElementsToFile()   
        
        print "Creating components cache file..."
        self.StoreComponentsToFile()          
        
        print "Creating materials cache file..."
        self.StoreMaterialsToFile()       
        
        print "nodeSectionsNumber " + str(nodeSectionsNumber)
        print "elementsSectionNumber " + str(elementsSectionNumber)
        print "componentSectionNumbers " + str(componentSectionNumbers)
        print "materialsMPDATASectionsNumber " + str(materialsMPDATASectionsNumber)
        
        return nodeSectionsNumber, elementsSectionNumber, componentSectionNumbers, \
               materialsMPDATASectionsNumber, 
    
    
        
    def StoreNodesToFile(self):
        """Store data to disk cache"""
        memFile = StringIO()
        for line in self.NodesMatrix:
            # print line
            # file.writelines
            for column in line:
                # print str(column)  + "  "
                memFile.write(str(column)  + '\t\t')
            memFile.write('\n')
        
        if Debug:
            print memFile.getvalue()
            
        diskFile = open(self.NodesOutputFileName, 'w')
        diskFile.write(memFile.getvalue())
        diskFile.close()
        print "Written cache node file: "  + self.NodesOutputFileName + " in directory: "  + self.CacheFolderName
    
    def StoreElementsToFile(self):
        """Store data to disk cache"""
        file = StringIO()
        for line in self.ElementsMatrix:
            # print line
            # file.writelines
            for column in line:
                # print str(column)  + "  "
                file.write(str(column)  + '\t\t')
            file.write('\n')
        
        if Debug:
            print file.getvalue()
        
        diskFile = open(self.ElementsOutputFileName, 'w')
        diskFile.write(file.getvalue())
        diskFile.close()
        print "Written cache elements file: "  + self.ElementsOutputFileName+ " in directory: "  + self.CacheFolderName
        
    def StoreComponentsToFile(self):
        """Store data to disk cache"""
        file = StringIO()
        for line in self.ComponentsData:
            # print line
            # file.writelines
            for column in line:
                # print str(column)  + "  "
                file.write(str(column)  + '\t\t')
            file.write('\n')
        
        if Debug:
            print file.getvalue()
        
        diskFile = open(self.ComponentsOutputFileName, 'w')
        diskFile.write(file.getvalue())
        diskFile.close()
        print "Written cache elements file: "  + self.ComponentsOutputFileName+ " in directory: "  + self.CacheFolderName
        
    def StoreMaterialsToFile(self):
        #MPDATA,R5.0, 1,EX  ,       2, 1,  1000.00000    ,
        #MPDATA,R5.0, 1,NUXY,       2, 1, 0.300000000    
        
        #   -> Generates ->   

        #MATERIAL NUMBER =      2 EVALUATED AT TEMPERATURE OF   0.0000    
        #EX   =   1000.0    
        #NUXY =  0.30000
        
        
        """Store data to disk cache"""
        # ['EX', '2', '1000.0', 'NUXY', '2', '0.3']
        file = StringIO()
        #print self.MaterialData
        for material in self.MaterialData:
                
                toWrite = "MATERIAL NUMBER =      %s EVALUATED AT TEMPERATURE OF   %s    " 
                args = (str(material[2]),str(material[0]))
                file.write(toWrite % args)
                file.write('\n')
                
                toWrite = "%s = %s" 
                args = (str(material[1]),str(material[3]))
                file.write(toWrite % args)
                file.write('\n')
                
                materialComponents = len(material)
                i = 4
                
                if Debug:
                    print materialComponents
                while i < (materialComponents - 1):
                    args = (str(material[i]),str(material[i + 2]))
                    file.write(toWrite % args)
                    file.write('\n')
                    i += 3
                                                                        
                # 1 3 4 6
                file.write('\n')
        
        if Debug:
            print file.getvalue()
                
        diskFile = open(self.MaterialsOutputFileName, 'w')
        diskFile.write(file.getvalue())
        diskFile.close()
        print "Written cache materials file: "  + self.MaterialsOutputFileName + " in directory: "  + self.CacheFolderName
        
    def ReadNodes(self, cargo):
        """Generate Nodes NodesMatrix"""
        #f = open("C:\\TestFileAnsys.txt", 'w')
        self.PrintProgress()
        file, line = cargo
        #Starting node section        
        list = AnsysCDBGrammar.nodeStartLine.parseString( line )
        colNumbers = list[1]
        
        line = file.readline()
        self.ParsedLineNumber += 1
        #ignore second line
        
        line = file.readline()
        self.ParsedLineNumber += 1
        
        #Core node section  
        while 1:
            
            if Debug:
                print " > " + line
                
            self.PrintProgress()
    
            try: 
                list = AnsysCDBGrammar.nodeTextLine3.parseString( line )
                if Debug:
                    print list[1:]
                    
                """finalList = list[:1]
                finalList.extend(list[2:])"""
                
                self.NodesMatrix.append(list)
                #f.writelines(" ".join(list))
                #f.writelines("\n")

            except ParseException:
                try: 
                    list = AnsysCDBGrammar.nodeTextLine2.parseString( line )
                    if Debug:
                        print list[1:]
                        
                    #value must be put for Y and Z
                    finalList = [list[0]]
                    triple = [list[1],list[2],"0.00000000"]
                    finalList.extend(triple)
                    
                    self.NodesMatrix.append(finalList)
                    #f.writelines(" ".join(finalList))
                    #f.writelines("\n")

                except ParseException:
                    try: 
                        list = AnsysCDBGrammar.nodeTextLine1.parseString( line )
                        if Debug:
                            print list[1:]
                        
                        #value must be put for Y and Z
                        finalList = [list[0]]
                        triple = [list[1],"0.00000000","0.00000000"]
                        finalList.extend(triple)
                    
                        self.NodesMatrix.append(finalList)
                        #f.writelines(" ".join(finalList))
                        #f.writelines("\n")

                    except ParseException:
                        if Debug:
                            print "cannot parse line: " + str(line) + "...exiting..."
                        break
            
            line = file.readline()
            self.ParsedLineNumber += 1
            self.PrintProgress()

        #f.close()
        if Debug:
            print "printing NodesMatrix..."
            for raw in self.NodesMatrix:
                print raw
            print "ReadNodes is returning: " + line 
                
        return file, line
        
    
    def ReadElements(self, cargo):
        """Generate Elements NodesMatrix"""
        file, line = cargo
        self.PrintProgress()
        #Starting element section        
        list = AnsysCDBGrammar.elementStartLine.parseString( line )
        fieldsNumber = list[1]
        
        line = file.readline()
        self.ParsedLineNumber += 1
        #ignore second line
        
        line = file.readline()
        self.ParsedLineNumber += 1
        #core  and end section
        
        
        while 1:
            if Debug:
                print "Processing line: " + line 
            element = []
            try: 
                list = AnsysCDBGrammar.elementDeclaration.parseString( line )
                idMaterial = int(list[0])
                
                if(idMaterial == -1): #end of EBLOCK
                    break                
                
                idTypeElement = int(list[1])
                idConstants = int(list[2])
                numberOfNodesInElement = int(list[8])
                numberOfElement = int(list[10])
                connectivityList = list[11:];
                
                if(numberOfNodesInElement > 8):
                    line = file.readline()
                    self.ParsedLineNumber += 1
                    list = AnsysCDBGrammar.elementDeclaration.parseString( line )
                    connectivityList.extend(list)
                
                element.append(numberOfElement)
                element.append(idMaterial)
                element.append(idTypeElement)
                element.append(idConstants)
                element.append(0)
                element.append(1)
                element.extend(connectivityList)
                
                self.ElementsMatrix.append(element)
                
                if Debug:
                    print list
                
                line = file.readline()
                self.ParsedLineNumber += 1
                self.PrintProgress()
                    

            except ParseException:
                if Debug:
                    print "cannot parse as TYPE line: " + line
     #            break
        return file, line
        
        
        
        """TYPE = []
        ESYS = []
        EN = []
        EMORE = []
        
        
            
        # outputNodeMatrixFileName = r'D:\vapps\LHPBuilder_Parabuild\ASCIIParser\testData\ansys\outputNodeMatrix.txt'
        # f = open(outputNodeMatrixFileName, 'w')

        if Debug:            
            print "ReadElements Section:"
        
        while 1:
             
            
            if Debug:
                print "Processing line: " + line 

            idTYPE = 0
            idMAT = 1
            idREAL = 2
            
            tmpLine = []            
            TYPELine = []
            
            try: 
                TYPE = AnsysGrammar.typeMatReal.parseString( line )

                # Modify order to match ansys one
                TYPELine.append(TYPE[idMAT])
                TYPELine.append(TYPE[idTYPE])
                TYPELine.append(TYPE[idREAL])
                
                if Debug:
                    print TYPELine
                    

            except ParseException:
                if Debug:
                    print "cannot parse as TYPE line: " + line
     #            break
              
            # 
            line = file.readline()
            self.ParsedLineNumber += 1
            self.PrintProgress()
            
            #if Debug:
                #print " Processing line: " + line 
                 
            #try: 
                #ESYS = AnsysGrammar.esysLine.parseString(line)
                #raw.append(ESYS)
            #except ParseException:
                #if Debug:
                    #print "cannot parse line: " + line
       #          break
       
            
            while 1:
               
                # parse EN line
                if Debug:
                    print " Processing line: " + line 
                
                try: 
                    EN = AnsysGrammar.connectivityLine.parseString(line)
                    if Debug:
                        print "EN Line found!"
                    break
                
                except ParseException:
                    if Debug:
                        print "cannot recognize line as EN... continuing...: "  + str(self.FileLinesNumber) + line

                line = file.readline()
                
                if Debug:
                    print "now trying: " + line
                self.ParsedLineNumber += 1
                self.PrintProgress()

            while 1:
                  
                # parse EN line
                if Debug:
                    print " Processing line: " + line 
                
                try: 
                    EN = AnsysGrammar.connectivityLine.parseString(line)
                    tmpLine.append(EN)
                except ParseException:
                    if Debug:
                        print "cannot parse line as EN: " + line
                    break
                
                # parse EMORE line
                line = file.readline()
                self.ParsedLineNumber += 1                
                self.PrintProgress()
                
                if Debug:
                    print " Processing line: " + line 
                
                try: 
                    EMORE = AnsysGrammar.moreConnectivityLine.parseString( line )
                    tmpLine.append(EMORE)
                except ParseException:
                    if Debug:
                        print "cannot parse line as EMORE: " + line
                    break
                
                # 
                line = file.readline()
                self.ParsedLineNumber += 1
                self.PrintProgress()
                
                if Debug:
                    print " Processing line: " + line 
            
                # simple example from the unit test sample data...
                #TYPE, 2  $ MAT, 2  $ REAL, 3
                #ESYS, 0
                #EN,       2,       3,       1,       2,       4,       7,       6,       9,      12
                #EMORE,       8,      10
                
                
                # more complex example...
                #TYPE, 6  $ MAT, 6  $ REAL, 6
                #ESYS, 0
                #EN,      18,    3199,   11050,   11325,   11888,   84747,   34788,   84746,   84744
                #EMORE,   34786,   33193
                #EN,      19,   10517,   11053,   11049,    3241,   38238,   34791,   38239,   84508
                #EMORE,   84506,   84507
                #EN,      20,   13087,    3133,   12813,   13266,   85133,   85134,   25688,   24645
                #EMORE,   85132,   25687
                #EN,      21,   11515,   11246,    3657,    3681,   33590,   82150,   82149,   82031
                #EMORE,   82032,   82152
                #EN,      22,   12812,    3656,    3657,   11246,   82155,   82161,   82148,   33588
                #EMORE,   82156,   82150
    
                element = []
                
                element.append(EN[0])
                for component in TYPELine:
                    element.append(component)
                
                # element.append(ESYS[0])
                # this is always zero
                element.append('0')
                
                element.append('1')
    
                for component in EN[1:]:
                    element.append(component)
    
                for component in EMORE:
                    element.append(component)
                
                # print "component: " + str("  ".join(component))
                self.ElementsMatrix.append(element)
            
            break
        # f.close()"""
        if Debug:
             print "returning line: " + line
        return file, line

    def ReadComponents(self, cargo):
        file, line = cargo
        
        self.PrintProgress()
        
        #Starting component section        
        list = AnsysCDBGrammar.componentStartLine.parseString( line )

        component = []
        componentName = str(list[1])
        componentType = str(list[2])
        component.append(componentName)
        component.append(componentType)
  
        line = file.readline()
        self.ParsedLineNumber += 1
        #ignore second line
   
        line = file.readline()
        self.ParsedLineNumber += 1
        
        #core and end section
        idPrevious = 0
        while 1:
            if Debug:
                print "Processing line: " + line             
            itemList = []
            try: 
                list = AnsysCDBGrammar.componentDeclaration.parseString( line )
                for item in list:
                    if(int(item) < 0):
                        for x in range(idPrevious+1, abs(int(item))+1):
                            component.append(str(x))
                    else:
                        component.append(item)
                    idPrevious = abs(int(item))
                
                
                if Debug:
                    print list
                
                line = file.readline()
                self.ParsedLineNumber += 1
                
                
            except ParseException:
                if Debug:
                    print "cannot parse as COMPONENT line: " + line
                break
            
        self.ComponentsData.append(component);    
        self.PrintProgress()
        return file, line
        
    def ReadMaterialsMPDATA(self,cargo):
        """Generate materials text"""
        self.PrintProgress()
        
        materialList = []
        #MPDATA,R5.0, 1,EX  ,       2, 1,  1000.00000    ,
        #MPDATA,R5.0, 1,NUXY,       2, 1, 0.300000000    
                
        matNumber = ""
        matTemp = ""
        matName = ""
        matValue = ""

        file, line = cargo
        
        materialList.append(self.CurrentMatTemp)
        
        #MPDATA,R5.0, 1,EX  ,       2, 1,  1000.00000    ,
        #MPDATA,R5.0, 1,NUXY,       2, 1, 0.300000000    
        
        # 
        #['MPDATA,', 'EX', '2', '1', '1000.0']
        #['MPDATA,', 'NUXY', '2', '1', '0.3']

        
        while 1:

            
            try: 
                MPDATA = AnsysCDBGrammar.matData.parseString( line )
                matName = MPDATA[3]
                matNumber = MPDATA[4]
                matValue = MPDATA[6]
                materialList.append(matName)
                materialList.append(matNumber)
                materialList.append(matValue)
                
                
            except ParseException:
                if Debug:
                    print "cannot parse line " + str(line)
                break
        
            line = file.readline()
            self.ParsedLineNumber += 1
            self.PrintProgress()
        
        
        if Debug:
            print materialList
            
        #before adding, it control if material already exist
        materialExist = False
        for matL in self.MaterialData:
            if(matL[2] == materialList[2]):
                materialExist = True
                matL.extend(materialList[1:])
                    
        if(materialExist == False):
            self.MaterialData.append(materialList)
            
        return file, line

    
    
    def ReadMaterialsMPTEMP(self,cargo):
        pass

    def PrintProgress(self):
        progress = (int((self.ParsedLineNumber * 100 / self.FileLinesNumber)))
        self.ProgressBar.updateAmount(progress)
        print self.ProgressBar, '\r'
        

def Run(CDBAnsysFileName, cacheFolderName , nodesOutputFileName , elementsOutputFileName , materialsOutputFileName ):                                            
    reader = ansysReader()
    reader.CDBAnsysFileName = CDBAnsysFileName
    reader.CacheFolderName = cacheFolderName
    reader.NodesOutputFileName = nodesOutputFileName
    reader.ElementsOutputFileName = elementsOutputFileName
    reader.MaterialsOutputFileName = materialsOutputFileName
    reader.Read()    

def main():
    args = sys.argv[1:]
    
    if Debug:
    
        fileName = r'ansysReaderDebugLog.txt'
        print fileName
        f = open(fileName, 'w')
        for arg in args:
             f.write(str(arg))
             f.write('\n')
        f.close()

    if len(args) != 5:
        print """
        usage: python.exe ansysReader.py
        CDBAnsysFileName 
        cacheFolderName
        nodesOutputFileName
        elementsOutputFileName
        materialsOutputFileName
        """
        sys.exit(-1)
    Run(args[0],args[1],args[2],args[3],args[4])

if __name__ == '__main__':
    main()
    