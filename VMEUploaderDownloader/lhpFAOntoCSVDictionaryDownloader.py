#-----------------------------------------------------------------------------
# BEWARE!!! This is mostly a prototype!!!
# code is changing very fast so don't rely on it :P
# author: Stefano Perticoni <s.perticoni@scsolutions.it>
#-----------------------------------------------------------------------------

import lhpFAOntoToLHPCSVConverter
import lhpCSVDictionaryDownloader
import lhpDictionaryVersionChecker
import dictionaryCSV2XML

import  sys

class lhpFAOntoCSVDictionaryDownloader(lhpCSVDictionaryDownloader.lhpCSVDictionaryDownloader):
    """Download FA Ontology csv dictionary from biomedtown and convert it to xml\
    See main() comments for further details"""
    

    def __init__(self):
        
        lhpCSVDictionaryDownloader.lhpCSVDictionaryDownloader.__init__(self)
        self.DictionaryFileSelector = "/biomed_town/LHDL/lhdl-management/Consortium-room/lhdl-repository/\
        WP5/Dictionaries/FA_onto.csv"
    
    def ConvertFAOntoCSVToLHPCSV(self):
        FAOnto2LHPCSV = lhpFAOntoToLHPCSVConverter.lhpFAOntoToLHPCSVConverter()
        FAOnto2LHPCSV.InputCSVDictionaryFileName = self.DownloadedCSVDictionaryFileName
        FAOnto2LHPCSV.OutputCSVDictionaryFileName = "LHPConverted_" + self.DownloadedCSVDictionaryFileName
        FAOnto2LHPCSV.BuildOutputDictionary()
        
    def ConvertLHPCSV2XML(self):
        csv2xml = dictionaryCSV2XML.dictionaryCSV2XML()
        csv2xml.InputCSVDictionaryFileName = "LHPConverted_" + self.DownloadedCSVDictionaryFileName
        csv2xml.OutputXMLDictionaryFileName = self.OutputXMLDictionaryFileName

        csv2xml.BuildXMLDictionary()
        
    
def run(host, DictionaryFileSelector, outputXMLDictionaryFileName):                                            
    
    
    dictDownloader = lhpFAOntoCSVDictionaryDownloader()    
    dictVersionChecker = lhpDictionaryVersionChecker.lhpDictionaryVersionChecker()
    dictDownloader.Host = host
    dictDownloader.OutputXMLDictionaryFileName = outputXMLDictionaryFileName
    
    dictDownloader.RemoveOldDictionariesFromDisk()
    
    # get the dictionary creation date
    date = dictVersionChecker.GetRemoteDictionaryDate()
    if date == -1:
        print " Cannot retrieve dictionary creation date: dictionary will not be created!"
        return -1 
    
    dictDownloader.DictionaryFileSelector = DictionaryFileSelector
    dictDownloader.OutputXMLDictionaryFileName = outputXMLDictionaryFileName  + "_" + str(date) + ".xml"
    
    dictDownloader.DownloadCSVDictionary()
    dictDownloader.ConvertFAOntoCSVToLHPCSV()
    dictDownloader.ConvertLHPCSV2XML()
    
def main():
    args = sys.argv[1:]
    if len(args) != 3:
        print """
        lhpFAOntoCSVDictionaryDownloader.py
        host FAOntoDictionaryFileSelector outputXMLDictionaryFileName
        
        For example:
        host = "www.biomedtown.org"
        DictionaryFileSelector = "/biomed_town/LHDL/lhdl-management/Consortium-room/lhdl-repository/WP5/Dictionaries/FA_onto.csv"

        Notes:
        "DateTime" and ".xml" strings will be appended to outputXMLDictionaryFileName
        """
        sys.exit(-1)
    print args
    run(args[0],args[1],args[2])

    

if __name__ == '__main__':
    main()