#include <iostream>
#include "ast.h"
#include "compile.h"

namespace AST {

int findInLocal(std::string &identifier, CPContext &context) {
    for (int i = (int)context.variable_stack.size() - 1; i >= 0; i--) {
        if (context.variable_stack[i] == identifier) {
            return i;
        }
    }
    return -1;
}

int findInArguments(std::string &identifier, CPContext &context) {
    for (int i = 0; i < (int)context.variable_arguments.size(); i++) {
        if (context.variable_arguments[i] == identifier) {
            return i;
        }
    }
    return -1;
}

int findFunctionIndex(std::string &identifier, CPContext &context) {
    for (int i = (int)context.function_stack.size() - 1; i >= 0; i--) {
        if (context.function_stack[i].first == identifier) {
            return context.function_stack[i].second;
        }
    }
    std::cout << "Error: function identifier not found" << std::endl;
    exit(1);
}

int findPhase(std::string &identifier, CPContext &context) {
    int idx = findInLocal(identifier, context);
    if (idx != -1) {
        return -(idx + 1) * 4;
    }
    else {
        idx = findInArguments(identifier, context);
        return (idx + 2) * 4;
    }
}

void Compile(std::shared_ptr <Node> node, std::ostream &out) {
    out << "; program\n";
    out << "global main\n";
    out << "extern malloc\n";
    out << "extern free\n";
    out << "section .text\n";
    out << "main:\n";
    out << "push ebp\n";
    out << "mov ebp, esp\n";
    CPContext context;
    node->Compile(out, context);
    out << "leave\n";
    out << "ret\n";
}

void Block::Compile(std::ostream &out, CPContext &context) {
    out << "; block\n";
    size_t old_variable_stack_size = context.variable_stack.size();
    size_t old_function_stack_size = context.function_stack.size();
    for (auto i = statement_list.begin(); i != statement_list.end(); i++) {
        (*i)->Compile(out, context);
    }
    while (context.variable_stack.size() > old_variable_stack_size)
        context.variable_stack.pop_back();
    while (context.function_stack.size() > old_function_stack_size)
        context.function_stack.pop_back();
}

void Asm::Compile(std::ostream &out, CPContext &context) {
    out << "; asm\n";
    out << code << "\n";
}

void If::Compile(std::ostream &out, CPContext &context) {
    out << "; if\n";
    branch_list[0].first->Compile(out, context);
    int idx = context.branch_index;
    context.branch_index++;
    out << "cmp [esp - 4], dword 0\n";
    out << "je _if_else" << idx << "\n";
    branch_list[0].second->Compile(out, context);
    out << "jmp _if_end" << idx << "\n";
    out << "_if_else" << idx << ":\n";
    if (else_body) {
        else_body->Compile(out, context);
    }
    out << "_if_end" << idx << ":\n";
}

void While::Compile(std::ostream &out, CPContext &context) {
    out << "; while\n";
    int idx = context.branch_index;
    context.branch_index++;
    out << "_while" << idx << ":\n";
    expression->Compile(out, context);
    out << "cmp [esp - 4], dword 0\n";
    out << "je _while_end" << idx << "\n";
    block->Compile(out, context);
    out << "jmp _while" << idx << "\n";
    out << "_while_end" << idx << ":\n";
}

void FunctionDefinition::Compile(std::ostream &out, CPContext &context) {
    out << "; function definition\n";
    int idx = context.function_index;
    out << "jmp _funend" << idx << "\n";
    out << "_fun" << idx << ":\n";
    out << "push ebp\n";
    out << "mov ebp, esp\n";
    out << "sub esp, " << (signature->identifiers.size() + 2) * 4 << "\n";
    CPContext _context;
    _context.function_index = context.function_index;
    for (int i = 0; i < (int)signature->identifiers.size(); i++) {
        _context.variable_arguments.push_back(signature->identifiers[i]);
    }
    int function_index = _context.function_index;
    _context.function_stack.push_back({name, function_index});
    _context.function_index++;
    body->Compile(out, _context);
    context.function_index = _context.function_index;
    context.function_stack.push_back({name, function_index});
    out << "leave\n";
    out << "ret\n";
    out << "_funend" << idx << ":\n";
}

void Definition::Compile(std::ostream &out, CPContext &context) {
    out << "; definition\n";
    context.variable_stack.push_back(identifier);
    out << "sub esp, 4\n";
}

void Assignment::Compile(std::ostream &out, CPContext &context) {
    out << "; assignment\n";
    value->Compile(out, context);
    int phase = findPhase(identifier, context);
    out << "mov eax, [esp - 4]\n";
    out << "mov [ebp + " << phase << "], eax\n";
}

void Movement::Compile(std::ostream &out, CPContext &context) {
    out << "; movement\n";
    value->Compile(out, context);
    int phase = findPhase(identifier, context);
    out << "mov eax, [esp - 4]\n";
    out << "mov ebx, [ebp + " << phase << "]\n";
    out << "mov [ebx], eax\n";
}

void Assumption::Compile(std::ostream &out, CPContext &context) {
}

void Identifier::Compile(std::ostream &out, CPContext &context) {
    out << "; identifier\n";
    int phase = findPhase(identifier, context);
    out << "mov eax, [ebp + " << phase << "]\n";
    out << "mov [esp - 4], eax\n";
}

void Integer::Compile(std::ostream &out, CPContext &context) {
    out << "; integer\n";
    out << "mov [esp - 4], dword " << value << "\n";
}

void Alloc::Compile(std::ostream &out, CPContext &context) {
    out << "; alloc\n";
    out << "push dword " << 4 * size << "\n";
    out << "call malloc\n";
    out << "add esp, 4\n";
    out << "mov [esp - 4], eax\n";
}

void Free::Compile(std::ostream &out, CPContext &context) {
    out << "; free\n";
    arg->Compile(out, context);
    out << "push dword [esp - 4]\n";
    out << "call free\n";
    out << "add esp, 4\n";
}

void FunctionCall::Compile(std::ostream &out, CPContext &context) {
    out << "; function call\n";
    for (int i = (int)arguments.size() - 1; i >= 0; i--) {
        int phase = findPhase(arguments[i], context);
        out << "push dword [ebp + " << phase << "]\n";
    }
    int idx = findFunctionIndex(identifier, context);
    out << "call _fun" << idx << "\n";
    out << "add esp, " << (int)arguments.size() * 4 << "\n";
    for (int i = (int)arguments.size() - 1; i >= 0; i--) {
        int phase = findPhase(arguments[i], context);
        out << "mov eax, [esp - " << (((int)arguments.size() - i) * 4) << "]\n";
        out << "mov [ebp + " << phase << "], eax\n";
    }
}

void Dereference::Compile(std::ostream &out, CPContext &context) {
    out << "; dereference\n";
    arg->Compile(out, context);
    out << "mov eax, [esp - 4]\n";
    out << "mov ebx, [eax]\n";
    out << "mov [esp - 4], ebx\n";
}

void Addition::Compile(std::ostream &out, CPContext &context) {
    out << "; addition\n";
    left->Compile(out, context);
    out << "sub esp, 4\n";
    context.variable_stack.push_back("__junk");
    right->Compile(out, context);
    out << "add esp, 4\n";
    context.variable_stack.pop_back();
    out << "mov eax, [esp - 4]\n";
    out << "add eax, [esp - 8]\n";
    out << "mov [esp - 4], eax\n";
}

void Less::Compile(std::ostream &out, CPContext &context) {
    out << "; less\n";
    left->Compile(out, context);
    out << "sub esp, 4\n";
    context.variable_stack.push_back("__junk");
    right->Compile(out, context);
    out << "add esp, 4\n";
    context.variable_stack.pop_back();
    out << "mov eax, [esp - 4]\n";
    out << "sub eax, [esp - 8]\n";
    int idx = context.branch_index++;
    out << "jl " << "_set1_" << idx << "\n";
    out << "mov [esp - 4], 0\n";
    out << "jmp _setend" << idx << "\n";
    out << "_set1_" << idx << ":\n";
    out << "mov [esp - 4], 1\n";
    out << "_setend" << idx << ":\n";
}

void Equal::Compile(std::ostream &out, CPContext &context) {
    out << "; less\n";
    left->Compile(out, context);
    out << "sub esp, 4\n";
    context.variable_stack.push_back("__junk");
    right->Compile(out, context);
    out << "add esp, 4\n";
    context.variable_stack.pop_back();
    out << "mov eax, [esp - 4]\n";
    out << "sub eax, [esp - 8]\n";
    int idx = context.branch_index++;
    out << "jz " << "_set1_" << idx << "\n";
    out << "mov [esp - 4], 0\n";
    out << "jmp _setend" << idx << "\n";
    out << "_set1_" << idx << ":\n";
    out << "mov [esp - 4], 1\n";
    out << "_setend" << idx << ":\n";
}

}