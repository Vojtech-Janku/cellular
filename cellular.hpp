/* The goal of this task is to implement a simulator for
 * one-dimensional cellular automata. You will implement this
 * simulator as a class, the interface of which is described below
 * You are free to add additional methods and data members to the
 * class, and additional classes and functions to the file, as you
 * see fit. You must, however, keep the entire interface in this
 * single file. The implementation can be in either ‹cellular.hpp›
 * or in ‹cellular.cpp›. Only these two files will be submitted. */

/* The class ‹automaton_state› represents the state of a 1D,
 * infinite binary cellular automaton. The ‹set› and ‹get› methods
 * can be passed an arbitrary index. */

#include <map>
#include <tuple>
#include <bitset>
#include <iostream>

class automaton_state
{
        std::map<int,bool> cells; // map index-value
    /* Attributes are up to you. */

public:
    automaton_state() /* create a blank state (all cells are 0) */
    {
        cells = {};
    }

    void set( int index, bool value ) /* change the given cell */
    {
        cells.erase( index );
        cells.emplace( index, value );
    }

    bool get( int index ) const
    {
        if ( cells.count( index ) == 0 ) return false;
        return cells.at( index );
    }

    std::map<int,bool> get_cells() const {
        return cells;
    }
};

/* The ‹automaton› class represents the automaton itself. The
 * automaton is arranged as a cross, with a horizontal and a
 * vertical automaton, which are almost entirely independent (each
 * has its own state and its own rule), with one twist: the center
 * cell (index 0 in both automata) is shared. The new state of the
 * shared center cell (after a computation step is performed in both
 * automata independently) is obtained by combining the two values
 * (that either automaton would assign to that cell) using a
 * specified boolean binary operator. The new center is obtained as
 * ‹horizontal_center OP vertical_center›. The state can look, for
 * example, like this:
 *
 *                            ┌───┐
 *                            │ … │
 *                            ├───┤
 *                            │ 1 │
 *                            ├───┤
 *                            │ 0 │
 *                ┌───┬───┬───┼───┼───┬───┬───┐
 *                │ … │ 0 │ 0 │ 1 │ 1 │ 0 │ … │
 *                └───┴───┴───┼───┼───┴───┴───┘
 *                            │ 0 │
 *                            ├───┤
 *                            │ 1 │
 *                            ├───┤
 *                            │ … │
 *                            └───┘
 *
 * The automaton keeps its state internally and allows the user to
 * perform simulation on this internal state. Initially, the state
 * of the automaton is 0 (false) everywhere. The rules for both the
 * vertical and the horizontal component are given to the
 * constructor by their Wolfram code.
 *
 * The center-combining operator is given by the same type of code,
 * but instead of 3 cells, only 2 need to be combined: there are
 * only 16 such operators (compared to 256 rules for each of the
 * automata). The input vectors to the binary operator are numbered
 * by their binary code as:
 *
 *  │  left │ right │ index │
 *  ├┄┄┄┄┄┄┄│┄┄┄┄┄┄┄│┄┄┄┄┄┄┄│
 *  │   0   │   0   │   0   │
 *  │   0   │   1   │   1   │
 *  │   1   │   0   │   2   │
 *  │   1   │   1   │   3   │
 *
 * The operator code is then a 4-digit binary number, e.g 0110 gives
 * the code for ‹xor› (0 0 → 0, 0 1 → 1, 1 0 → 1, 1 1 → 0) while
 * 1000 gives code for ‹and› (everything is zero except if both
 * inputs are 1). And so on and so forth. The same process but with
 * 3 input cells is used to construct the Wolfram code for the
 * automata. */

class automaton
{
        /* Attributes are up to you. */

        automaton_state _h_aut, _v_aut;
        int _h_rule, _v_rule, _center;

public:

    enum direction { vertical, horizontal };

    /* Constructs an automaton based on a rule given by its Wolfram
     * code for the horizontal component, another for the vertical
     * component, and a 4-bit code for the center operator. Assume
     * that neither of the rules contains the transition 000 → 1. */

    automaton( int h_rule, int v_rule, int center ):
    _h_rule( h_rule ), _v_rule( v_rule ), _center( center )
    {}

    automaton( automaton_state &horizontal_state, automaton_state &vertical_state,
                   int h_rule, int v_rule, int center ):
    _h_aut( horizontal_state ), _v_aut( vertical_state ),
    _h_rule( h_rule ), _v_rule( v_rule ), _center( center )
    {}

    /* The ‹read› method returns the current value of the (shared)
     * center cell. The set method sets the specified cell to the
     * value given. */

    bool read() const
    {
        return _h_aut.get( 0 );
    }

    void set( direction dir, int index, bool value )
    {
        if ( index == 0 ) {
                _h_aut.set( index, value );
                _v_aut.set( index, value );
        }
        if ( dir == direction::horizontal ) {
                _h_aut.set( index, value );
        } else {
                _v_aut.set( index, value );
        }
    }

    /* Finally, the following methods run the simulation – either
     * perform a single step (update each cell exactly once) or a
     * given number of steps (assume a non-negative number of
     * steps). */

    void step()
    {
        automaton_step( _h_aut, _h_rule );
        automaton_step( _v_aut, _v_rule );
        set( direction::horizontal, 0, compute( _h_aut.get( 0 ), _v_aut.get( 0 ), _center ) );
    }

    void run( int steps )
    {
        for ( int i = 0; i < steps; i++ ) {
                step();
        }
    }

    bool compute( std::tuple< bool, bool, bool > state, int rule )
    {
        std::bitset<sizeof(int)*8> rule_bits( rule );
        return rule_bits.test( 4*std::get<0>(state) + 2*std::get<1>(state) + std::get<2>(state) );
    }

    bool compute( bool hor, bool vert, int rule )
    {
        std::bitset<sizeof(int)*8> rule_bits( rule );
        return rule_bits.test( 2*hor + vert );
    }

    void automaton_step( automaton_state &aut, int rule )
    {
        std::tuple<int,int,int> state;
        std::map<int,bool> cells = aut.get_cells();
        std::map<int,bool> aut_new_cells;
                for ( auto cell : cells ) {
                int idx = cell.first;
                bool val = cell.second;
                state = std::make_tuple( aut.get( idx - 2 ), aut.get( idx - 1 ), val );
                aut_new_cells.emplace( idx - 1, compute( state, rule ) );
                state = std::make_tuple( aut.get( idx - 1 ), val, aut.get( idx + 1 ) );
                aut_new_cells.emplace( idx, compute( state, rule ) );
                state = std::make_tuple( val, aut.get( idx + 1 ), aut.get( idx + 2 ) );
                aut_new_cells.emplace( idx + 1, compute( state, rule ) );
        }
        for ( auto new_cell : aut_new_cells ) {
                aut.set( new_cell.first, new_cell.second );
        }
    }

    void print_aut( bool horizontal, int begin, int end )
    {
        automaton_state aut = ( horizontal ) ? _h_aut : _v_aut;
        char val;
        for ( int i = begin; i < end + 1; i++ ) {
                val = ( aut.get( i ) ) ? '*' : ' ';
                /*if ( i == 0 ) {
                        val = ( aut.get( i ) ) ? '#' : '.';
                } */
                std::cout << val;
        }
        std::cout << '\n';
    }
};

/* The ‹compute_cell› function takes two rule numbers, two initial
 * states, a center operator and a number of steps. It then computes
 * the value of the central cell after n steps of the automaton such
 * described. Like above, the number of steps is a non-negative
 * number. Assume that the center cell in both input states has the
 * same value. */

bool compute_cell( int vertical_rule, int horizontal_rule,
                   int center_op,
                   const automaton_state &vertical_state,
                   const automaton_state &horizontal_state,
                   int steps );
