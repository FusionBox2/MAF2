#include "ftk/Core/Root.h"

#include "ftk/Core/Node.h"
#include "ftk/IO/StorageElement.h"
#include "mafIndent.h"

namespace model::data
{
    Root::Root() = default;

    Root::~Root() = default;

    Root* Root::SafeDownCast(Node* o)
    {
        return dynamic_cast<Root*>(o);
    }

    void Root::StoreRoot(mafStorageElementBuilder& builder) const
    {
        builder(_R("MaxNodeId")).SetValue(mafToString(m_MaxNodeId));
    }

    void Root::RestoreRoot(const mafStorageElement& value)
    {
        SetMaxNodeId(value(_R("MaxNodeId")).As<mafID>());
    }

    void Root::Print(std::ostream& os, int tabs) const 
    {
        os << mafIndent(tabs) << "MaxNodeId: " << m_MaxNodeId << "\n";
    }

    mafID Root::GetMaxNodeId() const
    {
        return m_MaxNodeId;
    }

    void Root::SetMaxNodeId(mafID id)
    {
        m_MaxNodeId = id;
    }
}