#-----------------------------------------------------------------------------
# Grammar for parsing Ansys Input Files
# author: Stefano Perticoni <s.perticoni@scsolutions.it>
#-----------------------------------------------------------------------------



from pyparsing import Word, alphas, nums, ZeroOrMore, ParseException, Group, delimitedList,\
     alphanums, Literal,Dict, Suppress

# NODES grammar
nodeHeader = "N,"
nodeId = Word( alphanums ) + Suppress(",")
nodeCoords =  delimitedList(Word( alphanums + "." + "-"))
nodeTextLine = nodeHeader + nodeId + nodeCoords
# end grammar
           
# TYPE line grammar
elementType = Suppress("TYPE,") + Word(nums)
elementMat = Suppress("$") + Suppress(Word( alphas)) + Suppress(",") + Word( nums)
elementReal = Suppress("$") + Suppress(Word( alphas)) + Suppress(",") + Word( nums)
typeMatReal = elementType + elementMat + elementReal                        
# end grammar


# ESYS line grammar
esysHeader  = Suppress("ESYS,")
esysType = Word(nums)
esysLine = esysHeader + esysType
# end grammar

 # EN line grammar 
connectivityHeader  = Suppress("EN,")
elementId = Word(nums) + Suppress(",")
connectivityList = delimitedList(Word(nums))
connectivityLine = connectivityHeader + elementId + connectivityList
# end grammar

 # EMORE line grammar
moreConnectivityHeader  = Suppress("EMORE,")
moreConnectivityId = delimitedList(Word(nums))
moreConnectivityLine = moreConnectivityHeader + moreConnectivityId 
# end grammar

# MTEMP line grammar
materialTemp = "MPTEMP," + delimitedList(Word( alphanums + "."))
# end grammar


# MPDATA line grammar
matData = "MPDATA," + delimitedList(Word( alphanums + "."))
# end grammar
 