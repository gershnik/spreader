// Copyright (c) 2022, Eugene Gershnik
// SPDX-License-Identifier: BSD-3-Clause

#ifndef SPR_HEADER_AST_NODE_H_INCLUDED
#define SPR_HEADER_AST_NODE_H_INCLUDED

#include <spreader/scalar-generator.h>
#include <spreader/scalar.h>
#include <spreader/array.h>
#include <spreader/reference.h>
#include <spreader/tree-traversal.h>
#include <spreader/stack-memory-resource.h>
#include <spreader/formula-references.h>

#include <deque>
#include <numeric>

namespace Spreader {

    class CellGrid;
    class FormulaCell;
    struct ExecutionContext;

    class AstNode 
    {
    friend std::default_delete<AstNode>;
    friend class ArgumentList;

    public:
        using ExecutionMemoryResource = StackMemoryResource<4096>;

        template <class T, class MemRes>
        struct CustomAllocStackEntry : ExternalTreeTraversalStackEntry<T> {
            using ExternalTreeTraversalStackEntry<T>::ExternalTreeTraversalStackEntry;

        private:
            struct AllocationRecord {
                MemRes * memory;
                alignas(alignof(std::max_align_t)) std::byte data[1];
            };

        public:
            void * operator new(size_t size) = delete;

            void * operator new(size_t size, MemRes & memory) {
                auto ret = static_cast<AllocationRecord *>(memory.allocate(offsetof(AllocationRecord, data) + size,
                                                                           alignof(AllocationRecord)));
                ret->memory = &memory;
                return ret->data;
            }
            void operator delete(void * p, size_t size, MemRes & memory) {
                auto rec = reinterpret_cast<AllocationRecord *>(static_cast<std::byte *>(p) - offsetof(AllocationRecord, data));
                memory.deallocate(rec, offsetof(AllocationRecord, data) + size, alignof(AllocationRecord));
            }
            void operator delete(void * p, size_t size) {
                auto rec = reinterpret_cast<AllocationRecord *>(static_cast<std::byte *>(p) - offsetof(AllocationRecord, data));
                rec->memory->deallocate(rec, offsetof(AllocationRecord, data) + size, alignof(AllocationRecord));
            }
        };

        using ExecutionStackEntry = CustomAllocStackEntry<const AstNode, ExecutionMemoryResource>;
        using ExecutionStackEntryPtr = std::unique_ptr<ExternalTreeTraversalStackEntry<const AstNode>>;


        using ReconstructionStackEntry = ExternalTreeTraversalStackEntry<const AstNode>;
        using ReconstructionStackEntryPtr = std::unique_ptr<ReconstructionStackEntry>;


        struct TraversalAccessBase {
            SPR_ALWAYS_INLINE static auto firstChild(AstNode * node) noexcept -> AstNode *
                { return node->m_firstChild.get(); }
            SPR_ALWAYS_INLINE static auto firstChild(const AstNode * node) noexcept -> const AstNode *
                { return node->m_firstChild.get(); }
            SPR_ALWAYS_INLINE static auto nextSibling(AstNode * node) noexcept -> AstNode *
                { return node->m_nextSibling.get(); }
            SPR_ALWAYS_INLINE static auto nextSibling(const AstNode * node) noexcept -> const AstNode *
                { return node->m_nextSibling.get(); }
        };

        class ExecutionTraversalAccess : public TraversalAccessBase {
        public:
            ExecutionTraversalAccess(ExecutionMemoryResource & mem) : m_memory(&mem)
            {}

            SPR_ALWAYS_INLINE auto createStackEntry(const AstNode * node, TreeTraversalState state) -> ExecutionStackEntryPtr {
                return node->createExecutionStackEntry(*m_memory, state);
            }

        private:
            ExecutionMemoryResource * m_memory;
        };

        struct ReconstructionTraversalAccess : TraversalAccessBase {
            SPR_ALWAYS_INLINE static auto createStackEntry(const AstNode * node, TreeTraversalState state) -> ReconstructionStackEntryPtr {
                return node->createReconstructionStackEntry(state);
            }
        };
        
        struct ReconstructionContext {
            ConstFormulaReferencesPtr references;
            Point at;
        };

    protected:
        AstNode() noexcept = default;
        AstNode(std::unique_ptr<AstNode> && firstChild) noexcept :
            m_firstChild(std::move(firstChild)) 
        {}
        virtual ~AstNode() noexcept {
        }
        AstNode(const AstNode &) = delete;
        AstNode(AstNode &&) = delete;
        AstNode & operator=(const AstNode &) = delete;
        AstNode & operator=(AstNode &&) = delete;

    public:
        SPR_ALWAYS_INLINE void setBracketed(bool val) noexcept 
            { m_bracketed = val; }
        SPR_ALWAYS_INLINE auto isBracketed() const noexcept -> bool
            { return m_bracketed; }

        virtual auto execute(ExecutionContext & context) const -> bool = 0;
        
        virtual void reconstructPrefix(const ReconstructionContext & context, StringBuilder & dest) const;
        virtual void reconstructSuffix(const ReconstructionContext & context, StringBuilder & dest) const;

    protected:
        void destroyChildren() noexcept;

        virtual auto createExecutionStackEntry(ExecutionMemoryResource & mem, TreeTraversalState state) const -> ExecutionStackEntryPtr;
        virtual auto createReconstructionStackEntry(TreeTraversalState state) const -> ReconstructionStackEntryPtr;

    private:
        //Structure
        AstNode * m_parent = nullptr;  //not a real parent, this is parent in a binary tree
        std::unique_ptr<AstNode> m_firstChild;
        std::unique_ptr<AstNode> m_nextSibling;

        //bit flags
        mutable unsigned m_destructionState:2; //for binary tree traversal on destruction
        mutable unsigned m_bracketed:1 = 0;
    };
    
    using AstNodePtr = std::unique_ptr<AstNode>;
    using ConstAstNodePtr = std::unique_ptr<const AstNode>;
    
    class ScalarNode final : public AstNode {

    public:
        ScalarNode(const Scalar & val): m_value(val) {
        }
        ScalarNode(Scalar && val) noexcept : m_value(std::move(val)) {
        }

        auto execute(ExecutionContext & context) const -> bool override;
        
        void reconstructPrefix(const ReconstructionContext & context, StringBuilder & dest) const override;
    private:
        ~ScalarNode() noexcept = default;

    private:
        const Scalar m_value;
    };

    class ArrayNode final : public AstNode {

    public:
        ArrayNode(const ArrayPtr & val): m_value(val) {
        }
        ArrayNode(ArrayPtr && val) noexcept : m_value(std::move(val)) {
        }

        auto execute(ExecutionContext & context) const -> bool override;
        
        void reconstructPrefix(const ReconstructionContext & context, StringBuilder & dest) const override;
    private:
        ~ArrayNode() noexcept = default;

    private:
        const ArrayPtr m_value;
    };

    class ReferenceNode final : public AstNode {

    public:
        ReferenceNode(size_t refIndex) noexcept : m_refIndex(refIndex) 
        {}

        auto execute(ExecutionContext & context) const -> bool override;
        
        void reconstructPrefix(const ReconstructionContext & context, StringBuilder & dest) const override;
    private:
        ~ReferenceNode() noexcept = default;

    private:
        const size_t m_refIndex;
    };
    
    class ParseErrorNode final : public AstNode {

    public:
        ParseErrorNode(Error err, const String & original):
            m_value(err),
            m_originalText(original) {
        }

        auto execute(ExecutionContext & context) const -> bool override;
        
        void reconstructPrefix(const ReconstructionContext & context, StringBuilder & dest) const override;
    private:
        ~ParseErrorNode() noexcept = default;

    private:
        Error m_value;
        String m_originalText;
    };

    class ArgumentList {

    public:
        ArgumentList() noexcept = default;
        ArgumentList(AstNodePtr && single) noexcept :
            m_count(1),
            m_first(std::move(single)),
            m_last(m_first.get())
        {}
        ArgumentList(AstNodePtr && first, AstNodePtr && second) noexcept :
            m_count(2),
            m_first(std::move(first)),
            m_last(second.get()) {
            
            m_first->m_nextSibling = std::move(second);
            m_last->m_parent = m_first.get();
        }
        
        auto size() const noexcept -> uint16_t { return m_count; }
        

        void add(AstNodePtr && node) noexcept {
            auto newLast = node.get();
            if (m_last) {
                node->m_parent = m_last;
                m_last->m_nextSibling = std::move(node);
            } else {
                m_first = std::move(node);
            }
            m_last = newLast;
            ++m_count;
        }

        void add(ArgumentList && list) noexcept {
            if (m_last) {
                if (list.m_first)
                    list.m_first->m_parent = m_last;
                m_last->m_nextSibling = std::move(list.m_first);
            } else {
                m_first = std::move(list.m_first);
            }
            m_last = list.m_last;
            m_count += list.m_count;
        }

        auto build() noexcept -> AstNodePtr {
            return AstNodePtr(std::move(m_first));
        }
    private:
        uint16_t m_count = 0;
        AstNodePtr m_first;
        AstNode * m_last = nullptr;
    };

    class FunctionNode : public AstNode {
    friend std::default_delete<FunctionNode>;

    public:
        struct ExecutionStackEntry : public AstNode::ExecutionStackEntry {
            using AstNode::ExecutionStackEntry::ExecutionStackEntry;

            uint16_t handledChildIdx = 0;
        };

        struct ReconstructionStackEntry : public AstNode::ReconstructionStackEntry {
            using AstNode::ReconstructionStackEntry::ExternalTreeTraversalStackEntry;

            uint16_t handledChildIdx = 0;
        };
    public:
        auto createExecutionStackEntry(ExecutionMemoryResource & mem, TreeTraversalState state) const 
            -> ExecutionStackEntryPtr override;
        virtual void onBeforeArguments(ExecutionContext & context) const;
        virtual auto onAfterArgument(ExecutionContext & context) const -> TraversalEventOutcome;

        auto createReconstructionStackEntry(TreeTraversalState state) const -> std::unique_ptr<AstNode::ReconstructionStackEntry> override;
        virtual void reconstructAfterChild(const ReconstructionContext & context, uint16_t idx, StringBuilder & dest) const = 0;

        auto childrenCount() const noexcept -> uint16_t {
            return m_childrenCount;
        }
    protected:
        FunctionNode() noexcept = default;
        ~FunctionNode() noexcept {
            destroyChildren();
        }

        FunctionNode(ArgumentList && args) noexcept:
            AstNode(args.build()),
            m_childrenCount(args.size()) {

        }
    private:
        const uint16_t m_childrenCount = 0;
    };

    typedef std::unique_ptr<FunctionNode> FunctionNodePtr;
    typedef std::unique_ptr<const FunctionNode> ConstFunctionNodePtr;
    
    
    using AstExecutionTraversal = ExternalTreeTraversal<const AstNode, AstNode::ExecutionTraversalAccess>;
    using AstReconstructionTraversal = ExternalTreeTraversal<const AstNode, AstNode::ReconstructionTraversalAccess>;
    
    static_assert(std::is_move_assignable_v<AstExecutionTraversal> && std::is_move_constructible_v<AstExecutionTraversal>,
                  "AstExecutionTraversal must be movable");
}

#endif 
