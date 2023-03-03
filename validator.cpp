#include <iostream>
#include <set>
#include "ast.h"
#include "validator.h"
#include "exception.h"
#include "state.h"

namespace AST {

std::vector <std::pair <std::string, int> > variable_stack; // {name, index}
std::vector <Type> variable_type;
int variable_index = 0;
std::vector <int> heap_size;
int heap_index = 0;
std::vector <std::pair <std::string, std::shared_ptr <FunctionSignature> > > function_stack;
int function_index = 0;

std::set <State> states;
std::vector < std::pair <int, int> > states_log;
bool root_block = false;

bool operator < (const State &a, const State &b) {
    return a.heap < b.heap;
}

bool operator == (const State &a, const State &b) {
    return a.heap == b.heap;
}

int getIndex(std::string id, Node *node) {
    for (int i = (int)variable_stack.size() - 1; i >= 0; i--) {
        if (variable_stack[i].first == id) {
            return variable_stack[i].second;
        }
    }
    throw AliasException("Identifier was not declared in this scope", node);
}

Type getType(std::string id, Node *node) {
    for (int i = (int)variable_stack.size() - 1; i >= 0; i--) {
        if (variable_stack[i].first == id) {
            return variable_type[variable_stack[i].second];
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
}

void printStatesLog() {
    std::cout << "States" << std::endl;
    for (std::pair <int, int> p : states_log) {
        std::cout << p.first << ":" << p.second << std::endl;
    }
}

void checkLeak(Node *node) {
    for (State state : states) {
        std::vector <bool> used(heap_index);
        for (std::pair <int,int> p : state.heap) {
            if (p.first != -1) {
                used[p.first] = true;
            }
        }
        for (int i = 0; i < heap_index; i++) {
            if (!used[i] && heap_size[i] != 0) {
                throw AliasException("Memory leak", node);
            }
        }
    }
}

void Block::Validate() {
    bool this_root_block = false;
    if (!root_block) {
        this_root_block = true;
        root_block = true;
        states.insert(State());
    }

    size_t variable_stack_size = variable_stack.size();
    size_t function_stack_size = function_stack.size();

    for (auto i = statement_list.begin(); i != statement_list.end(); i++) {
        (*i)->Validate();
        if (Settings::GetTarget() == Settings::Target::Server)
            states_log.push_back({(*i)->line_begin + 1, (int)states.size()});
    }

    if (this_root_block)
        return;

    std::set <State> _states;
    for (State state : states) {
        for (int i = (int)variable_stack.size() - 1; i >= (int)variable_stack_size; i--) {
            state.heap[i] = {-1, 0};
        }
        _states.insert(state);
    }
    states = _states;
    checkLeak(this);
    while (variable_stack.size() > variable_stack_size)
        variable_stack.pop_back();
    while (function_stack.size() > function_stack_size)
        function_stack.pop_back();
}

void If::Validate() {
    std::set <State> _states = states;
    int n_heap_1 = (int)heap_size.size();
    branch_list[0].second->Validate();
    int n_heap_2 = (int)heap_size.size();
    std::vector <int> _heap_size = heap_size;
    
    for (int i = n_heap_1; i < n_heap_2; i++)
        heap_size[i] = 0;

    std::set <State> _states1 = states;
    states = _states;
    std::set <State> _states2 = states;
    if (else_body) {
        else_body->Validate();
        _states2 = states;
    }

    for (int i = n_heap_1; i < n_heap_2; i++)
        heap_size[i] = _heap_size[i];

    states.clear();
    for (State state : _states1)
        states.insert(state);
    for (State state : _states2)
        states.insert(state);
    
    checkLeak(this);
}

void While::Validate() {
    int n_heap = (int)heap_size.size();
    int cnt = 0;
    while (true) {
        cnt++;
        if (cnt == 100) {
            throw AliasException("Fatal Error: while loop", this);
        }
        std::set <State> _states = states;
        block->Validate();
        if (n_heap != (int)heap_size.size()) {
            throw AliasException("Inexpected allocation in while loop", this);
        }
        bool add = false;
        for (State state : states) {
            if (_states.find(state) == _states.end())
                add = true;
            _states.insert(state);
        }
        if (!add)
            break;
        states = _states;
    }

    checkLeak(this);
}

void FunctionDefinition::Validate() {
    int n = (int)signature->identifiers.size();
    std::vector <int> allocated;
    for (int i = 0; i < n; i++) {
        variable_stack.push_back({signature->identifiers[i], variable_index++});
        variable_type.push_back(signature->types[i]);
        if (signature->types[i] == Type::Ptr) {
            heap_size.push_back(signature->sizes[i]);
            std::set <State> _states;
            for (State state : states) {
                state.heap.push_back({heap_index, 0});
                _states.insert(state);
            }
            states = _states;
            allocated.push_back(heap_index);
            heap_index++;
        }
        else {
            std::set <State> _states;
            for (State state : states) {
                state.heap.push_back({-1, 0});
                _states.insert(state);
            }
            states = _states;
        }
    }

    function_stack.push_back({name, signature});

    body->Validate();

    std::set <State> _states;
    for (State state : states) {
        for (int i = (int)variable_stack.size() - 1; i >= (int)variable_stack.size() - n; i--) {
            state.heap[i] = {-1, 0};
        }
        _states.insert(state);
    }
    states = _states;

    for (int i : allocated) {
        heap_size[i] = 0;
    }

    for (int i = 0; i < n; i++) {
        variable_stack.pop_back();
    }
}

void Definition::Validate() {
    variable_stack.push_back({identifier, variable_index});
    variable_type.push_back(type);
    std::set <State> _states;
    for (State state : states) {
        state.heap.push_back({-1, 0});
        _states.insert(state);
    }
    states = _states;
    variable_index++;
}

void Assignment::Validate() {
    if (auto _alloc = std::dynamic_pointer_cast <AST::Alloc> (value)) {
        int index = getIndex(identifier, this);
        heap_size.push_back(_alloc->size);
        std::set <State> _states;
        for (State state : states) {
            state.heap[index] = {heap_index, 0};
            _states.insert(state);
        }
        states = _states;
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
            std::set <State> _states;
            for (State state : states) {
                if (state.heap[index2].first == -1) {
                    state.heap[index1] = {-1, 0};
                }
                else {
                    state.heap[index1] = {state.heap[index2].first, state.heap[index2].second + _integer->value};
                }
                _states.insert(state);
            }
            states = _states;
        }
        else {
            int index1 = getIndex(identifier, this);
            std::set <State> _states;
            for (State state : states) {
                for (int i = -1; i < heap_index; i++) {
                    for (int j = 0; j < ((i == -1) ? 1 : heap_size[i]); j++) {
                        State _state = state;
                        _state.heap[index1] = {i, j};
                        _states.insert(_state);
                    }
                }
            }
            states = _states;
        }
    }
    else {
        value->Validate();
    }
    checkLeak(this);
}

void Movement::Validate() {
    if (getType(identifier, this) == Type::Ptr) {
        int index = getIndex(identifier, this);
        for (State state : states) {
            if (state.heap[index].first == -1 ||
                state.heap[index].second < 0 ||
                state.heap[index].second >= heap_size[state.heap[index].first]) {
                throw AliasException("Access violation", this);
            }
        }
    }
    else {
        throw AliasException("Pointer expected in left part of movement", this);
    }
    value->Validate();
}

void Assumption::Validate() {
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
        std::set <State> _states;
        for (State state : states) {
            if (state.heap[index1].first == state.heap[index2].first &&
                state.heap[index1].second - state.heap[index2].second == _integer->value) {
                _states.insert(state);
            }
        }
        states = _states;
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
        for (State state : states) {
            if (state.heap[index1].first == state.heap[index2].first &&
                state.heap[index1].second - state.heap[index2].second < _integer->value) {
                _states.insert(state);
            }
        }
        states = _states;
    }
    else {
        throw AliasException("Equation expected in assumption", this);
    }
}

void Identifier::Validate() {
}

void Integer::Validate() {
}

void Alloc::Validate() {
}

void Free::Validate() {
    if (auto _identifier = std::dynamic_pointer_cast <AST::Identifier> (arg)) {
        int ind = getIndex(_identifier->identifier, this);
        int heap_id = -1;
        for (State state : states) {
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
        heap_size[heap_id] = 0;
        std::set <State> _states;
        for (State state : states) {
            for (int i = 0; i < variable_index; i++) {
                if (state.heap[i].first == heap_id) {
                    state.heap[i] = {-1, 0};
                }
            }
            _states.insert(state);
        }
        states = _states;
    }
    else {
        throw AliasException("Identifier expected in free statement", this);
    }
}

void FunctionCall::Validate() {
    std::shared_ptr <FunctionSignature> signature = nullptr;
    for (int i = (int)function_stack.size(); i >= 0; i--) {
        if (function_stack[i].first == identifier) {
            signature = function_stack[i].second;
            break;
        }
    }
    
    if (!signature) {
        throw AliasException("Identifier was not declared in this scope", this);
    }

    if (signature->identifiers.size() != arguments.size()) {
        throw AliasException("Incorrect number of arguments in function call", this);
    }
    
    for (int i = 0; i < (int)signature->identifiers.size(); i++) {
        if (signature->types[i] != getType(arguments[i], this)) {
            throw AliasException("Incorrect type of argument in function call", this);
        }
        if (signature->types[i] == AST::Type::Ptr) {
            int ind = getIndex(arguments[i], this);
            for (State state : states) {
                if (state.heap[ind].first == -1 ||
                    state.heap[ind].second < 0 ||
                    heap_size[state.heap[ind].first] - state.heap[ind].second < signature->sizes[i]) {
                    throw AliasException("Incorrect function call", this);
                }
            }
        }
    }
}

void Dereference::Validate() {
    if (auto _identifier = std::dynamic_pointer_cast <AST::Identifier> (arg)) {
        if (getType(_identifier->identifier, this) == Type::Ptr) {
            int index = getIndex(_identifier->identifier, this);
            for (State state : states) {
                if (state.heap[index].first == -1 ||
                    state.heap[index].second < 0 ||
                    state.heap[index].second >= heap_size[state.heap[index].first]) {
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
    }
}

void Addition::Validate() {
    left->Validate();
    right->Validate();
}

void Less::Validate() {
}

void Equal::Validate() {
}

}
