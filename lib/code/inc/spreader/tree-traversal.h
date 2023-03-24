// Copyright (c) 2022, Eugene Gershnik
// SPDX-License-Identifier: BSD-3-Clause

#ifndef SPR_HEADER_TREE_TRAVERSAL_H_INCLUDED
#define SPR_HEADER_TREE_TRAVERSAL_H_INCLUDED

#include <spreader/types.h>

#include <memory>
#include <concepts>

namespace Spreader {

    enum class TreeTraversalState {
        FirstSeen = 0,
        BeforeChild,
        AfterChild,
        BeforeExit
    };

    template<class Node>
    class ExternalTreeTraversalStackEntry {
    public:
        ExternalTreeTraversalStackEntry(Node * n, TreeTraversalState ts) noexcept : 
            node(n), nextChild(nullptr), traversalState(ts) {
        }
        virtual ~ExternalTreeTraversalStackEntry() noexcept {}

        std::unique_ptr<ExternalTreeTraversalStackEntry> prev;
        Node * const node;
        Node * nextChild;
        TreeTraversalState traversalState;
    };

    template<class T, class Node>
    concept TreeNodeAccessor = std::is_class_v<T> && requires(T ac, Node * node) {

        { ac.firstChild(node) } -> std::same_as<Node *>;
        { ac.nextSibling(node) } -> std::same_as<Node *>;
        { ac.createStackEntry(node, TreeTraversalState()) } -> std::same_as<std::unique_ptr<ExternalTreeTraversalStackEntry<Node>>>;
    };

    template<class T, class Node>
    concept IntrusiveTreeNodeAccessor = TreeNodeAccessor<T, Node> && requires(T ac, Node * node) {
        
        { ac.parent(node) } -> std::convertible_to<Node *>;
        { ac.startTraversal(node) };
        { ac.getTraversalState(node) } -> std::same_as<TreeTraversalState>;
        { ac.setTraversalState(node, TreeTraversalState()) };
        { ac.getNextTraversalChild(node) } -> std::same_as<Node *>;
        { ac.setNextTraversalChild(node, node) };
    };

    template<class T>
    concept TreeTraversalExtraState = std::is_void_v<T> || std::is_default_constructible_v<T>;

    enum class TraversalEventOutcome : uint32_t {
        Pause                           = 0,
        Continue                        = 0x10000,
        ContinueAndSkipNextChild        = 0x10001,
        ContinueAndSkipAllChildren      = 0x1FFFF,
        RepeatChild                     = 0x20000,
        //RestartChildren                 = 0x40000
    };
    template<> struct MarkBitwiseEnum<TraversalEventOutcome> { static constexpr bool Value = true; };

    constexpr inline auto makeContinueAndSkipOutcome(uint16_t extraCount) noexcept -> TraversalEventOutcome {
        return static_cast<TraversalEventOutcome>(static_cast<uint32_t>(TraversalEventOutcome::Continue) | extraCount);
    }

    template<class T, class Invoker>
    concept TreeTraversalEventHandler = requires(T op) {
        { op(std::declval<typename Invoker::Enter>()) } -> std::same_as<TraversalEventOutcome>;
        { op(std::declval<typename Invoker::BeforeChild>()) } -> std::same_as<TraversalEventOutcome>;
        { op(std::declval<typename Invoker::AfterChild>()) } -> std::same_as<TraversalEventOutcome>;
        { op(std::declval<typename Invoker::Exit>()) } -> std::same_as<TraversalEventOutcome>;
    };

    template<class T>
    concept TreeTraversalPolicy = requires(T pol, typename T::NodeType & node) {

        typename T::Enter; 
        typename T::BeforeChild; 
        typename T::AfterChild; 
        typename T::Exit; 

        pol.setCurrentState(TreeTraversalState());
        { pol.currentState() } -> std::same_as<TreeTraversalState>;
        { pol.setFirstChildAsNext() } -> std::same_as<bool>;
        { pol.setNextSiblingAsNext() } -> std::same_as<bool>;
        { pol.hasNextChild() } -> std::same_as<bool>;
        pol.descendToNext(TreeTraversalState(), TreeTraversalState());
        { pol.ascend(&node) } -> std::same_as<bool>;
        pol.reset(node, TreeTraversalState());

        { pol.template invoke<typename T::Enter>       ([](auto &&) { return TraversalEventOutcome(); }) } -> std::same_as<TraversalEventOutcome>;
        { pol.template invoke<typename T::BeforeChild> ([](auto &&) { return TraversalEventOutcome(); }) } -> std::same_as<TraversalEventOutcome>;
        { pol.template invoke<typename T::AfterChild>  ([](auto &&) { return TraversalEventOutcome(); }) } -> std::same_as<TraversalEventOutcome>;
        { pol.template invoke<typename T::Exit>        ([](auto &&) { return TraversalEventOutcome(); }) } -> std::same_as<TraversalEventOutcome>;
    };

    template<class Node, TreeNodeAccessor<Node> NodeAccess>
    class ExternalTreeTraversalPolicy : NodeAccess {

    private:
        using StackEntry = ExternalTreeTraversalStackEntry<Node>;
        using StackEntryPtr = std::unique_ptr<StackEntry>;
    public:
        using NodeType = Node;

        struct Enter { StackEntry * stackEntry; };
        struct BeforeChild { StackEntry * stackEntry; };
        struct AfterChild {  StackEntry * stackEntry; };
        struct Exit { StackEntry * stackEntry;};

    public:
        template<class ...Args>
        ExternalTreeTraversalPolicy(Node & root, TreeTraversalState initialState, Args && ...args):
            NodeAccess(std::forward<Args>(args)...) {
            m_current = NodeAccess::createStackEntry(&root, initialState);
        }
        ~ExternalTreeTraversalPolicy() {
            clear();
        }
        ExternalTreeTraversalPolicy(ExternalTreeTraversalPolicy && src) noexcept = default;
        
        auto operator=(ExternalTreeTraversalPolicy && src) noexcept -> ExternalTreeTraversalPolicy & {
            this->~ExternalTreeTraversalPolicy();
            return *new (this) ExternalTreeTraversalPolicy(std::move(src));
        }

        void setCurrentState(TreeTraversalState state) {
            m_current->traversalState = state;
        }

        auto currentState() const -> TreeTraversalState {
            return m_current->traversalState;
        }

        auto setFirstChildAsNext() -> bool {
            m_current->nextChild = NodeAccess::firstChild(m_current->node);
            return m_current->nextChild;
        }

        auto setNextSiblingAsNext() -> bool { 
            m_current->nextChild = NodeAccess::nextSibling(m_current->nextChild);
            return m_current->nextChild;
        }

        auto hasNextChild() -> bool {
            return m_current->nextChild;
        }

        void descendToNext(TreeTraversalState currentState, TreeTraversalState nextState) {
            m_current->traversalState = currentState;
            StackEntryPtr newCurrent = NodeAccess::createStackEntry(m_current->nextChild, nextState);
            swap(newCurrent->prev, m_current);
            swap(m_current, newCurrent);
        }

        auto ascend(NodeType * /*root*/) -> bool {
            StackEntryPtr newCurrent = std::move(m_current->prev);
            swap(newCurrent, m_current);
            return bool(m_current);
        }

        void reset(Node & node, TreeTraversalState initialState) {
            clear();
            m_current = NodeAccess::createStackEntry(&node, initialState);
        }

        template<class Event, TreeTraversalEventHandler<ExternalTreeTraversalPolicy> Handler>
        auto invoke(Handler && handler) -> TraversalEventOutcome {

            return std::forward<Handler>(handler)(Event{m_current.get()});
        }

    private:

        void clear() {
            while(m_current) {
                auto * prev = m_current->prev.release();
                m_current.reset(prev);
            }
        }

    private:
        StackEntryPtr m_current;
    };

    template<class Node, IntrusiveTreeNodeAccessor<Node> NodeAccess>
    class IntrusiveTreeTraversalPolicy : NodeAccess {

    public:
        using NodeType = Node;

        struct Enter { Node * node; };
        struct BeforeChild { 
            BeforeChild(Node * current) :
                node(current),
                child(NodeAccess::getNextTraversalChild(current)) 
            {}
            Node * node; 
            Node * child; 
        };
        struct AfterChild { 
            AfterChild(Node * current) :
                node(current),
                child(NodeAccess::getNextTraversalChild(current)) 
            {}
            Node * node; 
            Node * child; 
        };
        struct Exit { Node * node; };

    public:
        IntrusiveTreeTraversalPolicy(Node & root, TreeTraversalState initialState) {
            reset(root, initialState);
        }

        void setCurrentState(TreeTraversalState state) {
            NodeAccess::setTraversalState(m_current, state);
        }

        auto currentState() const -> TreeTraversalState {
            return NodeAccess::getTraversalState(m_current);
        }

        auto setFirstChildAsNext() -> bool {
            auto firstChild = NodeAccess::firstChild(m_current);
            NodeAccess::setNextTraversalChild(m_current, firstChild);
            return firstChild;
        }

        auto setNextSiblingAsNext() -> bool {
            auto nextSibling = NodeAccess::nextSibling(NodeAccess::getNextTraversalChild(m_current));
            NodeAccess::setNextTraversalChild(m_current, nextSibling);
            return nextSibling;
        }

        auto hasNextChild() -> bool {
            return NodeAccess::getNextTraversalChild(m_current);
        }

        auto descendToNext(TreeTraversalState currentState, TreeTraversalState nextState)  {
            auto nextChild = NodeAccess::getNextTraversalChild(m_current);
            NodeAccess::setTraversalState(m_current, currentState);
            reset(*nextChild, nextState);
        }

        auto ascend(Node * root) -> bool {
            if (m_current != root) {
                m_current = NodeAccess::parent(m_current);
                return true;
            }
            m_current = nullptr;
            return false;
        }

        void reset(Node & node, TreeTraversalState initialState) {
            m_current = &node;
            NodeAccess::startTraversal(m_current);
            NodeAccess::setNextTraversalChild(m_current, nullptr);
            NodeAccess::setTraversalState(m_current, initialState);
        }

        template<class Event, TreeTraversalEventHandler<IntrusiveTreeTraversalPolicy> Handler>
        auto invoke(Handler && handler) -> TraversalEventOutcome {

            return std::forward<Handler>(handler)(Event{m_current});
        }

    private:
        Node * m_current;
    };



    template<TreeTraversalPolicy TraversalPolicy>
    class TreeTraversal : private TraversalPolicy {
    public:
        enum Result {
            Done, 
            Paused
        };

        using typename TraversalPolicy::Enter;
        using typename TraversalPolicy::BeforeChild;
        using typename TraversalPolicy::AfterChild;
        using typename TraversalPolicy::Exit;

        using typename TraversalPolicy::NodeType;

    public:
        template<class ...Args>
        TreeTraversal(NodeType & root, Args && ...args) :
            TraversalPolicy(root, TreeTraversalState::FirstSeen, std::forward<Args>(args)...),
            m_root(&root) {
        }

        template<TreeTraversalEventHandler<TreeTraversal> Handler>
        auto traverse(Handler handler) -> Result {

            for( ; ; ) {
                switch (TraversalPolicy::currentState()) {
                    case TreeTraversalState::FirstSeen: {
                        auto outcome = TraversalPolicy::template invoke<Enter>(handler);
                        SPR_ASSERT_LOGIC(outcome != TraversalEventOutcome::RepeatChild);
                        if (outcome == TraversalEventOutcome::Pause)
                            return Paused;
                        auto hasChild = TraversalPolicy::setFirstChildAsNext();
                        auto skipCount = static_cast<uint32_t>(outcome & (~TraversalEventOutcome::Continue));
                        while (hasChild && skipCount--)
                            hasChild = TraversalPolicy::setNextSiblingAsNext();
                        TraversalPolicy::setCurrentState(TreeTraversalState::BeforeChild);
                    }
                        [[fallthrough]];
                    case TreeTraversalState::BeforeChild: {
                        if (TraversalPolicy::hasNextChild()) {
                            auto outcome = TraversalPolicy::template invoke<BeforeChild>(handler);
                            SPR_ASSERT_LOGIC(outcome == TraversalEventOutcome::Continue ||
                                             outcome == TraversalEventOutcome::Pause);
                            if (outcome == TraversalEventOutcome::Pause)
                                return Paused;
                            TraversalPolicy::descendToNext(TreeTraversalState::AfterChild, TreeTraversalState::FirstSeen);
                            continue;
                        }
                        TraversalPolicy::setCurrentState(TreeTraversalState::BeforeExit);
                    }
                        [[fallthrough]];
                    case TreeTraversalState::BeforeExit: {
                        auto outcome = TraversalPolicy::template invoke<Exit>(handler);
                        SPR_ASSERT_LOGIC(outcome == TraversalEventOutcome::Continue ||
                                         outcome == TraversalEventOutcome::Pause);
                        if (outcome == TraversalEventOutcome::Pause)
                            return Paused;
                        if (!TraversalPolicy::ascend(m_root))
                            return Done;
                        continue;
                    }

                    case TreeTraversalState::AfterChild: {
                        auto outcome = TraversalPolicy::template invoke<AfterChild>(handler);
                        if (outcome == TraversalEventOutcome::Pause)
                            return Paused;
                        if (outcome == TraversalEventOutcome::RepeatChild) {
                            TraversalPolicy::descendToNext(TreeTraversalState::AfterChild, TreeTraversalState::FirstSeen);
                            continue;
                        }
                        // bool hasChild;
                        // if (outcome == TraversalEventOutcome::RestartChildren) {
                        //     hasChild = TraversalPolicy::setFirstChildAsNext();
                        // } else {
                           bool hasChild = TraversalPolicy::setNextSiblingAsNext();
                        // }
                        auto skipCount = static_cast<uint32_t>(outcome & (~TraversalEventOutcome::Continue));
                        while (hasChild && skipCount--)
                            hasChild = TraversalPolicy::setNextSiblingAsNext();
                        TraversalPolicy::setCurrentState(TreeTraversalState::BeforeChild);
                        continue;
                    }
                }
            }
        }

        void reset() {
            TraversalPolicy::reset(*m_root, TreeTraversalState::FirstSeen);
        }
    private:
        NodeType * m_root;
    };

    template<class Node, TreeNodeAccessor<Node> NodeAccess>
    using ExternalTreeTraversal = TreeTraversal<ExternalTreeTraversalPolicy<Node, NodeAccess>>;

    template<class Node, IntrusiveTreeNodeAccessor<Node> NodeAccess>
    using IntrusiveTreeTraversal = TreeTraversal<IntrusiveTreeTraversalPolicy<Node, NodeAccess>>;

}

#endif
