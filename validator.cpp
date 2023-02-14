#include <iostream>
#include <set>
#include "ast.h"
#include "validator.h"
#include "exception.h"

namespace AST {

bool operator < (const State &a, const State &b) {
    return a.heap < b.heap;
}

int getIndex(std::string id, Node *node) {
    for (int i = (int)variable_stack.size() - 1; i >= 0; i--) {
        if (variable_stack[i].first == id) {
            return i;
        }
    }
    throw AliasException("Identifier was not declared in this scope", node);
}

Type getType(std::string id, Node *node) {
    for (int i = (int)variable_stack.size() - 1; i >= 0; i--) {
        if (variable_stack[i].first == id) {
            return variable_type[i];
        }
    }
    throw AliasException("Identifier was not declared in this scope", node);
}

void simplifyStates() {
    std::set <State> _states;
    for (State state : states)
        _states.insert(state);
    states.clear();
    for (State state : _states)
        states.push_back(state);
}

void Block::Validate() {
    if (states.empty())
        states.push_back(State());

    std::cout << "Block begin" << std::endl;

    size_t variable_stack_size = variable_stack.size();
    for (auto i = statement_list.begin(); i != statement_list.end(); i++) {
        (*i)->Validate();
        std::cout << states.size() << std::endl;
    }

    while (variable_stack.size() > variable_stack_size)
        variable_stack.pop_back();

    std::cout << "Block end" << std::endl;
}

void If::Validate() {
    std::cout << "If begin" << std::endl;

    std::vector <State> _states = states;
    branch_list[0].second->Validate();
    std::vector <State> _states1 = states;
    states = _states;
    std::vector <State> _states2 = states;
    if (else_body) {
        else_body->Validate();
        _states2 = states;
    }
    states.clear();
    for (State &state : _states1)
        states.push_back(state);
    for (State &state : _states2)
        states.push_back(state);

    std::cout << "If end" << std::endl;
}

void Definition::Validate() {
    std::cout << "Definition begin" << std::endl;

    variable_stack.push_back({identifier, variable_index});
    variable_type.push_back((Type)type_degree);
    for (State &state : states) {
        state.heap.push_back({-1, 0});
    }
    variable_index++;

    std::cout << "Definition end" << std::endl;
}

void Assignment::Validate() {
    std::cout << "Assignment begin" << std::endl;

    if (auto _alloc = std::dynamic_pointer_cast <AST::Alloc> (value)) {
        int index = getIndex(identifier, this);
        heap_size.push_back(_alloc->size);
        for (State &state : states) {
            state.heap[index] = {heap_index, 0};
        }
        heap_index++;
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
            for (State &state : states) {
                if (state.heap[index2].first == -1){
                    state.heap[index1] = {-1, 0};
                }
                else{
                    state.heap[index1] = {state.heap[index2].first, state.heap[index2].second + _integer->value};
                }
            }
        }
        else {
            int index1 = getIndex(identifier, this);
            std::vector <State> _states;
            for (State &state : states) {
                for (int i = -1; i < heap_index; i++) {
                    for (int j = 0; j < ((i == -1) ? 1 : heap_size[i]); j++) {
                        State _state = state;
                        _state.heap[index1] = {i, j};
                        _states.push_back(_state);
                    }
                }
            }
            states = _states;
        }
    }
    else {
        throw "Assignment error";
    }

    simplifyStates();

    std::cout << "Assignment end" << std::endl;
}

void Assumption::Validate() {
    std::cout << "Assumption begin" << std::endl;

    if (auto _equal = std::dynamic_pointer_cast <AST::Equal> (condition)) {
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
        std::vector <State> _states;
        for (State &state : states) {
            if (state.heap[index1].first == state.heap[index2].first &&
                state.heap[index1].second - state.heap[index2].second == _integer->value) {
                _states.push_back(state);
            }
        }
        states = _states;
    }
    else {
        throw AliasException("Equation expected in assumption", this);
    }

    std::cout << "Assumption end" << std::endl;
}

void Identifier::Validate() {
}

void Integer::Validate() {
}

void Alloc::Validate() {
}

void Free::Validate() {
}

void Addition::Validate() {
}

void Equal::Validate() {
}

}
