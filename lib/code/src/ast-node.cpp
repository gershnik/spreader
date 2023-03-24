// Copyright (c) 2022, Eugene Gershnik
// SPDX-License-Identifier: BSD-3-Clause

#include <spreader/ast-node.h>
#include <spreader/cell-grid.h>

#include <formula-parser.h>

#include "execution-context.h"

using namespace Spreader;


void AstNode::reconstructPrefix(const ReconstructionContext & /*context*/, StringBuilder & /*dest*/) const {
}
void AstNode::reconstructSuffix(const ReconstructionContext & /*context*/, StringBuilder & /*dest*/) const {
}

auto AstNode::createExecutionStackEntry(ExecutionMemoryResource & mem, TreeTraversalState state) const -> ExecutionStackEntryPtr {
    return ExecutionStackEntryPtr(new (mem) ExecutionStackEntry(this, state));
}

auto AstNode::createReconstructionStackEntry(TreeTraversalState state) const -> ReconstructionStackEntryPtr {
    return ReconstructionStackEntryPtr(new ReconstructionStackEntry(this, state));
}

void AstNode::destroyChildren() noexcept {
    //early check to prevent recursive traversal of the same tree
    if (m_firstChild || m_nextSibling) {
        AstNode * current = this;
        current->m_destructionState = 0;

        do {
            if (current->m_destructionState == 0) {
                ++current->m_destructionState;
                if (current->m_firstChild) {
                    current = current->m_firstChild.get();
                    current->m_destructionState = 0;
                    continue;
                }
            }
            if (current->m_destructionState == 1) {
                current->m_firstChild.reset();
                ++current->m_destructionState;
                if (current->m_nextSibling) {
                    current = current->m_nextSibling.get();
                    current->m_destructionState = 0;
                    continue;
                }
            }
            current->m_nextSibling.reset();
            current = current->m_parent;
        }
        while(current);
    }

}

//MARK: - ScalarNode

auto ScalarNode::execute(ExecutionContext & context) const -> bool {
    context.returnedValue = m_value;
    context.returnedExtent = Size{1, 1};
    return true;
}

void ScalarNode::reconstructPrefix(const ReconstructionContext & /*context*/, StringBuilder & dest) const {
    m_value.reconstruct(dest);
}

//MARK: - ArrayNode


auto ArrayNode::execute(ExecutionContext & context) const -> bool {
    context.returnedValue = m_value;
    context.returnedExtent = m_value->size();
    return true;
}

void ArrayNode::reconstructPrefix(const ReconstructionContext & /*context*/, StringBuilder & dest) const {
    m_value->reconstruct(dest);
}

//MARK: - ReferenceNode

auto ReferenceNode::execute(ExecutionContext & context) const -> bool {
    
    const auto & ref = context.references()[m_refIndex];
    visit([&context] (auto && value) {

        using RefType = std::remove_cvref_t<decltype(value)>;
        
        if constexpr (std::is_same_v<RefType, IllegalReference>) {
            
            context.returnedValue = Error::InvalidReference;
            context.returnedExtent = Size{1, 1};
            
        } else if constexpr (std::is_same_v<RefType, CellReference>) {
            
            SPR_ASSERT_LOGIC((context.offset == Point{0, 0}));

            context.returnedValue = value.dereference(context.at());
            context.returnedExtent = Size{1, 1};

        } else if constexpr (std::is_same_v<RefType, AreaReference>) {

            Rect rect = value.dereference(context.at());
            context.returnedValue = rect;
            context.returnedExtent = rect.size;
            
        } else if constexpr (std::is_same_v<RefType, ColumnReference>) { 

            auto [start, size] = value.dereference(context.at());
            Rect rect {
                .origin = Point{start, 0},
                .size = Size{size, context.grid().size().height}
            };
            context.returnedValue = rect;
            context.returnedExtent = rect.size;
            
        } else if constexpr (std::is_same_v<RefType, RowReference>) { 
            
            auto [start, size] = value.dereference(context.at());
            Rect rect {
                .origin = Point{0, start},
                .size = Size{context.grid().size().width, size}
            };
            context.returnedValue = rect;
            context.returnedExtent = rect.size;
        }
    }, ref);

    return true;
}

void ReferenceNode::reconstructPrefix(const ReconstructionContext & context, StringBuilder & dest) const {
    const auto & ref = (*context.references)[m_refIndex];
    visit([&](auto && value) {
        
        using RefType = std::remove_cvref_t<decltype(value)>;
        
        if constexpr (std::is_same_v<RefType, IllegalReference>) {
            dest.append(toString(Error::InvalidReference));
        } else {
            value.reconstruct(context.at, dest);
        }
    }, ref);
}


//MARK: - ParseErrorNode

auto ParseErrorNode::execute(ExecutionContext & context) const -> bool {
    context.returnedValue = m_value;
    context.returnedExtent = Size{1 ,1};
    return true;
}

void ParseErrorNode::reconstructPrefix(const ReconstructionContext & /*context*/, StringBuilder & dest) const {
    dest.append(m_originalText);
}


//MARK: - FunctionNode

auto FunctionNode::createExecutionStackEntry(ExecutionMemoryResource & mem, TreeTraversalState state) const -> ExecutionStackEntryPtr {
    return ExecutionStackEntryPtr(new (mem) ExecutionStackEntry(this, state));
}

auto FunctionNode::createReconstructionStackEntry(TreeTraversalState state) const -> ReconstructionStackEntryPtr {
    return ReconstructionStackEntryPtr(new ReconstructionStackEntry(this, state));
}

void FunctionNode::onBeforeArguments(ExecutionContext & /*context*/) const {
}

auto FunctionNode::onAfterArgument(ExecutionContext & /*context*/) const -> TraversalEventOutcome {
    return TraversalEventOutcome::Continue;
}










