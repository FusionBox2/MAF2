#pragma once

#include "ftkConfigure.h"

#include "mafDefines.h"
#include "mafEventSender.h"

BEGIN_FTK_NAMESPACE

class mafStorageElement;
class mafStorageElementBuilder;

namespace model::data
{
    class Node;

    class FTK_CORE_EXPORT Root : public mafEventSender
    {
    public:

        static Root* SafeDownCast(Node* o);

        Root(const Root&) = delete;

        Root& operator=(const Root&) = delete;

        mafID GetMaxNodeId() const;

        void SetMaxNodeId(mafID id = 0);

        void Print(std::ostream& os, int tabs = 0) const;

    protected:

        Root();

        ~Root() override;

        void StoreRoot(mafStorageElementBuilder& builder) const;

        void RestoreRoot(const mafStorageElement& value);

    private:

        mafID m_MaxNodeId = 0;
    };
}

END_FTK_NAMESPACE
