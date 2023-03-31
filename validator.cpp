#include <iostream>
#include <set>
#include "ast.h"
#include "validator.h"
#include "exception.h"
#include "state.h"

namespace AST {
std::vector < std::pair <int, int> > states_log;
// CContext Context;

bool operator < (const State &a, const State &b) {
    return a.heap < b.heap;
}

bool operator == (const State &a, const State &b) {
    return a.heap == b.heap;
}

/* int getIndex(std::string id, Node *node) {
    for (int i = (int)Context.variable_stack.size() - 1; i >= 0; i--) {
        if (Context.variable_stack[i].first == id) {
            return Context.variable_stack[i].second;
        }
    }
    throw AliasException("Identifier was not declared in this scope", node);
}

Type getType(std::string id, Node *node) {
    for (int i = (int)Context.variable_stack.size() - 1; i >= 0; i--) {
        if (Context.variable_stack[i].first == id) {
            return Context.variable_type[Context.variable_stack[i].second];
        }
    }
    throw AliasException("Identifier was not declared in this scope", node);
}

void printStates(std::set <State> states) {
    for (State state : states) {
        std::cout << "[ ";
        for (std::pair <int,int> p : state.heap) {
            std::cout << "(" << p.first << ", " << p.second << ") ";
        }
        std::cout << "]" << std::endl;
    }
    std::cout << "____________________" << std::endl;
} */

void PrintStatesLog() {
    std::cout << "States" << std::endl;
    for (std::pair <int, int> p : states_log) {
        std::cout << p.first << ":" << p.second << std::endl;
    }
}

void checkLeak(Node *node, VLContext &context) {
    for (State state : context.states) {
        std::vector <bool> used(context.packet_size.size());
        for (std::pair <int,int> p : state.heap) {
            if (p.first != -1) {
                used[p.first] = true;
            }
        }
        for (int i = 0; i < (int)context.packet_size.size(); i++) {
            if (!used[i] && context.packet_size[i] != 0) {
                throw AliasException("Memory leak", node);
            }
        }
    }
}

void Validate(std::shared_ptr <Node> node) {
    VLContext context;
    node->Validate(context);
}

void Block::Validate(VLContext &context) {
    size_t old_variable_stack_size = context.variable_stack.size();
    size_t old_function_stack_size = context.function_stack.size();

    for (auto i = statement_list.begin(); i != statement_list.end(); i++) {
        (*i)->Validate(context);
        if (Settings::GetTarget() == Settings::Target::Server)
            states_log.push_back({(*i)->line_begin + 1, (int)context.states.size()});
    }

    std::set <State> _states;
    for (State state : context.states) {
        for (int i = 0; i < (int)context.variable_stack.size() - old_function_stack_size; i++) {
            state.heap.pop_back();
        }
        _states.insert(state);
    }
    context.states = _states;

    checkLeak(this, context);
    while (context.variable_stack.size() > old_variable_stack_size)
        context.variable_stack.pop_back();
    while (context.function_stack.size() > old_function_stack_size)
        context.function_stack.pop_back();
}

void Asm::Validate(VLContext &context) {
}

void If::Validate(VLContext &context) {
    /* branch_list[0].first->Validate();
    std::set <State> _states = Context.states;
    int n_heap_1 = (int)Context.heap_size.size();
    branch_list[0].second->Validate();
    int n_heap_2 = (int)Context.heap_size.size();
    std::vector <int> _heap_size = Context.heap_size;
    
    for (int i = n_heap_1; i < n_heap_2; i++)
        Context.heap_size[i] = 0;

    std::set <State> _states1 = Context.states;
    Context.states = _states;
    std::set <State> _states2 = Context.states;
    if (else_body) {
        else_body->Validate();
        _states2 = Context.states;
    }

    for (int i = n_heap_1; i < n_heap_2; i++)
        Context.heap_size[i] = _heap_size[i];

    Context.states.clear();
    for (State state : _states1)
        Context.states.insert(state);
    for (State state : _states2)
        Context.states.insert(state);

    checkLeak(this); */
}

void While::Validate(VLContext &context) {
    /* int n_heap = (int)Context.heap_size.size();
    int cnt = 0;
    while (true) {
        cnt++;
        if (cnt == 100) {
            throw AliasException("Fatal Error: while loop", this);
        }
        std::set <State> _states = Context.states;
        block->Validate();
        if (n_heap != (int)Context.heap_size.size()) {
            throw AliasException("Inexpected allocation in while loop", this);
        }
        bool add = false;
        for (State state : Context.states) {
            if (_states.find(state) == _states.end())
                add = true;
            _states.insert(state);
        }
        if (!add)
            break;
        Context.states = _states;
    }

    checkLeak(this); */
}

void FunctionDefinition::Validate(VLContext &context) {
    /* Context.push();

    int n = (int)signature->identifiers.size();
    State state;
    for (int i = 0; i < 10; i++) {
        if (i < n && signature->types[i] == Type::Ptr) {
            if (signature->size_in[i] == 0)
                state.heap.push_back({-1, 0});
            else {
                state.heap.push_back({Context.heap_index, 0});
                Context.heap_size.push_back(signature->size_in[i]);
                Context.heap_index++;
            }
        }
        else {
            state.heap.push_back({-1, 0});
        }
    }
    Context.states.clear();
    Context.states.insert(state);

    for (int i = 0; i < n; i++) {
        Context.variable_stack.push_back({signature->identifiers[i], Context.variable_index});
        Context.variable_type.push_back(signature->types[i]);
        Context.variable_index++;
    }

    Context.function_stack.push_back({name, signature});
    body->Validate();

    std::vector <int> heap_num(n, -2);
    for (State state : Context.states) {
        for (int i = 0; i < n; i++) {
            if (signature->types[i] == Type::Int) continue;
            if (signature->size_out[i] == 0) {
                heap_num[i] = -1;
                if (state.heap[i].first != -1) {
                    throw AliasException("Function post condition failed", this);
                }
            }
            else {
                if (state.heap[i].first == -1) {
                    throw AliasException("Function post condition failed", this);
                }
                if (heap_num[i] == -2) {
                    heap_num[i] = state.heap[i].first;
                }
                else if (state.heap[i].first != heap_num[i]) {
                    throw AliasException("Function post condition has several heaps", this);
                }
                if (state.heap[i].second < 0 || Context.heap_size[heap_num[i]] - state.heap[i].second < signature->size_out[i]) {
                    throw AliasException("Function post condition failed", this);
                }
            }
        }
    }
    for (int i : heap_num) {
        if (i >= 0) {
            Context.heap_size[i] = 0;
        }
    }
    for (int i : Context.heap_size) {
        if (i != 0) {
            throw AliasException("Memory leak", this);
        }
    }

    Context.pop();
    Context.function_stack.push_back({name, signature}); */
}

void Definition::Validate(VLContext &context) {
    /* Context.variable_stack.push_back({identifier, Context.variable_index});
    Context.variable_type.push_back(type);
    Context.variable_index++; */
}

void Assignment::Validate(VLContext &context) {
    /* if (auto _int = std::dynamic_pointer_cast <AST::Integer> (value)) {
        int index = getIndex(identifier, this);
        std::set <State> _states;
        for (State state : Context.states) {
            state.heap[index] = {-1, 0};
            _states.insert(state);
        }
        Context.states = _states;
    }
    else if (auto _alloc = std::dynamic_pointer_cast <AST::Alloc> (value)) {
        int index = getIndex(identifier, this);
        Context.heap_size.push_back(_alloc->size);
        std::set <State> _states;
        for (State state : Context.states) {
            state.heap[index] = {Context.heap_index, 0};
            _states.insert(state);
        }
        Context.states = _states;
        Context.heap_index++;
    }
    else if (getType(identifier, this) == Type::Ptr) {
        auto _addition = std::dynamic_pointer_cast <AST::Addition> (value);
        if (!_addition) {
            throw AliasException("Addition expected in right part of assignment", this);
        }
        auto _identifier = std::dynamic_pointer_cast <AST::Identifier> (_addition->left);
        auto _integer = std::dynamic_pointer_cast <AST::Integer> (_addition->right);
        if (!_identifier) {
            throw AliasException("Identifier expected in left part of addition in right part of assignment", this);
        }
        if (!_integer) {
            throw AliasException("Integer expected in right part of addition in right part of assignment", this);
        }
        if (getType(_identifier->identifier, this) == Type::Ptr) {
            int index1 = getIndex(identifier, this);
            int index2 = getIndex(_identifier->identifier, this);
            std::set <State> _states;
            for (State state : Context.states) {
                if (state.heap[index2].first == -1) {
                    state.heap[index1] = {-1, 0};
                }
                else {
                    state.heap[index1] = {state.heap[index2].first, state.heap[index2].second + _integer->value};
                }
                _states.insert(state);
            }
            Context.states = _states;
        }
        else {
            int index1 = getIndex(identifier, this);
            std::set <State> _states;
            for (State state : Context.states) {
                for (int i = -1; i < Context.heap_index; i++) {
                    for (int j = 0; j < ((i == -1) ? 1 : Context.heap_size[i]); j++) {
                        State _state = state;
                        _state.heap[index1] = {i, j};
                        _states.insert(_state);
                    }
                }
            }
            Context.states = _states;
        }
    }
    else {
        value->Validate();
    }
    checkLeak(this); */
}

void Movement::Validate(VLContext &context) {
    /* if (getType(identifier, this) == Type::Ptr) {
        int index = getIndex(identifier, this);
        for (State state : Context.states) {
            if (state.heap[index].first == -1 ||
                state.heap[index].second < 0 ||
                state.heap[index].second >= Context.heap_size[state.heap[index].first]) {
                throw AliasException("Access violation", this);
            }
        }
    }
    else {
        throw AliasException("Pointer expected in left part of movement", this);
    }
    value->Validate(); */
}

void Assumption::Validate(VLContext &context) {
    /* if (auto _equal = std::dynamic_pointer_cast <AST::Equal> (condition)) {
        auto _expression1 = _equal -> left;
        auto _expression2 = _equal -> right;
        auto _identifier1 = std::dynamic_pointer_cast <AST::Identifier> (_equal->left);
        auto _addition = std::dynamic_pointer_cast <AST::Addition> (_equal->right);
        if (!_identifier1) {
            throw AliasException("Identifier expected in left part of equality of assumption", this);
        }
        if (!_addition) {
            throw AliasException("Addition expected in right part of equality of assumption", this);
        }
        auto _identifier2 = std::dynamic_pointer_cast <AST::Identifier> (_addition->left);
        auto _integer = std::dynamic_pointer_cast <AST::Integer> (_addition->right);
        if (!_identifier2) {
            throw AliasException("Identifier expected in left part of addition in right part of equality of assumption", this);
        }
        if (!_integer) {
            throw AliasException("Integer expected in right part of addition in right part of equality of assumption", this);
        }

        int index1 = getIndex(_identifier1->identifier, this);
        int index2 = getIndex(_identifier2->identifier, this);
        std::set <State> _states;
        for (State state : Context.states) {
            if (state.heap[index1].first == state.heap[index2].first &&
                state.heap[index1].second - state.heap[index2].second == _integer->value) {
                _states.insert(state);
            }
        }
        Context.states = _states;
    }
    else if (auto _less = std::dynamic_pointer_cast <AST::Less> (condition)) {
        auto _expression1 = _less -> left;
        auto _expression2 = _less -> right;
        auto _identifier1 = std::dynamic_pointer_cast <AST::Identifier> (_less->left);
        auto _addition = std::dynamic_pointer_cast <AST::Addition> (_less->right);
        if (!_identifier1) {
            throw AliasException("Identifier expected in left part of less of assumption", this);
        }
        if (!_addition) {
            throw AliasException("Addition expected in right part of less of assumption", this);
        }
        auto _identifier2 = std::dynamic_pointer_cast <AST::Identifier> (_addition->left);
        auto _integer = std::dynamic_pointer_cast <AST::Integer> (_addition->right);
        if (!_identifier2) {
            throw AliasException("Identifier expected in left part of addition in right part of less of assumption", this);
        }
        if (!_integer) {
            throw AliasException("Integer expected in right part of addition in right part of less of assumption", this);
        }

        int index1 = getIndex(_identifier1->identifier, this);
        int index2 = getIndex(_identifier2->identifier, this);
        std::set <State> _states;
        for (State state : Context.states) {
            if (state.heap[index1].first == state.heap[index2].first &&
                state.heap[index1].second - state.heap[index2].second < _integer->value) {
                _states.insert(state);
            }
        }
        Context.states = _states;
    }
    else {
        throw AliasException("Equation expected in assumption", this);
    } */
}

void Identifier::Validate(VLContext &context) {
}

void Integer::Validate(VLContext &context) {
}

void Alloc::Validate(VLContext &context) {
}

void Free::Validate(VLContext &context) {
    /* if (auto _identifier = std::dynamic_pointer_cast <AST::Identifier> (arg)) {
        int ind = getIndex(_identifier->identifier, this);
        int heap_id = -1;
        for (State state : Context.states) {
            if (state.heap[ind].first == -1 || state.heap[ind].second != 0) {
                throw AliasException("Access violation", this);
            }
            if (heap_id == -1) {
                heap_id = state.heap[ind].first;
            }
            else if (heap_id != state.heap[ind].first) {
                throw AliasException("Unpredictible free", this);
            }
        }
        Context.heap_size[heap_id] = 0;
        std::set <State> _states;
        for (State state : Context.states) {
            for (int i = 0; i < Context.variable_index; i++) {
                if (state.heap[i].first == heap_id) {
                    state.heap[i] = {-1, 0};
                }
            }
            _states.insert(state);
        }
        Context.states = _states;
    }
    else {
        throw AliasException("Identifier expected in free statement", this);
    } */
}

void FunctionCall::Validate(VLContext &context) {
    /* std::shared_ptr <FunctionSignature> signature = nullptr;
    for (int i = (int)Context.function_stack.size() - 1; i >= 0; i--) {
        if (Context.function_stack[i].first == identifier) {
            signature = Context.function_stack[i].second;
            break;
        }
    }
    
    if (!signature) {
        throw AliasException("Identifier was not declared in this scope", this);
    }

    if (signature->identifiers.size() != arguments.size()) {
        throw AliasException("Incorrect number of arguments in function call", this);
    }
    
    int n = (int)signature->identifiers.size();
    std::vector <int> heap_num(n, -2);

    for (int i = 0; i < n; i++) {
        if (signature->types[i] != getType(arguments[i], this)) {
            throw AliasException("Incorrect type of argument in function call", this);
        }
        if (signature->types[i] == AST::Type::Ptr) {
            int idx = getIndex(arguments[i], this);
            for (State state : Context.states) {
                if (signature->size_in[i] == 0) {
                    heap_num[i] = -1;
                    if (state.heap[idx].first != -1) {
                        throw AliasException("Function pre condition failed", this);
                    }
                }
                else {
                    if (state.heap[idx].first == -1) {
                        throw AliasException("Function pre condition failed", this);
                    }
                    if (heap_num[i] == -2) {
                        heap_num[i] = state.heap[idx].first;
                    }
                    else if (state.heap[idx].first != heap_num[i]) {
                        throw AliasException("Function pre condition has several heaps", this);
                    }
                    if (state.heap[idx].second < 0 || Context.heap_size[heap_num[i]] - state.heap[idx].second < signature->size_in[i]) {
                        throw AliasException("Function pre condition failed", this);
                    }
                }
            }
        }
    }

    std::vector <int> new_heap(n);
    for (int i = 0; i < n; i++) {
        if (signature->types[i] == Type::Ptr) {
            if (heap_num[i] >= 0) {
                Context.heap_size[heap_num[i]] = 0;
            }
            new_heap[i] = Context.heap_index;
            Context.heap_size.push_back(signature->size_out[i]);
            Context.heap_index++;
        }
    }

    std::set <State> _states;
    for (State state : Context.states) {
        for (int i = 0; i < n; i++) {
            if (getType(arguments[i], this) == Type::Ptr) {
                int idx = getIndex(arguments[i], this);
                for (int j = 0; j < (int)state.heap.size(); j++) {
                    if (j != idx && heap_num[i] >= 0 && state.heap[j].first == heap_num[i]) {
                        state.heap[j] = {-1, 0};
                    }
                }
                if (signature->size_out[i] == 0) {
                    state.heap[i] = {-1, 0};
                }
                else {
                    state.heap[idx] = {new_heap[i], 0};
                }
            }
        }
        _states.insert(state);
    }
    Context.states = _states; */
}

void Dereference::Validate(VLContext &context) {
    /* if (auto _identifier = std::dynamic_pointer_cast <AST::Identifier> (arg)) {
        if (getType(_identifier->identifier, this) == Type::Ptr) {
            int index = getIndex(_identifier->identifier, this);
            for (State state : Context.states) {
                if (state.heap[index].first == -1 ||
                    state.heap[index].second < 0 ||
                    state.heap[index].second >= Context.heap_size[state.heap[index].first]) {
                    throw AliasException("Access violation", this);
                }
            }
        }
        else {
            throw AliasException("Dereference operator has to be applied to pointer", this);
        }
    }
    else {
        throw AliasException("Identifier expected after dereference operator", this);
    } */
}

void Addition::Validate(VLContext &context) {
    /* left->Validate();
    right->Validate(); */
}

void Less::Validate(VLContext &context) {
}

void Equal::Validate(VLContext &context) {
}

}
