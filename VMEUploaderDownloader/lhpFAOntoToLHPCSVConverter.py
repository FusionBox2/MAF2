#-----------------------------------------------------------------------------
# BEWARE!!! This is mostly a prototype!!!
# code is changing very fast so don't rely on it :P
# author: Stefano Perticoni <s.perticoni@scsolutions.it>
#-----------------------------------------------------------------------------

from Debug import Debug

import csv, sys
import Enum
from xml.dom.minidom import Document
import os

class lhpFAOntoToLHPCSVConverter:    
    """convert Functional Anatomy Ontology CSV to LHDL CSV dictionary format"""
    
    def __init__(self):                                
        
        self.LHPDictionaryColumnLabels = Enum.Enum([\
            'NumTag', 'L1', 'L2', 'L3', 'L4', 'L5', 'L6', 'L7', 'L8',\
            'ValueType', 'Multiplicity', 'ValueList', 'Editable', 'Searchable',\
            'DefaultValue', 'Expert', 'Notes'])

        self.FAOntoDictionaryColumnLabels = Enum.Enum([\
            'NumTag','L1','L2','L3','L4','L5','L6','L7','L8',\
            'ValueType','Editable','Searchable','Value','IsPartOf'\
            ,'HasPart','AttachTo','ArticulateWith','Expert'])

        
        self.InputCSVDictionaryFileName = "UNDEFINED"
        self.OutputCSVDictionaryFileName = "outputCSVDictionary.csv"
   
        
    def BuildOutputDictionary(self): 
        
        reader = csv.reader(open(self.InputCSVDictionaryFileName, "r"))       
        reader.next()
        
        firstOutputCSVDictionaryFileNameLine = '"NumTag","L1","L2","L3","L4","L5","L6","L7","L8",\
"ValueType","Multiplicity","ValueList","Editable","Searchable","DefaultValue","Expert","Notes"\n'
   
        outputList = []
        outputList.append(firstOutputCSVDictionaryFileNameLine)
        
        try:
            for row in reader:
                                  
                 ontoLabels = self.FAOntoDictionaryColumnLabels             
                 numTagLabel = ontoLabels.NumTag.asInt
                 editableLabel = ontoLabels.Editable.asInt         
                 
                 tmp = ""
                 
                 for label in ontoLabels[numTagLabel:editableLabel]:
                     value = row[label.asInt]

                     tmp = tmp + "\"" + value + "\","                      
                     
                 
#      SOURCE   
#            
#            'editableLabel','Searchable','Value','IsPartOf'\
#            ,'HasPart','AttachTo','ArticulateWith','Expert'])
#
                 
                 sourceEditableValue = row[ontoLabels.Editable.asInt]
                 
                 sourceSearchableValue = row[ontoLabels.Searchable.asInt]
                 
                 sourceValueValue = row[ontoLabels.Value.asInt]
                 
                 sourceIsPartOfValue = row[ontoLabels.IsPartOf.asInt]
                 
                 sourceHasPartValue = row[ontoLabels.HasPart.asInt]
                 
                 sourceAttachToValue = row[ontoLabels.AttachTo.asInt]
                 
                 sourceArticulateWith = row[ontoLabels.ArticulateWith.asInt]
                
                 sourceExpertValue = row[ontoLabels.Expert.asInt]
                 
                 targetMultiplicityValue = 1
                 targetValueListValue = ""
                 targetEditableValue = sourceEditableValue
                 targetSearchableValue = sourceSearchableValue
                 targetDefaultValue = sourceValueValue
                 targetExpertValue = sourceExpertValue
                 targetNotesValue = ""
                 
                 tmp = tmp + "\"" + str(targetMultiplicityValue) + "\""\
                 + ",\"" + targetValueListValue + "\"" \
                 + ",\"" + targetEditableValue + "\""\
                 + ",\"" + targetSearchableValue + "\""\
                 + ",\"" + targetDefaultValue + "\""\
                 + ",\"" + targetExpertValue + "\""\
                 + ",\"" + targetNotesValue + "\"\n" 
                 
                 outputList.append(tmp)
                  
#      TARGET    
#            'Multiplicity', 'ValueList', 'Editable', 'Searchable',\
#            'DefaultValue', 'Expert', 'Notes'])
 
         
        except csv.Error, e:
            sys.exit('file %s, line %d: %s' % (filename, reader.line_num, e))
            
        
        
        writer = open(self.OutputCSVDictionaryFileName, "w")
        for row in outputList:
            writer.write(row)
                
        if Debug:
            print "Output file " + self.OutputCSVDictionaryFileName + " written to:"
            print os.getcwd() + "\\" + self.OutputCSVDictionaryFileName
                
def run(lhdlMasterDictionary, outputXMLDictionaryFileName):                      
    
    csvToXML = lhpFAOntoToLHPCSVConverter()
    csvToXML.InputCSVDictionaryFileName = lhdlMasterDictionary
    csvToXML.OutputCSVDictionaryFileName = outputXMLDictionaryFileName
    csvToXML.BuildOutputDictionary()
    
def main():
    args = sys.argv[1:]
    if len(args) != 2:
        print 'usage: lhpFAOntoToLHPCSVConverter FA_Onto.csv ConvertedOnto.csv'
        sys.exit(-1)
    run(args[0], args[1])

if __name__ == '__main__':
    main()

