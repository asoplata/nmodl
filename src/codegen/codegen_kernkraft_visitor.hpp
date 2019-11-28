/*************************************************************************
 * Copyright (C) 2018-2019 Blue Brain Project
 *
 * This file is part of NMODL distributed under the terms of the GNU
 * Lesser General Public License. See top-level LICENSE file for details.
 *************************************************************************/

#pragma once

#include "codegen/codegen_c_visitor.hpp"


namespace nmodl {
namespace codegen {

/**
 * \class CodegenKernkraft
 * \brief Visitor for printing c code with OpenMP backend
 */
class CodegenKernkraft: public CodegenCVisitor {
  protected:
    /// name of the code generation backend
    std::string backend_name() override;


    /// Also reimplement something to handle nt->dt similar to below
    std::string float_variable_name(SymbolType& symbol, bool use_instance) override;

    /// get variable name for global variable
    std::string global_variable_name(SymbolType& symbol)override ;

    /// get ion shadow variable name
    std::string ion_shadow_variable_name(SymbolType& symbol) override;

    /// get variable name for int variable
    std::string int_variable_name(IndexVariableInfo& symbol,
                                          const std::string& name,
                                          bool use_instance) override;

    /// any statement block in nmodl with option to (not) print braces
    void print_statement_block(ast::StatementBlock* node,
                                       bool open_brace = true,
                                       bool close_brace = true) override;

    /// call to internal or external function
    void print_function_call(ast::FunctionCall* node) override;

    void print_global_function_common_code(BlockType type) override;

    /// helper function to find unique variable names and print them in a single declaration statement
    void print_unique_local_variables(ast::StatementVector statements);

    std::string get_variable_name(const std::string& name, bool use_instance = true) override;

    /// entry point to code generation
    void print_codegen_routines() override;

    /// structure that wraps all range and int variables required for mod file
    void print_mechanism_range_var_structure() override;

    /// nrn_state / state update function definition
    void print_nrn_state() override;

    /// nrn_cur / current update function definition
    void print_nrn_cur() override;

    /// main body of nrn_cur function
    void print_nrn_cur_kernel(ast::BreakpointBlock* node) override;

    /// nrn_cur_kernel will use this kernel if conductance keywords are specified
    void print_nrn_cur_conductance_kernel(ast::BreakpointBlock* node) override;

    /// update to matrix elements with/without shadow vectors
    void print_nrn_cur_matrix_shadow_update() override;

    /// backend specific block start for tiling on channel iteration
    virtual void print_channel_iteration_tiling_block_begin(BlockType type) override;

    /// backend specific channel instance iteration block start
    virtual void print_channel_iteration_block_begin(BlockType type) override;

    /// ion variable copies are avoided
    bool optimize_ion_variable_copies() override;

    /// check if give statement should be skipped during code generation
    bool statement_to_skip(ast::Statement* node) override;

  public:
    CodegenKernkraft(std::string mod_file,
                     std::string output_dir,
                     LayoutType layout,
                     std::string float_type)
        : CodegenCVisitor(mod_file, output_dir, layout, float_type, ".kr.cpp") {}

    CodegenKernkraft(std::string mod_file,
                     std::stringstream& stream,
                     LayoutType layout,
                     std::string float_type)
        : CodegenCVisitor(mod_file, stream, layout, float_type) {}

    void visit_binary_expression(ast::BinaryExpression* node) override;
    void visit_if_statement(ast::IfStatement* node) override;
    void visit_else_if_statement(ast::ElseIfStatement* node) override;
    void visit_else_statement(ast::ElseStatement* node) override;


};

}  // namespace codegen
}  // namespace nmodl