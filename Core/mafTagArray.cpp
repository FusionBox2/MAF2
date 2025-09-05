#include "mafTagArray.h"

#include "ftk/IO/StorageElement.h"
#include "ftk/IO/ParseContainers.h"
#include "mafIndent.h"
#include <assert.h>

mafTagArray::mafTagArray() = default;

mafTagArray::~mafTagArray() = default;

mafTagArray& mafTagArray::operator=(const mafTagArray& a)
{
    Superclass::operator=(a);
    return *this;
}

void mafTagArray::DeepCopy(const mafTagArray* a)
{
    Superclass::DeepCopy(a);
    m_Tags = a->m_Tags;
}

void mafTagArray::DeepCopy(const mafAttribute* a)
{
    if (a->IsMAFType(mafTagArray))
    {
        DeepCopy(static_cast<const mafTagArray*>(a));
    }
}

const mafTagItem* mafTagArray::GetTag(const mafString& name) const
{
    if (auto  it = m_Tags.find(name); it != end(m_Tags))
        return &it->second;
    return nullptr;
}

mafTagItem* mafTagArray::GetTag(const mafString& name)
{
    if (auto  it = m_Tags.find(name); it != end(m_Tags))
        return &it->second;
    return nullptr;
}

void mafTagArray::SetTag(const mafTagItem& value)
{
    m_Tags[value.GetName()] = value;
}

void mafTagArray::DeleteTag(const mafString& name)
{
    if (auto  it = m_Tags.find(name); it != end(m_Tags))
        m_Tags.erase(it);
}

std::vector<mafString> mafTagArray::GetTagList() const
{
    std::vector<mafString> res;
    res.reserve(m_Tags.size());
    for (auto& entry : m_Tags)
    {
        res.push_back(entry.second.GetName());
    }
    return res;
}

bool mafTagArray::operator==(const mafTagArray& a) const
{
    return Equals(&a);
}

bool mafTagArray::Equals(const mafTagArray* array) const
{
    if (!array)
        return false;
    return m_Tags == array->m_Tags;
}

/*void mafTagArray::GetTagsByType(int type, std::vector<mafTagItem *> &array)
{
  array.clear();
  mmuTagsMap::iterator it=m_Tags.begin();
  for (;it!=m_Tags.end();it++)
  {
    if (it->second.GetType()==type)
    {
      array.push_back(&(it->second));
    }
  }
}*/
size_t mafTagArray::GetNumberOfTags() const
{
    return m_Tags.size();
}

void mafTagArray::InternalStore(mafStorageElementBuilder& parent)
{
    Superclass::InternalStore(parent);
    parent(_R("NumberOfTags")).SetValue(GetNumberOfTags());
    for (auto& item : m_Tags)
    {
        parent[_R("TItem")].SetValue(item.second);
    }
}

void mafTagArray::InternalRestore(const mafStorageElement& node)
{
    Superclass::InternalRestore(node);// == MAF_OK)
    mafID numAttrs = node(_R("NumberOfTags")).As<mafID>();

    auto children = node[_R("TItem")].As<std::vector<mafTagItem>>();
    for (auto& item : children)
    {
        SetTag(item);
    }
}

void mafTagArray::Print(std::ostream& os, const int tabs) const
{
    Superclass::Print(os, tabs);

    mafIndent indent(tabs);
    os << indent << "Tags:" << std::endl;
    mafIndent next_indent(indent.GetNextIndent());

    for (auto& entry : m_Tags)
    {
        entry.second.Print(os, next_indent);
    }
}