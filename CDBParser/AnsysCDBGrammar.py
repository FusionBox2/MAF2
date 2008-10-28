#-----------------------------------------------------------------------------
# Grammar for parsing Ansys CDB Files
# author: Daniele Giunchi <d.giunchi@scsolutions.it>
#-----------------------------------------------------------------------------


from pyparsing import Word, alphas, nums, ZeroOrMore, ParseException, Group, delimitedList,\
     alphanums, Literal,Dict, Suppress

#generic
double = Word( nums + "." + "-") 

# CDB NODES grammar
#start section
#first line NBLOCK,6,SOLID
nodeHeader = Word( alphanums ) + Suppress(",")
nodeNumberOfColumns = Word( nums ) + Suppress(",")
modelType = Word( alphanums )
nodeStartLine = nodeHeader + nodeNumberOfColumns + modelType
#second line (3i8,6e16.9)
#ignored 

#core section   legenda (i) = ignored
#linetype   4       0       0  0.00000000      0.00000000      1.00000000
#          ID     IDSE(i)  IDL(i)    X                Y              Z
#end section
nodeID = Word(nums)
nodeIDSolidEntity = Word(nums)
nodeIDLine = Word(nums)

# BEWARE: this node grammar skips rotaded nodes! only note coordinates are parsed
nodeCoords1 = double
nodeCoords2 = double + double
nodeCoords3 = double + double + double

nodeTextLine1 = nodeID + Suppress(nodeIDSolidEntity) + Suppress(nodeIDLine) + nodeCoords1
nodeTextLine2 = nodeID + Suppress(nodeIDSolidEntity) + Suppress(nodeIDLine) + nodeCoords2
nodeTextLine3 = nodeID + Suppress(nodeIDSolidEntity) + Suppress(nodeIDLine) + nodeCoords3
# CDB NODES grammar end


"""# TYPE line grammar
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
# end grammar"""

#------------------------------------------------------------------------
#Element Block Section
#start section
#first line EBLOCK,19,SOLID,       2	

elementHeader = Word( alphanums ) + Suppress(",")
elementNumberOfFields = Word( nums ) + Suppress(",")
modelType = Word( alphanums ) + Suppress(",")
unknownNumber = Word( nums )
elementStartLine =  nodeHeader + nodeNumberOfColumns + Suppress(modelType) + Suppress(unknownNumber)

#second line to ignore

#end section

#Core line (and end-> idmat == -1)
#3       3       3       1       0       0       0       0      10       0       1       5       2       4       3      11      10      14      13
#idmat  idTypeEl idConst (i)............................(i)    numNodes  (i)    NumElem  (connectivity..............  may continue on more lines)  
elementDeclaration = delimitedList(Word( nums + "-"), '')

#------------------------------------------------------------------------
#CMBLOCK,TET     ,ELEM,       2  
#      comp. name comp.type  number of component
#start section
componentHeader = Word( alphanums ) + Suppress(",")
componentName = Word( alphanums + "_") + Suppress(",")
componentTypeOfItems = Word( alphanums ) + Suppress(",")
componentsNumberOfItems = Word( nums )
componentStartLine = componentHeader + componentName + componentTypeOfItems + componentsNumberOfItems
#end start section

#core section and end
#1        -2
# items in the component (elements or nodes)
componentDeclaration = delimitedList(Word( nums + "-"), '')
#-----------------------------------------------------------------------------

#MPDATA,R5.0, 1,EX  ,       2, 1,  1000.00000    
#        (i) (i) (prop)    IDm (i)    (val)
# MPDATA line grammar

matData = "MPDATA," + delimitedList(Word( alphanums + "."))
# end grammar

#-----------------------------------------------------------------------------
