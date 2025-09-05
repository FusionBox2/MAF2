#pragma once

#include "mafAttribute.h"
#include "mafTagItem.h"
#include <map>

BEGIN_FTK_NAMESPACE

class FTK_CORE_EXPORT mafTagArray : public mafAttribute
{
public:
    using Tags = std::map<mafString, mafTagItem>;

    mafTagArray();

    ~mafTagArray() override;

    mafTypeMacro(mafTagArray, mafAttribute)

        mafTagArray& operator=(const mafTagArray& a);

    bool operator==(const mafTagArray& a) const;

    /** provide access to vector items. If idx is outside the result is invalid */
    //mafTagItem &operator [](const char *name);

    /** provide access to vector items. If idx is outside the result is invalid */
    //const mafTagItem &operator [](const char *name) const;

    const mafTagItem* GetTag(const mafString& name) const;

    mafTagItem* GetTag(const mafString& name);

    void SetTag(const mafTagItem& value);

    void DeleteTag(const mafString& name);

    std::vector<mafString> GetTagList() const;

    bool Equals(const mafTagArray* array) const;

    void DeepCopy(const mafTagArray* a);

    void DeepCopy(const mafAttribute* a) override;

    /**
    Search the tag array for tags of a given type and
    put in the array pointers to them   */
    //void GetTagsByType(int type, std::vector<mafTagItem *> &array);

    size_t GetNumberOfTags() const;

    const Tags& GetTagsContainer() const { return m_Tags; }

    Tags& GetTagsContainer() { return m_Tags; }

    /** dump tags stored into this array */
    void Print(std::ostream& os, const int tabs = 0) const override;

protected:
    void InternalStore(mafStorageElementBuilder& parent) override;
    void InternalRestore(const mafStorageElement& node) override;

    Tags m_Tags;
};

//-------------------------------------------------------------------------
template <class ArrayPtr, class TType>
inline TType mafRestoreNumericFromTag(ArrayPtr array, const mafString& name, TType& variable, TType unset_value, TType default_value)
//-------------------------------------------------------------------------
{
    if (variable == unset_value)
    {
        if (auto item = array->GetTag(name))
        {
            variable = item->GetValueAsDouble();
        }
        else
        {
            variable = default_value;
        }
    }

    return variable;
}

//-------------------------------------------------------------------------
template <class ArrayPtr, class TType>
inline TType mafRestoreStringFromTag(ArrayPtr array, const mafString& name, TType& variable, const mafString& unset_value, const mafString& default_value)
//-------------------------------------------------------------------------
{
    if (variable == unset_value)
    {
        if (auto item = array->GetTag(name))
        {
            variable = item->GetValue();
        }
        else
        {
            variable = default_value;
        }
    }

    return variable;
}


END_FTK_NAMESPACE