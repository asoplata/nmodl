/*************************************************************************
 * Copyright (C) 2018-2019 Blue Brain Project
 *
 * This file is part of NMODL distributed under the terms of the GNU
 * Lesser General Public License. See top-level LICENSE file for details.
 *************************************************************************/

#pragma once

/**
 * \file
 * \brief Implement class to represent a symbol in Symbol Table
 */

#include <map>
#include <memory>

#include "lexer/modtoken.hpp"
#include "symtab/symbol_properties.hpp"


namespace nmodl {

namespace ast {
class Ast;
}

/// %Symbol table related implementations
namespace symtab {

/**
 * @ingroup sym_tab
 * @{
 */

/**
 * \class Symbol
 * \brief Represent symbol in symbol table
 *
 * Symbol table generator pass visit the AST and insert symbol for
 * each node into symtab::SymbolTable. Symbol could appear multiple
 * times in a block or different global blocks. NmodlType object has all
 * nmodl properties information.
 *
 * \todo
 *   - Multiple tokens (i.e. location information) for symbol should
 *     be tracked
 *   - Scope information should be more than just string
 *   - Perf block should track information about all usage of the symbol
 *     (would be helpful for perf modeling)
 *   - Need to keep track of all renaming information, currently only we
 *     keep last state
 */
class Symbol {
    /// name of the symbol
    std::string name;

    /// original name of the symbol if renamed
    std::string renamed_from;

    /// unique id or index position when symbol is inserted into specific table
    int id = 0;

    /// first AST node for which symbol is inserted
    /// Variable can appear multiple times in the mod file. This node
    /// represent the first occurance of the variable in the input. Currently
    /// we don't track all AST nodes.
    ast::Ast* node = nullptr;

    /// token associated with symbol (from node)
    ModToken token;

    /// properties of symbol as a result of usage across whole mod file
    syminfo::NmodlType properties{syminfo::NmodlType::empty};

    /// status of symbol after processing through various passes
    syminfo::Status status{syminfo::Status::empty};

    /// scope of the symbol (nmodl block name where it appears)
    std::string scope;

    /// order in case of state / prime variable
    int order = 0;

    /// order in which symbol appears in the mod file
    /// Different variables appear in different blocks (NEURON, PARAMETER, STATE)
    /// and accordingly they appear in the data array (in NEURON). This order is
    /// based on appearance in the mod file.
    int definition_order = -1;

    /// associated value in case of parameters, constant variable
    std::shared_ptr<double> value;

    /// true if symbol represent array variable
    bool array = false;

    /// dimension/length in case of array variable
    int length = 1;

    /// number of values that variable can take in case of table variable
    int num_values = 0;

    /// number of times symbol is read
    int read_count = 0;

    /// number of times symbol is written
    int write_count = 0;

  public:
    /// \name Ctor & dtor
    /// \{

    Symbol() = delete;

    Symbol(std::string name, ast::Ast* node)
        : name(name)
        , node(node) {}

    Symbol(std::string name, ModToken token)
        : name(name)
        , token(token) {}

    Symbol(std::string name, ast::Ast* node, ModToken token)
        : name(name)
        , node(node)
        , token(token) {}

    /// \}

    /// increment read count
    void read() {
        read_count++;
    }

    /// increment write count
    void write() {
        write_count++;
    }

    /// \name Setter
    /// \{

    void set_scope(std::string s) {
        scope = s;
    }

    void set_id(int i) {
        id = i;
    }

    /**
     * Set new name for the symbol
     *
     * If symbol is already renamed, do not rename it again
     * as we want to keep track of original name and not intermediate
     * renames
     */
    void set_name(std::string new_name) {
        if (renamed_from.empty()) {
            renamed_from = name;
        }
        name = new_name;
    }

    /**
     * Set order in case of prime/state variable
     *
     * Prime variable will appear in different block and could have
     * multiple derivative orders. We have to store highest order.
     */
    void set_order(int new_order) {
        if (new_order > order) {
            order = new_order;
        }
    }

    void set_definition_order(int order) {
        definition_order = order;
    }

    void set_value(double val) {
        value = std::make_shared<double>(val);
    }

    void set_as_array(int len) {
        array = true;
        length = len;
    }

    void set_num_values(int n) {
        num_values = n;
    }

    void set_original_name(std::string new_name) {
        renamed_from = new_name;
    }

    /// \}

    /// \name Getter
    /// \{

    int get_length() {
        return length;
    }

    int get_num_values() {
        return num_values;
    }

    std::string get_original_name() {
        return renamed_from;
    }

    std::shared_ptr<double> get_value() {
        return value;
    }

    std::string get_name() {
        return name;
    }

    int get_id() {
        return id;
    }

    std::string get_scope() {
        return scope;
    }

    syminfo::NmodlType get_properties() {
        return properties;
    }

    syminfo::Status get_status() {
        return status;
    }

    ast::Ast* get_node() {
        return node;
    }

    ModToken get_token() {
        return token;
    }

    int get_read_count() const {
        return read_count;
    }

    int get_write_count() const {
        return write_count;
    }

    int get_definition_order() const {
        return definition_order;
    }

    /// \}

    /**
     * Check if symbol represent an external variable
     *
     * External variables are the variables that are defined in NEURON
     * and available in mod file.
     *
     * \todo Need to check if we should check two properties using
     *        has_any_property instead of exact comparison
     *
     * \sa nmodl::details::NEURON_VARIABLES
     */
    bool is_external_variable() {
        return (properties == syminfo::NmodlType::extern_neuron_variable ||
                properties == syminfo::NmodlType::extern_method);
    }

    /// check if symbol has any of the given property
    bool has_any_property(syminfo::NmodlType new_properties) {
        return static_cast<bool>(properties & new_properties);
    }

    /// check if symbol has all of the given properties
    bool has_all_properties(syminfo::NmodlType new_properties) {
        return ((properties & new_properties) == new_properties);
    }

    /// check if symbol has any of the status
    bool has_any_status(syminfo::Status new_status) {
        return static_cast<bool>(status & new_status);
    }

    /// check if symbol has all of the status
    bool has_all_status(syminfo::Status new_status) {
        return ((status & new_status) == new_status);
    }

    /// add new properties to symbol
    void add_properties(syminfo::NmodlType new_properties) {
        properties |= new_properties;
    }

    /// add new property to symbol
    void add_property(syminfo::NmodlType property) {
        properties |= property;
    }

    /// mark symbol as inlined (in case of procedure/function)
    void mark_inlined() {
        status |= syminfo::Status::inlined;
    }

    /// mark symbol as newly created (in case of new variable)
    void mark_created() {
        status |= syminfo::Status::created;
    }

    void mark_renamed() {
        status |= syminfo::Status::renamed;
    }

    /// mark symbol as localized (e.g. from RANGE to LOCAL conversion)
    void mark_localized() {
        status |= syminfo::Status::localized;
    }

    void mark_thread_safe() {
        status |= syminfo::Status::thread_safe;
    }

    /// mark symbol as newly created variable for the STATE variable
    /// this is used with legacy euler/derivimplicit solver where DState
    /// variables are created
    void created_from_state() {
        mark_created();
        status |= syminfo::Status::from_state;
    }

    bool is_array() {
        return array;
    }

    /// check if symbol is a variable in nmodl
    bool is_variable();

    std::string to_string();
};

/** @} */  // end of sym_tab

}  // namespace symtab
}  // namespace nmodl
