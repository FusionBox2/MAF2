#pragma once
//----------------------------------------------------------------------------
// forward declarations:
//----------------------------------------------------------------------------
class mafNode;
class vtkMAFAssembly;
class mafPipe;
class vtkRenderer;
class mafSceneGraph;
//----------------------------------------------------------------------------
// mafSceneNode :
//----------------------------------------------------------------------------
/** mafSceneNode is a node of mafScenegraph. */
class MAF_EXPORT mafSceneNode
{
public:
  mafSceneNode(mafSceneGraph *sg, mafSceneNode *parent, std::shared_ptr<mafNode> vme, vtkRenderer *ren1, vtkRenderer *ren2 = nullptr, vtkRenderer *ren3 = nullptr);
  virtual      ~mafSceneNode    ();

  /** Call Select method for the vme's pipe. */
	void Select(bool select);

  /** Call Show method for the vme's pipe. */
	//void Show(bool show);  --  //SIL. 21-4-2005: removed unused function Show from mafScenenNode and mafPipe

  /** Call UpdateProperty method for the vme's pipe. */
	void UpdateProperty(bool fromTag = false);

  /** Return the vme's visibility. */
  bool IsVisible() {return m_Pipe != NULL;};

  /** print a dump of this object */
  virtual void Print(std::ostream& os, const int tabs=0);// const;

  std::shared_ptr<mafNode> m_Vme;
  mafSceneNode      *m_Parent;
  std::shared_ptr<mafPipe> m_Pipe;
  bool               m_PipeCreatable;
  bool               m_Mutex;
  vtkRenderer       *m_RenFront;
  vtkMAFAssembly    *m_AssemblyFront;
  vtkRenderer       *m_RenBack;
  vtkMAFAssembly    *m_AssemblyBack;
  vtkRenderer       *m_AlwaysVisibleRenderer;
  vtkMAFAssembly    *m_AlwaysVisibleAssembly;
  mafSceneNode      *m_Next;
  mafSceneGraph     *m_Sg;
};
