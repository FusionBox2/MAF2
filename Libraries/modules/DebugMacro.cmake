#Macro that print messages useful in debug phase.
MACRO(DBG_MESSAGE MSG)
	IF(DEBUG_MESSAGES) 
		MESSAGE(${MSG})
	ENDIF(DEBUG_MESSAGES)
ENDMACRO(DBG_MESSAGE)