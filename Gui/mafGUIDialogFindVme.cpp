#include "mafGUIDialogFindVme.h"
#include "mafDecl.h"
#include "mafGUIValidator.h"
#include "ftk/Gui/wx/CheckTree.h"

#include "mafVMERoot.h"

enum FIND_VME_ID
{
  FIND_STRING = MINID,
  MATCH_CASE_ID,
  WHOLE_WORD_ID,
  FIND_NEXT
};
//----------------------------------------------------------------------------
mafGUIDialogFindVme::mafGUIDialogFindVme(const mafString& title, long style)
: mafGUIDialog(title, style)
//----------------------------------------------------------------------------
{
  m_SearchString = _R("");
  m_MatchCase = 0;
  m_WholeWord = 0;
  m_Root = NULL;
  m_Tree = NULL;
  m_NodeFoundList.clear();

  m_Gui = new mafGUI(this);
  m_Gui->String(FIND_STRING,_L("find vme: "), &m_SearchString);
  m_Gui->Bool(MATCH_CASE_ID, _L("match case"), &m_MatchCase, 1);
  m_Gui->Bool(WHOLE_WORD_ID, _L("match whole word"), &m_WholeWord, 1);

  wxBoxSizer *buttons_sizer = new wxBoxSizer(wxHORIZONTAL);
  wxButton *findNextButton = new wxButton(this,FIND_NEXT,_("find next"));
  findNextButton->SetValidator(mafGUIValidator(this, FIND_NEXT, findNextButton));
  wxButton *cancelButton = new wxButton(this,wxID_CANCEL,_("cancel"));
  buttons_sizer->Add(findNextButton);
  buttons_sizer->Add(cancelButton);
  
  Add(m_Gui,1);
  Add(buttons_sizer,0);
}
//----------------------------------------------------------------------------
mafGUIDialogFindVme::~mafGUIDialogFindVme()
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
void mafGUIDialogFindVme::OnEvent(mafEventBase *maf_event)
//----------------------------------------------------------------------------
{
  switch(maf_event->GetId())
  {
    case FIND_STRING:
    break;
    case FIND_NEXT:
    {
      if (!FindNextNode(m_SearchString.GetCStr()))
      {
        if (m_NodeFoundList.size() == 0)
        {
          mafMessage(_M(_L("No VME found named ") + m_SearchString));
        }
        else
        {
          mafMessage(_M(mafString(_L("End of the tree!!"))));
          m_NodeFoundList.clear();
        }
      }
    }
    break;
    case MATCH_CASE_ID:
    break;
    case WHOLE_WORD_ID:
    break;
    default:
      mafGUIDialog::OnEvent(maf_event);
  }
}
//----------------------------------------------------------------------------
bool mafGUIDialogFindVme::FindNextNode(const char *text)
//----------------------------------------------------------------------------
{
  if (m_Root == NULL || m_Tree == NULL)
  {
    return false;
  }
  m_SearchString = _R(text);
  mafNode *nodeFound = FindInTreeByName(m_Root, m_SearchString.GetCStr(), m_MatchCase != 0, m_WholeWord != 0);
  if (nodeFound == NULL)
  {
    return false;
  }
  return m_Tree->SelectNode((intptr_t)nodeFound);
}
//----------------------------------------------------------------------------
void mafGUIDialogFindVme::SetTree(mafGUICheckTree *tree)
//----------------------------------------------------------------------------
{
  m_Tree = tree;
  if (m_Tree != NULL)
  {
    mafNode *n = m_Tree->GetSelectedNode();
    if (n != NULL)
    {
      m_Root = mafVMERoot::SafeDownCast(n->GetRoot());
    }
  }
}
//-------------------------------------------------------------------------
mafNode *mafGUIDialogFindVme::FindInTreeByName(mafNode *node, const char *name, bool match_case, bool whole_word)
//-------------------------------------------------------------------------
{
  wxString word_to_search;
  word_to_search = name;
  wxString myName = mafStringToWx(node->GetName());

  if (!match_case)
  {
    word_to_search.MakeLower();
    myName.MakeLower();
  }

  if (whole_word)
  {
    if (myName == word_to_search)
    {
      if (!AlreadyFound(node))
      {
        m_NodeFoundList.push_back(node);
        return node;
      }
    }
  }
  else
  {
    if (myName.Find(word_to_search) != -1)
    {
      if (!AlreadyFound(node))
      {
        m_NodeFoundList.push_back(node);
        return node;
      }
    }
  }

  for (int i = 0; i < node->GetNumberOfChildren(); i++)
  {
    if (mafNode *n = FindInTreeByName(node->GetChild(i).get(), name, match_case, whole_word))
      return n;
  }
  return NULL;
}
//-------------------------------------------------------------------------
bool mafGUIDialogFindVme::AlreadyFound(mafNode *n)
//-------------------------------------------------------------------------
{
  int checkNode = 0;
  bool alreadyFound = false;

  for (; checkNode < m_NodeFoundList.size(); checkNode++)
  {
    if (n == m_NodeFoundList[checkNode])
    {
      alreadyFound = true;
    }
  }
  return alreadyFound;
}
