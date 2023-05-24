#include <iostream>
#include <set>
#include <map>
#include "ast.h"
#include "validator.h"
#include "exception.h"

namespace AST {
std::map < std::string, std::vector < std::pair <int, int> > > states_log;

bool operator < (const State &a, const State &b) {
    return a.heap < b.heap;
}

bool operator == (const State &a, const State &b) {
    return a.heap == b.heap;
}

int getVariableIndex(std::string id, Node *node, VLContext &context) {
    for (int i = (int)context.variable_stack.size() - 1; i >= 0; i--) {
        if (context.variable_stack[i] == id) {
            return i;
        }
    }
    throw AliasException("Identifier was not declared in this scope", node);
}

Type getVariableType(std::string id, Node *node, VLContext &context) {
    for (int i = (int)context.variable_stack.size() - 1; i >= 0; i--) {
        if (context.variable_stack[i] == id) {
            return context.variable_type_stack[i];
        }
    }
    throw AliasException("Identifier was not declared in this scope", node);
}

std::shared_ptr <FunctionSignature> getFunctionSignature(std::string id, Node *node, VLContext &context) {
    for (int i = (int)context.function_stack.size() - 1; i >= 0; i--) {
        if (context.function_stack[i] == id) {
            return context.function_signature_stack[i];
        }
    }
    throw AliasException("Identifier was not declared in this scope", node);
}

void printStates(std::set <State> states) {
    std::cout << "{" << std::endl;
    for (State state : states) {
        std::cout << "[ ";
        for (std::pair <int,int> p : state.heap) {
            std::cout << "(" << p.first << ", " << p.second << ") ";
        }
        std::cout << "]" << std::endl;
    }
    std::cout << "}" << std::endl;
    std::cout << "____________________" << std::endl;
}

void PrintStatesLog() {
    std::cout << "States" << std::endl;
    for (auto v : states_log) {
        std::cout << v.first << std::endl;
        std::cout << (int)v.second.size() << std::endl;
        for (std::pair <int, int> p : v.second) {
            std::cout << p.first << ":" << p.second << std::endl;
        }
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
    context.states.insert(State());
    node->Validate(context);
}

void Block::Validate(VLContext &context) {
    size_t old_variable_stack_size = context.variable_stack.size();
    size_t old_function_stack_size = context.function_stack.size();

    for (auto i = statement_list.begin(); i != statement_list.end(); i++) {
        (*i)->Validate(context);
        states_log[(*i)->filename].push_back({(*i)->line_begin + 1, (int)context.states.size()});
    }

    std::set <State> _states;
    for (State state : context.states) {
        for (int i = 0; i < (int)context.variable_stack.size() - old_variable_stack_size; i++) {
            state.heap.pop_back();
        }
        _states.insert(state);
    }
    context.states = _states;
    checkLeak(this, context);
    while (context.variable_stack.size() > old_variable_stack_size) {
        context.variable_stack.pop_back();
        context.variable_type_stack.pop_back();
    }
    while (context.function_stack.size() > old_function_stack_size) {
        context.function_stack.pop_back();
        context.function_signature_stack.pop_back();
    }
}

void Asm::Validate(VLContext &context) {
}

void If::Validate(VLContext &context) {
    branch_list[0].first->Validate(context);
    std::set <State> _states = context.states;
    int old_cnt_packets1 = (int)context.packet_size.size();
    branch_list[0].second->Validate(context);
    int old_cnt_packets2 = (int)context.packet_size.size();
    std::vector <int> _packet_size = context.packet_size;
    
    for (int i = old_cnt_packets1; i < old_cnt_packets2; i++)
        context.packet_size[i] = 0;

    std::set <State> _states1 = context.states;
    context.states = _states;
    std::set <State> _states2 = context.states;
    if (else_body) {
        else_body->Validate(context);
        _states2 = context.states;
    }

    for (int i = old_cnt_packets1; i < old_cnt_packets2; i++)
        context.packet_size[i] = _packet_size[i];

    context.states.clear();
    for (State state : _states1)
        context.states.insert(state);
    for (State state : _states2)
        context.states.insert(state);

    checkLeak(this, context);
}

void While::Validate(VLContext &context) {
    int n_heap = (int)context.packet_size.size();
    int cnt = 0;
    while (true) {
        cnt++;
        if (cnt == 100) {
            throw AliasException("While loop check limit exceeded", this);
        }
        std::set <State> _states = context.states;
        block->Validate(context);
        if (n_heap != (int)context.packet_size.size()) {
            throw AliasException("Inexpected allocation in while loop", this);
        }
        bool add = false;
        for (State state : context.states) {
            if (_states.find(state) == _states.end())
                add = true;
            _states.insert(state);
        }
        if (!add)
            break;
        context.states = _states;
    }

    checkLeak(this, context);
}

void FunctionDefinition::Validate(VLContext &context) {
    VLContext _context = context;
    context = VLContext();
    context.function_stack = _context.function_stack;
    context.function_signature_stack = _context.function_signature_stack;

    int n = (int)signature->identifiers.size();
    State state;
    for (int i = 0; i < n; i++) {
        if (signature->types[i] == Type::Ptr) {
            if (signature->size_in[i] == 0)
                state.heap.push_back({-1, 0});
            else {
                state.heap.push_back({(int)context.packet_size.size(), 0});
                context.packet_size.push_back(signature->size_in[i]);
            }
        }
        else {
            state.heap.push_back({-1, 0});
        }
    }
    context.states.insert(state);

    for (int i = 0; i < n; i++) {
        context.variable_stack.push_back(signature->identifiers[i]);
        context.variable_type_stack.push_back(signature->types[i]);
    }

    context.function_stack.push_back(name);
    context.function_signature_stack.push_back(signature);
    body->Validate(context);

    std::vector <int> packet_num(n, -2);
    for (State state : context.states) {
        for (int i = 0; i < n; i++) {
            if (signature->types[i] == Type::Int) continue;
            if (signature->size_out[i] == 0) {
                packet_num[i] = -1;
                if (state.heap[i].first != -1) {
                    throw AliasException("Function post condition failed", this);
                }
            }
            else {
                if (state.heap[i].first == -1 || state.heap[i].second != 0 || context.packet_size[state.heap[i].second] < signature->size_out[i]) {
                    throw AliasException("Function post condition failed", this);
                }
                if (packet_num[i] == -2) {
                    packet_num[i] = state.heap[i].first;
                }
                else if (state.heap[i].first != packet_num[i]) {
                    throw AliasException("Function post condition has several packets", this);
                }
            }
        }
    }
    for (int i : packet_num) {
        if (i >= 0) {
            context.packet_size[i] = 0;
        }
    }
    for (int i : context.packet_size) {
        if (i != 0) {
            throw AliasException("Memory leak", this);
        }
    }

    context = _context;
    context.function_stack.push_back(name);
    context.function_signature_stack.push_back(signature);
}

void Prototype::Validate(VLContext &context) {
    context.function_stack.push_back(name);
    context.function_signature_stack.push_back(signature);
}

void Definition::Validate(VLContext &context) {
    context.variable_stack.push_back(identifier);
    context.variable_type_stack.push_back(type);

    std::set <State> _states;
    for (State state : context.states) {
        state.heap.push_back({-1, 0});
        _states.insert(state);
    }
    context.states = _states;
}

void Assignment::Validate(VLContext &context) {
    if (getVariableType(identifier, this, context) == Type::Ptr) {
        if (auto _alloc = std::dynamic_pointer_cast <AST::Alloc> (value)) {
            int index = getVariableIndex(identifier, this, context);
            std::set <State> _states;
            for (State state : context.states) {
                state.heap[index] = {(int)context.packet_size.size(), 0};
                _states.insert(state);
            }
            context.states = _states;
            context.packet_size.push_back(_alloc->size);
        }
        else if (auto _addition = std::dynamic_pointer_cast <AST::Addition> (value)) {
            auto _identifier = std::dynamic_pointer_cast <AST::Identifier> (_addition->left);
            auto _integer = std::dynamic_pointer_cast <AST::Integer> (_addition->right);
            if (!_identifier) {
                throw AliasException("Identifier expected in left part of addition in right part of assignment", this);
            }
            if (!_integer) {
                throw AliasException("Integer expected in right part of addition in right part of assignment", this);
            }
            if (getVariableType(_identifier->identifier, this, context) == Type::Ptr) {
                int index1 = getVariableIndex(identifier, this, context);
                int index2 = getVariableIndex(_identifier->identifier, this, context);
                std::set <State> _states;
                for (State state : context.states) {
                    if (state.heap[index2].first == -1) {
                        state.heap[index1] = {-1, 0};
                    }
                    else {
                        state.heap[index1] = {state.heap[index2].first, state.heap[index2].second + _integer->value};
                    }
                    _states.insert(state);
                }
                context.states = _states;
            }
            else {
                int index = getVariableIndex(identifier, this, context);
                std::set <State> _states;
                for (State state : context.states) {
                    for (int i = -1; i < (int)context.packet_size.size(); i++) {
                        for (int j = 0; j < ((i == -1) ? 1 : context.packet_size[i]); j++) {
                            State _state = state;
                            _state.heap[index] = {i, j};
                            _states.insert(_state);
                        }
                    }
                }
                context.states = _states;
            }
        }
        else {
            int index = getVariableIndex(identifier, this, context);
            std::set <State> _states;
            for (State state : context.states) {
                state.heap[index] = {-1, 0};
                _states.insert(state);
            }
            context.states = _states;
        }
        checkLeak(this, context);
    }
    else {
        value->Validate(context);
    }
}

void Movement::Validate(VLContext &context) {
    if (getVariableType(identifier, this, context) == Type::Ptr) {
        int index = getVariableIndex(identifier, this, context);
        for (State state : context.states) {
            if (state.heap[index].first == -1 ||
                state.heap[index].second < 0 ||
                state.heap[index].second >= context.packet_size[state.heap[index].first]) {
                throw AliasException("Access violation", this);
            }
        }
    }
    else {
        throw AliasException("Pointer expected in left part of movement", this);
    }
    value->Validate(context);
}

void Assumption::Validate(VLContext &context) {
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

        int index1 = getVariableIndex(_identifier1->identifier, this, context);
        int index2 = getVariableIndex(_identifier2->identifier, this, context);
        std::set <State> _states;
        for (State state : context.states) {
            if (state.heap[index1].first == state.heap[index2].first &&
                state.heap[index1].second - state.heap[index2].second == _integer->value) {
                _states.insert(state);
            }
        }
        context.states = _states;
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

        int index1 = getVariableIndex(_identifier1->identifier, this, context);
        int index2 = getVariableIndex(_identifier2->identifier, this, context);
        std::set <State> _states;
        for (State state : context.states) {
            if (state.heap[index1].first == state.heap[index2].first &&
                state.heap[index1].second - state.heap[index2].second < _integer->value) {
                _states.insert(state);
            }
        }
        context.states = _states;
    }
    else {
        throw AliasException("Equation expected in assumption", this);
    }
}

void Identifier::Validate(VLContext &context) {
    getVariableIndex(identifier, this, context);
}

void Integer::Validate(VLContext &context) {
}

void Alloc::Validate(VLContext &context) {
    throw AliasException("Unexpected alloc outside assignment", this);
}

void Free::Validate(VLContext &context) {
    if (auto _identifier = std::dynamic_pointer_cast <AST::Identifier> (arg)) {
        int index = getVariableIndex(_identifier->identifier, this, context);
        int packet_id = -1;
        for (State state : context.states) {
            if (state.heap[index].first == -1 || state.heap[index].second != 0) {
                throw AliasException("Access violation", this);
            }
            if (packet_id == -1) {
                packet_id = state.heap[index].first;
            }
            if (packet_id != state.heap[index].first) {
                throw AliasException("Unpredictible free", this);
            }
        }
        context.packet_size[packet_id] = 0;
        std::set <State> _states;
        for (State state : context.states) {
            for (int i = 0; i < (int)state.heap.size(); i++) {
                if (state.heap[i].first == packet_id) {
                    state.heap[i] = {-1, 0};
                }
            }
            _states.insert(state);
        }
        context.states = _states;
    }
    else {
        throw AliasException("Identifier expected in free statement", this);
    }
}

void FunctionCall::Validate(VLContext &context) {
    std::shared_ptr <FunctionSignature> signature = getFunctionSignature(identifier, this, context);

    if (signature->identifiers.size() != arguments.size()) {
        throw AliasException("Incorrect number of arguments in function call", this);
    }
    
    int n = (int)signature->identifiers.size();
    std::vector <int> packet_num(n, -2);

    for (int i = 0; i < n; i++) {
        if (signature->types[i] != getVariableType(arguments[i], this, context)) {
            throw AliasException("Incorrect type of argument in function call", this);
        }
        if (signature->types[i] == AST::Type::Ptr) {
            int index = getVariableIndex(arguments[i], this, context);
            for (State state : context.states) {
                if (signature->size_in[i] == 0) {
                    packet_num[i] = -1;
                    if (state.heap[index].first != -1) {
                        throw AliasException("Function pre condition failed", this);
                    }
                }
                else {
                    if (state.heap[index].first == -1 || state.heap[index].second != 0 || context.packet_size[state.heap[index].first] < signature->size_in[i]) {
                        throw AliasException("Function pre condition failed", this);
                    }
                    if (packet_num[i] == -2) {
                        packet_num[i] = state.heap[index].first;
                    }
                    else if (state.heap[index].first != packet_num[i]) {
                        throw AliasException("Function pre condition has several packets", this);
                    }
                }
            }
        }
    }

    std::vector <int> new_packet(n);
    for (int i = 0; i < n; i++) {
        if (signature->types[i] == Type::Ptr) {
            if (packet_num[i] >= 0) {
                context.packet_size[packet_num[i]] = 0;
            }
            new_packet[i] = (int)context.packet_size.size();
            context.packet_size.push_back(signature->size_out[i]);
        }
    }

    std::set <State> _states;
    for (State state : context.states) {
        for (int i = 0; i < n; i++) {
            if (getVariableType(arguments[i], this, context) == Type::Ptr) {
                int index = getVariableIndex(arguments[i], this, context);
                for (int j = 0; j < (int)state.heap.size(); j++) {
                    if (j != index && packet_num[i] >= 0 && state.heap[j].first == packet_num[i]) {
                        state.heap[j] = {-1, 0};
                    }
                }
                if (signature->size_out[i] == 0) {
                    state.heap[i] = {-1, 0};
                }
                else {
                    state.heap[index] = {new_packet[i], 0};
                }
            }
        }
        _states.insert(state);
    }
    context.states = _states;
}

void Dereference::Validate(VLContext &context) {
    if (auto _identifier = std::dynamic_pointer_cast <AST::Identifier> (arg)) {
        if (getVariableType(_identifier->identifier, this, context) == Type::Ptr) {
            int index = getVariableIndex(_identifier->identifier, this, context);
            for (State state : context.states) {
                if (state.heap[index].first == -1 ||
                    state.heap[index].second < 0 ||
                    state.heap[index].second >= context.packet_size[state.heap[index].first]) {
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

void Addition::Validate(VLContext &context) {
    left->Validate(context);
    right->Validate(context);
}

void Less::Validate(VLContext &context) {
    left->Validate(context);
    right->Validate(context);
}

void Equal::Validate(VLContext &context) {
    left->Validate(context);
    right->Validate(context);
}

}
