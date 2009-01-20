#-----------------------------------------------------------------------------
# BEWARE!!! This is mostly a prototype!!!
# code is changing very fast so don't rely on it :P
# author: Stefano Perticoni <s.perticoni@scsolutions.it>
#-----------------------------------------------------------------------------

import os
import sys, string

import unittest
from cStringIO import StringIO
from Debug  import Debug
import ansysReader
import shutil
from pyparsing import Word, alphas, nums, ZeroOrMore, ParseException, Group, delimitedList, alphanums, Literal,Dict, Suppress
import pprint
import progressBar

class ansysWriter:
        
    
    def __init__(self):                       
        
        self.NodesFileName = "UNDEFINED"        
        self.ElementsFileName = "UNDEFINED"
        self.  MaterialsFileName = "UNDEFINED"
        self.AnsysOutputFileName = "AnsysOutput.inp"
        
        self.ProgressBar = progressBar.progressBar()
            
    def Write(self):
        
        self.PrintProgress(0)
        
        self.AnsysHeader = r"""/TITLE,
/COM,lhpBuilder generated
/PREP7

"""
        
        if Debug:
            print self.AnsysHeader
            
        # read the nodes file in memory
        ansysSeparator = "             "        
     
        self.NodesMatrix = []
        
        diskFile = open(self.NodesFileName)
        self.FileLinesNumber = 0
        for curr in diskFile.readlines() :
            self.FileLinesNumber += 1
            if Debug:
                print curr
            
            self.NodesMatrix.append(curr.split())
            
        print "Input files contains: " + str(self.FileLinesNumber) + " lines" 
        
        if Debug:
            print self.NodesMatrix
            
        
        # write nodes section
        
        
        self.NodesNSectionList = []
        
        self.ParsedLineNumber = 0
     
        ansysSeparator = "             "
        
        for row in self.NodesMatrix:
            
            # file first-line
            #  ID |     point coord
            # ['1', '0', '0', '0']
            if Debug:
                print row
                
            ansysNodesLine = "N," + row[0] + "," + ansysSeparator + row[1] + \
            "," + ansysSeparator + row[2] + \
            "," + ansysSeparator + row[3] + '\n'
            

            self.NodesNSectionList.append(ansysNodesLine)
            
            
        if Debug:
            for row in self.NodesNSectionList:
                print row
                
        """
        from:
        #  nlist.txt
        1 0 0 0 
        2 1 0 0 
        3 0 1 0 
        
        to:
        # ansys nodes section:
        N,1,             0.0,             0.0,             0.0
        N,2,             1.0,             0.0,             0.0
        N,3,             0.0,             1.0,             0.0
        """

        self.PrintProgress(20)
        
        # write materials section
                 
        self.MaterialsMatrix = []

        diskFile = open(self.MaterialsFileName)
        self.FileLinesNumber = 0
        for curr in diskFile.readlines() :
            self.FileLinesNumber += 1
            if Debug:
                print curr
            
            self.MaterialsMatrix.append(curr)
            
        print "Input files contains: " + str(self.FileLinesNumber) + " lines" 
        
        if Debug:
            print self.MaterialsMatrix

        diskFile.seek(0)
        
        line = diskFile.readline()
        # read the material number line
            
        self.MaterialsMPDATASectionList = []
        
        self.MaterialsIDList = []
        
        while 1:
            
            # search for material number line
            materialID =  -1 
        
            try: 
                materialHeaderGrammar = Suppress("MATERIAL NUMBER = ") + Word(nums)
                materialIDList =  materialHeaderGrammar.parseString(line)
                materialID = materialIDList[0]
                self.MaterialsIDList.append(materialID)
                
                if Debug:
                    print "Material ID: " + str(materialID)
                
            except ParseException:
                if Debug:
                    print "cannot parse line "
                
            # this is a guess: this parameter needs to be supported
            NumberOfComponents = 1
            
            # While there is a property read the property lines
            while 1:
                line = diskFile.readline()
                
                propertyString = "UNDEFINED"
                propertyValue = -9999
                
                try: 
                    materialPropertiesGrammar = Word(alphas) + Suppress("=") + Word( alphanums + "." + "-")
                    material = materialPropertiesGrammar.parseString(line)
                    propertyString = material[0]
                    propertyValue = material[1]
                    MPDATALine = "MP," + str( propertyString ) + "," + str( materialID) + \
                                 "," + ansysSeparator + str( propertyValue ) + '\n'
                    
                    self.MaterialsMPDATASectionList.append(MPDATALine)
                    
                    if Debug:
                        print "Property: " + str(propertyString)
                        print "Value: " + str(propertyValue)                    
                        print "MPDATALine: " + MPDATALine
                    
                except ParseException:
                    if Debug:
                        print "cannot parse line "
                    break
            
            self.MaterialsMPDATASectionList.append('\n')
            
            # search for the next material if any...
            # exit when end of file is found
            if not line: break
                
            # read next line
            line = diskFile.readline() 
        
        if Debug:
            print "Materials Section:"
            
            for row in self.MaterialsMPDATASectionList:
                print row
            
            print "Materials ID List:"
            print self.MaterialsIDList

        self.PrintProgress(40)
        
        # write elements section
                
        self.ElementsMatrix = []

        diskFile = open(self.ElementsFileName)
        self.FileLinesNumber = 0
        for curr in diskFile.readlines() :
            self.FileLinesNumber += 1
            if Debug:
                print curr
            
            self.ElementsMatrix.append(curr)
            
        print "Input files contains: " + str(self.FileLinesNumber) + " lines" 
        
        if Debug:
            for row in self.ElementsMatrix:
                print row

        diskFile.seek(0)
        
        line = diskFile.readline()
        # read the material number line
        
        """
        TYPE, 2  $ MAT, 2  $ REAL, 3
ESYS, 0
EN,       2,       3,       1,       2,       4,       7,       6,       9,      12
EMORE,       8,      10
!!!!le 5 righe seguenti sono comandi di selezione e raggruppamento di ANSYS, vanno ignorati nell'import, sono da riconsiderare per l'export 
ESEL, S, TYPE,,2
ESEL, R, MAT,,2
ESEL, R, REAL,,3
CM, TYPE2-REAL3-MAT2, ELEM
ESEL, ALL
        """
        
        self.ElementsSectionList = []
        
        groupsDictionary = {}
           
        for row in self.ElementsMatrix:
            
            elementIdColumn = 0
            materialColumn = 1
            typeColumn = 2
            realColumn = 3
            esysColumn = 4
            compColumn = 5
            
            headerSize = 6
            
            rowSplit = row.split()
            if Debug:
                print rowSplit
            
            currentGroupKey =  str(rowSplit[materialColumn]) + "_" + str(rowSplit[typeColumn])
            
            hasKey = groupsDictionary.has_key(currentGroupKey)
            
            # check if this group already exist

            if hasKey == False:
                # create new group
                groupsDictionary[currentGroupKey] = []
                groupsDictionary[currentGroupKey].append(rowSplit)
                
                if Debug:
                    print "key: " + str(currentGroupKey) + " not found! creating new group"
                    print groupsDictionary[currentGroupKey]
            
            else:
                groupsDictionary[currentGroupKey].append(rowSplit)
                if Debug:
                    print "key: " + str(currentGroupKey) + " found! appending... " + str(rowSplit) 
            
         
        # for each group key create an element group 
        for key in groupsDictionary:
            
            # print type mat real stuff...
             
            groupElementsMatrix = groupsDictionary[key]
           
            probeLine = groupElementsMatrix[0]
            
            pointsNumber = len(probeLine) - headerSize
            if Debug:
                print "Number of points: " + str(pointsNumber)
                print "Group: " + str(probeLine[materialColumn]) + " , " + str(probeLine[typeColumn])
                   
            typeLine =  "TYPE, " + str(probeLine[typeColumn]) + \
              " $ MAT, " + str(probeLine[materialColumn]) + \
              " $ REAL, " +  str(probeLine[realColumn])+ '\n'
        
            self.ElementsSectionList.append(typeLine)

            # commented since it is not present in Test Files
            # esysLine =  "ESYS, " + str(probeLine[esysColumn])+ '\n'
            # self.ElementsSectionList.append(esysLine)
            
            
            # for each element in group...
            
            for elementRow in groupElementsMatrix:
                
                if Debug:
                    print "element row: "
                    print elementRow

    
                ENPointsNumber = -1
                
                
                if pointsNumber  > 8: 
                    ENPointsNumber = 8
                else:
                    ENPointsNumber = pointsNumber
                    
                enLine = "EN," + ansysSeparator + str(elementRow[elementIdColumn]) 
    
                for point in elementRow[headerSize:(headerSize + ENPointsNumber)]:
                     enLine = enLine + "," + ansysSeparator  + str(point)
                
                self.ElementsSectionList.append(enLine + '\n')
                     
                if Debug:
                    print enLine
                
                if pointsNumber  > 8: 
                    
                    # if the element has more than 8 nodes write the EMORE line also
                    emoreLine = "EMORE"
    
                    for point in elementRow[(headerSize + ENPointsNumber):]:
                         emoreLine = emoreLine + "," + ansysSeparator  + str(point)
                    
                    self.ElementsSectionList.append(emoreLine + '\n')
                    if Debug:
                        print emoreLine
                        
            cmLine  = "CM, TYPE" + str(elementRow[typeColumn]) + "-REAL" + str(elementRow[realColumn]) \
                  + "-MAT" + str(elementRow[materialColumn]) + ", ELEM" + '\n'
            self.ElementsSectionList.append(cmLine)
            
            
            self.ElementsSectionList.append('\n')
        
        if Debug:
            print "Elements Matrix: " + '\n'
            for line in self.ElementsSectionList:
                print line
                
        self.PrintProgress(60)
        
        # ET section
        
        # gather groups id
        ETGroupsIdDictionary = {}
        
        for key in groupsDictionary:
            pos = key.rfind("_")
            groupId = key[pos + 1:]
            if ETGroupsIdDictionary.has_key(groupId) == False:
                ETGroupsIdDictionary[groupId] = "dummy val"

        
        # probe for element type
   
        
        nodesPerElement = len(self.ElementsMatrix[0].split()) - 6
            
        elementId = -1
        
        if nodesPerElement == 4 or nodesPerElement == 8 :
            elementId = 45
            
        elif nodesPerElement == 10:
            elementId = 187

        elif nodesPerElement == 10:
            elementId = 186
            
        if Debug:
            print "Nodes per element: " + str(nodesPerElement)
            print "Element ANSYS Id: " + str(elementId)

        self.MaterialsETSectionList = []
        for key in ETGroupsIdDictionary:
            etLine = "ET," + key + "," + str(elementId) + '\n'
            self.MaterialsETSectionList.append(etLine)
            
        if Debug:
            for row in self.MaterialsETSectionList:
                print row
                
        
        
        self.EndSection = "FINISH"
        
        self.PrintProgress(80)
        
        # write output         
        
        ansysOutputFile = open(self.AnsysOutputFileName, 'w')
        self.FileLinesNumber = 0
        
        ansysOutputFile.writelines(self.AnsysHeader)
        
        for line in self.NodesNSectionList:

            ansysOutputFile.writelines(line)
            self.FileLinesNumber += 1
            
            if Debug:
                print line
            
        ansysOutputFile.write('\n')
            
        for line in self.MaterialsMPDATASectionList:

            ansysOutputFile.writelines(line)
            self.FileLinesNumber += 1
            
            if Debug:
                print line
         
        ansysOutputFile.write('\n')
        
        for line in self.MaterialsETSectionList:

            ansysOutputFile.writelines(line)
            self.FileLinesNumber += 1
            
            if Debug:
                print line
    
        ansysOutputFile.write('\n')
        
        for line in self.ElementsSectionList:

            ansysOutputFile.writelines(line)
            self.FileLinesNumber += 1
            
            if Debug:
                print line
            
        eselAllLine = "ESEL, ALL" + '\n'
        ansysOutputFile.write(eselAllLine)
        ansysOutputFile.write('\n')
        ansysOutputFile.writelines(self.EndSection)
        ansysOutputFile.write('\n')



        self.PrintProgress(100)
        
    def PrintProgress(self,progress):
        self.ProgressBar.updateAmount(progress)
        print self.ProgressBar, '\r'
    
def Run(nodesFileName , elementsFileName , materialsFileName, ansysOutputFileName  ):
    
    
    exporter = ansysWriter()
    exporter.NodesFileName = nodesFileName       
    exporter.ElementsFileName = elementsFileName
    exporter.MaterialsFileName = materialsFileName
    exporter.AnsysOutputFileName = ansysOutputFileName
    exporter.Write()
    
  


def main():
    args = sys.argv[1:]
    
    if Debug:
        
        fileName = r'ansysWriterDebugLog.txt'
        print fileName
        f = open(fileName, 'w')
        for arg in args:
             f.write(str(arg))
             f.write('\n')
        f.close()
        
    
    if len(args) != 4:
        print """
        usage: python.exe ansysWriter.py
        nodesFileName       
        elementsFileName
        materialsFileName
        ansysOutputFileName
        """
        sys.exit(-1)
    Run(args[0],args[1],args[2],args[3])

if __name__ == '__main__':
    main()

    
    