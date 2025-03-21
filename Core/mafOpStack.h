#pragma once

//----------------------------------------------------------------------------
// forward references :
//----------------------------------------------------------------------------
class mafOp;
//----------------------------------------------------------------------------
// mafOpStack :
//----------------------------------------------------------------------------
/**
*/
class MAF_EXPORT mafOpStack
{
public:
	mafOpStack();

	~mafOpStack(); 
	
	/** Return true is the stack is empty */
	bool IsEmpty() const;

  /** Insert op on top od the stack */
	void Push(mafOp* op);
	
  /** Return the op on the top of the stack, and remove it */
	mafOp* Pop();
	
  /** Empty the stack and call delete on every inserted op */
	void Clear();

protected:
	mafOp *m_Stack = nullptr;
};
