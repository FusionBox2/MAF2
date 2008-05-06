#-----------------------------------------------------------------------------
# Grammar for parsing Ansys Input Files
# author: Stefano Perticoni <s.perticoni@scsolutions.it>
#-----------------------------------------------------------------------------


from pyparsing import Word, alphas, nums, ZeroOrMore, ParseException, Group, delimitedList,\
     alphanums, Literal,Dict, Suppress


# NODES grammar
nodeHeader = "N,"
nodeId = Word( alphanums ) + Suppress(",")
# nodeCoords =  delimitedList(Word( alphanums + "." + "-"))
double = Word( alphanums + "." + "-") 
nodeCoords = double + Suppress(",") + double + Suppress(",") + double

nodeTextLine = nodeHeader + nodeId + nodeCoords
# end grammar

# TYPE line grammar
elementType = Suppress("TYPE,") + Word(nums)
elementMat = Suppress("$") + Suppress(Word( alphas)) + Suppress(",") + Word( nums)
elementReal = Suppress("$") + Suppress(Word( alphas)) + Suppress(",") + Word( nums)

typeMatReal = elementType + elementMat + elementReal                        
# end grammar

#-----------------------------------------------------------------------------

# ESYS line grammar
esysHeader  = Suppress("ESYS,")
esysType = Word(nums)

esysLine = esysHeader + esysType
# end grammar

#-----------------------------------------------------------------------------

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

#-----------------------------------------------------------------------------

#MTEMP line grammar

materialTemp = "MPTEMP," + delimitedList(Word( alphanums + "."))
# end grammar

#-----------------------------------------------------------------------------

#MPDATA,DENS,2,1,          1.0
#MPDATA,EX  ,2,1,          1000.0
#MPDATA,NUXY,2,1,             0.3

# MPDATA line grammar
matData = "MPDATA," + delimitedList(Word( alphanums + "."))
# end grammar

#-----------------------------------------------------------------------------

#MP,DENS,2,             1
#MP,EX  ,2,             1000.0
#MP,NUXY,2,             0.3

# MP line grammar
matData2 = "MP," + delimitedList(Word( alphanums + "."))
# end grammar

