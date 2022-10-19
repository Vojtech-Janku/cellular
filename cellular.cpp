#include "cellular.hpp"
#include <iostream>
#include <unistd.h>


/* You can put implementations of non-template methods and functions
 * into this file, as you see fit. Do not put anything after the
 * ‹main› function, and make sure its prototype remains untouched.
 * Feel free to add testing code to ‹main›. */

bool compute_cell( int vertical_rule, int horizontal_rule, int center_op,
                                   const automaton_state &vertical_state,
                                   const automaton_state &horizontal_state,
                                   int steps )
{
        automaton_state hor_state = horizontal_state, vert_state = vertical_state;
        automaton aut( hor_state, vert_state, horizontal_rule, vertical_rule, center_op );
        aut.run( steps );
        return aut.read();
}

int main()
{
        /* tests go here */
        int vert_rule = 100, hor_rule = 30, center_op = 12;
        automaton_state hor_state, vert_state;
        hor_state.set( 10, true );
        //vert_state.set( 12, true );
        automaton aut( hor_state, vert_state, hor_rule, vert_rule, center_op );

        int begin = -120, end = 140, steps = 1000;

        std::cout << "Printing horizontal states:\n";
        automaton aut1 = aut;
        std::cout << 0 << ":    ";
        aut1.print_aut( true, begin, end );
        for ( int i = 0; i < steps; i++ ) {
                aut1.step();
                std::cout << i+1 << ":  ";
                aut1.print_aut( true, begin, end );
                usleep(50000);
        }

        /*std::cout << "Printing vertical states:\n";
        automaton aut2 = aut;
        std::cout << 0 << ":    ";
        aut2.print_aut( false, begin, end );
        for ( int i = 0; i < steps; i++ ) {
                aut2.step();
                std::cout << i+1 << ":  ";
                aut2.print_aut( false, begin, end );
        }

        std::cout << "Printing center cell:\n";
        for ( int i = 0; i <= steps; i++ ) {
                bool val = compute_cell( vert_rule, hor_rule, center_op, vert_state, hor_state, i );
                char chr = ( val ) ? '#' : '.';
                std::cout << i << ":            " << chr << '\n';
        }
        */
}
