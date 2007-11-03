#-----------------------------------------------------------------------------
# BEWARE!!! This is mostly a prototype!!!
# code is changing very fast so don't rely on it :P
# author: Stefano Perticoni <s.perticoni@scsolutions.it>
#-----------------------------------------------------------------------------

# creating template for general rader
# this one is using regular expressions, grammars and exception handling to perform parsing
# code is very clean but I guess it could be improved by using a finite state machine
# experimenting...

from Debug import Debug
import os
import sys, string, StringIO
import unittest
import shutil
import re
from pyparsing import Word, alphas, nums, ZeroOrMore, ParseException, Group, delimitedList,\
     alphanums, Literal,Dict, Suppress
import pprint
import fileUtilities
            
class ansysReader:
    """"""
    
    def __init__(self):                       
        
        self.InputAnsysFileName = "No ansys input file defined"
        self.CacheFolderName = "AnsysReaderCacheFolder"
        self.NodesOutputFileName = "nodes.lis"
        self.ElementsOutputFileName = "elements.lis"
        self.MaterialsOutputFileName = "materials.lis"
        self.NodesMatrix = []
        self.ElementsMatrix = []       
        self.MaterialData = []
        
    def Read(self):
        return self.Parse()
        
    def Parse(self):
        
        self.ElementsMatrix = []
        
        # create the cache folder directory
        fileUtilities._mkdir(self.CacheFolderName)
        os.chdir(self.CacheFolderName)
          
        diskFile = open(self.InputAnsysFileName)
        file = StringIO.StringIO()
        for curr in diskFile.readlines() :
            file.write(curr)
        file.seek(0)
        
        lineNumber = 0
        
        nodeSectionsNumber = 0
        materialsSectionsNumber = 0
        elementTypeSectionsNumber = 0
        elementDeclarationSectionsNumber = 0
        
        # parse the file structure and gather informations
        while 1:
           
            line = file.readline() 
            if Debug:
                print str(lineNumber) + " > " + line
            cargo = file, line
            
            # # NODES section
            # Find the NODES section
            if re.search("^(N,)", line):
                print "parsing Nodes section..."
                if Debug:
                    print "Found NODES section at line " + str(lineNumber)                
                nodeSectionsNumber += 1
                
                # create the NODES NodesMatrix
                cargo = self.ReadNodes(cargo)
                
            # # Elements type section
            # find the elements type section
            if re.search("^(ET,)", line):
                # print "parsing Elements section..."
                if Debug:
                    print "Found elements type section at line " + str(lineNumber)
                elementTypeSectionsNumber += 1
                # self.ReadElementsType()
                
            # # Elements declaration section
            # find the elements declaration section
            if re.search("^(TYPE,)", line):
                if Debug:
                    print "Found elements declaration at line " + str(lineNumber)
                elementDeclarationSectionsNumber += 1
                cargo = self.ReadElements(cargo)
            
            # Materials section
            # find the materials section
            if re.search("^(MPTEMP,)", line):
                # print "parsing MPTEMP materials section..."
                if Debug:
                    print "Found materials section at line " + str(lineNumber)
                materialsSectionsNumber += 1
                
                # create the materials NodesMatrix
                cargo = self.ReadMaterials(cargo)
                
            if not line: break
            # output files into the cache directory
        
        if Debug:
                
            print "printing NodesMatrix..."
            for raw in self.NodesMatrix:
                print raw
                
            print "printing ElementsMatrix..."
            for raw in self.ElementsMatrix:
                print raw
           
            print "materials Data..."
            for raw in self.MaterialData:
                print raw
            
        print "Creating nodes cache file..."
        self.StoreNodesToFile()       
        
        print "Creating elements cache file..."
        self.StoreElementsToFile()       
        
        print "Creating materials cache file..."
        self.StoreMaterialsToFile()       
        
        print "nodeSectionsNumber " + str(nodeSectionsNumber)
        print "materialsSectionsNumber " + str(materialsSectionsNumber)
        print "elementTypeSectionsNumber " + str(elementTypeSectionsNumber)
        print "elementDeclarationSectionsNumber " + str(elementDeclarationSectionsNumber)
        
        return nodeSectionsNumber, elementDeclarationSectionsNumber, elementTypeSectionsNumber, \
               materialsSectionsNumber
    
    
        
    def StoreNodesToFile(self):
        """Store data to disk cache"""
        memFile = StringIO.StringIO()
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
    
    def StoreElementsToFile(self):
        """Store data to disk cache"""
        file = StringIO.StringIO()
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
    
    def StoreMaterialsToFile(self):
        #!!         2 "MAT2"
        #MPTEMP,1,             0.0
        #MPDATA,EX  ,2,1,          1000.0
        #MPDATA,NUXY,2,1,             0.3
        
        #   -> Generates ->   

        #MATERIAL NUMBER =      2 EVALUATED AT TEMPERATURE OF   0.0000    
        #EX   =   1000.0    
        #NUXY =  0.30000
        #DENS =  0.10700    
        
        """Store data to disk cache"""
        # ['0.0', 'EX', '2', '1000.0', 'NUXY', '2', '0.3']
        file = StringIO.StringIO()
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
                    i += 2
                                                                        
                # 1 3 4 6
                file.write('\n')
        
        if Debug:
            print file.getvalue()
                
        diskFile = open(self.MaterialsOutputFileName, 'w')
        diskFile.write(file.getvalue())
        diskFile.close()
        
    def ReadNodes(self, cargo):
        """Generate Nodes NodesMatrix"""
        
        file, line = cargo
            
        while 1:
            
            if Debug:
                print " > " + line

            # start grammar
            nodeHeader = "N,"
            nodeId = Word( alphanums ) + Suppress(",")
            nodeCoords =  delimitedList(Word( alphanums + "." + "-"))
            nodeTextLine = nodeHeader + nodeId + nodeCoords
            # end grammar
            
            try: 
                list = nodeTextLine.parseString( line )
                if Debug:
                    print list[1:]
                self.NodesMatrix.append(list[1:])
                # f.writelines(list[3:])

            except ParseException:
                if Debug:
                    print "cannot parse line: " + str(line) + "...exiting..."
                break
            
            line = file.readline()

        # f.close()
        if Debug:
            print "printing NodesMatrix..."
            for raw in self.NodesMatrix:
                print raw
            
        return file, line
    
    def ReadElements(self, cargo):
        """Generate Elements NodesMatrix"""
        
        TYPE = []
        ESYS = []
        EN = []
        EMORE = []
        
        file, line = cargo
            
        # outputNodeMatrixFileName = r'D:\vapps\LHPBuilder_Parabuild\ASCIIParser\testData\ansys\outputNodeMatrix.txt'
        # f = open(outputNodeMatrixFileName, 'w')
        
        while 1:
             
            raw = []
            
            if Debug:
                print " Processing line: " + line 
           
            # parse the TYPE line
            elementType = Suppress("TYPE,") + Word(nums)
            elementMat = Suppress("$") + Suppress(Word( alphas)) + Suppress(",") + Word( nums)
            elementReal = Suppress("$") + Suppress(Word( alphas)) + Suppress(",") + Word( nums)
            typeMatReal = elementType + elementMat + elementReal                        
            
            try: 
                TYPE = typeMatReal.parseString( line )
                raw.append(TYPE)
            except ParseException:
                if Debug:
                    print "cannot parse line: " + line
                break
              
            # 
            line = file.readline()
            if Debug:
                print " Processing line: " + line 
             
            # parse the ESYS line
            esysHeader  = Suppress("ESYS,")
            esysType = Word(nums)
            esysLine = esysHeader + esysType
            
            try: 
                ESYS = esysLine.parseString(line)
                raw.append(ESYS)
            except ParseException:
                if Debug:
                    print "cannot parse line: " + line
                break
                
            line = file.readline()
            
            while 1:
                
                # parse EN line
                if Debug:
                    print " Processing line: " + line 
               
                connectivityHeader  = Suppress("EN,")
                elementId = Word(nums) + Suppress(",")
                connectivityList = delimitedList(Word(nums))
                connectivityLine = connectivityHeader + elementId + connectivityList
    
                try: 
                    EN = connectivityLine.parseString(line)
                    raw.append(EN)
                except ParseException:
                    if Debug:
                        print "cannot parse line: " + line
                    break
                
                # parse EMORE line
                line = file.readline()
                if Debug:
                    print " Processing line: " + line 
               
                moreConnectivityHeader  = Suppress("EMORE,")
                moreConnectivityId = delimitedList(Word(nums))
                moreConnectivityLine = moreConnectivityHeader + moreConnectivityId 
                
                try: 
                    EMORE = moreConnectivityLine.parseString( line )
                    raw.append(EMORE)
                except ParseException:
                    if Debug:
                        print "cannot parse line: " + line
                    break
                
                # 
                line = file.readline()
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
                for component in TYPE:
                    element.append(component)
                
                element.append(ESYS[0])
                element.append('1')
    
                for component in EN[1:]:
                    element.append(component)
    
                for component in EMORE:
                    element.append(component)
                
                # print "component: " + str("  ".join(component))
                self.ElementsMatrix.append(element)
            
            break
        # f.close()
        return file, line

    def ReadMaterials(self,cargo):
        """Generate materials text"""
           
        materialList = []
        #!!         2 "MAT2"
        #MPTEMP,1,             0.0
        #MPDATA,EX  ,2,1,          1000.0
        #MPDATA,NUXY,2,1,             0.3
        
        # or????
        
        #!!         1 "1"
        #!MPTEMP,0
        #MP,DENS,1,         25974.3
        #MP,EX  ,1,         25974.3
        #MP,NUXY,1,             0.3
        #!!HMNAME MAT 
        #!!         2 "2"
        #!MPTEMP,0
        #MP,DENS,2,         25752.3
        #MP,EX  ,2,         25752.3
        #MP,NUXY,2,             0.3

        #   -> Generates ->   

        #MATERIAL NUMBER =      2 EVALUATED AT TEMPERATURE OF   0.0000    
        #EX   =   1000.0    
        #NUXY =  0.30000
        #DENS =  0.10700    
                
        matNumber = ""
        matTemp = ""
        matName = ""
        matValue = ""
        
        # 
        #MPTEMP,1,             0.0 
        
        # 
        #['MPTEMP,', '1', '0.0']

        file, line = cargo

        materialTemp = "MPTEMP," + delimitedList(Word( alphanums + "."))
        
        try: 
            MPTEMP =  materialTemp.parseString( line )
            matTemp = MPTEMP[2]
            materialList.append(matTemp)
            
        except ParseException:
            if Debug:
                print "cannot parse line " + str(lineNum)
            return
        
        # 
        #MPDATA,EX  ,2,1,          1000.0
        #MPDATA,NUXY,2,1,             0.3
        
        # 
        #['MPDATA,', 'EX', '2', '1', '1000.0']
        #['MPDATA,', 'NUXY', '2', '1', '0.3']

        line = file.readline()
        
        while 1:
            matData = "MPDATA," + delimitedList(Word( alphanums + "."))
        
            try: 
                MPDATA = matData.parseString( line )
                matName = MPDATA[1]
                matNumber = MPDATA[2]
                matValue = MPDATA[4]
                materialList.append(matName)
                materialList.append(matNumber)
                materialList.append(matValue)
                
            except ParseException:
                if Debug:
                    print "cannot parse line " + str(line)
                break
        
            line = file.readline()
        
        self.MaterialData.append(materialList)
            
        return file, line

def Run(inputAnsysFileName, cacheFolderName , nodesOutputFileName , elementsOutputFileName , materialsOutputFileName ):                                            
    reader = ansysReader()
    reader.InputAnsysFileName = inputAnsysFileName
    reader.CacheFolderName = cacheFolderName
    reader.NodesOutputFileName = nodesOutputFileName
    reader.ElementsOutputFileName = elementsOutputFileName
    reader.MaterialsOutputFileName = materialsOutputFileName
    reader.Read()    

def main():
    args = sys.argv[1:]
    if len(args) != 5:
        print """
        usage: python.exe ansysReader.py
        inputAnsysFileName 
        cacheFolderName
        nodesOutputFileName
        elementsOutputFileName
        materialsOutputFileName
        """
        sys.exit(-1)
    Run(args[0],args[1],args[2],args[3],args[4])

if __name__ == '__main__':
    main()
    