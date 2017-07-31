
*phorward* is a free toolkit for parser development, lexical analysis, regular expressions and more.

# ABOUT 

The toolkit is primarily a library, written in C, that provides an interface for defining, running and processing parsers, lexical anlyzers and regular expressions through a consistent and easy-to-use interface. The following example program defines a simple expressional language, runs a parser on it and prints the generated abstract syntax tree.

    #include <phorward.h>
    
    int main()
    {
        pparse* parser;
        ppast*  ast;
        char*   input = "1+2*(3+4)+5";
        char*   end;
    
        parser = pp_create( 0,  "@int /[0-9]+/ ;"
                                "fact : int | '(' expr ')' ;"
                                "term : @mul( term '*' fact ) | fact ;"
                                "expr : @add( expr '+' term ) | term ;" );
    
        if( !pp_parse_to_ast( &ast, parser, input, &end ) )
            return 1; /* parse error */
    
        pp_ast_dump_short( stdout, ast );
        return 0;
    }

It can easily be compiled with:

    $ cc -o example example.c -lphorward

Furthermore, the toolkit comes with a command-line tool serving testing and prototyping facilities. The following command call yields in an equivalent parser and its abstract syntax tree, althought some symbol names are shortened.

    $ pparse "@int /[0-9]+/; f: int | '(' e ')'; t: @mul( t '*' f ) | f; e: @add( e '+' t ) | t;" "1+2*(3+4)+5"

phorward also provides useful general-purpose extensions for C programming. This includes dynamic data structures (e.g. linked lists, hash-tables, stacks and arrays), extended string management functions and platform-independent, system-specific helper functions.

# FEATURES 

*phorward* provides the following features:

 * Parser development tools
  * Self-hosted Backus-Naur-Form (BNF) grammar definition language
  * *pparse* provides a modular LR(1) and LALR(1) parser generator
  * *ppast* is a representation of a browsable abstract syntax tree (AST)
 * Lexer development tools
  * regular expressions and pattern definition interface
  * *plex* provides a lexical analyzer
  * *pregex* for definition and execution of regular expression
  * *pccl* for unicode-enabled character classes
  * tools for regex and lexer deployment
  * string functions for regular expression match, split and replace
 * Runtime evaluation tools
  * construction of dynamic intermediate languages and interpreters
  * *pany* is a data object for handling different data-types in one object
  * *pvm* for defining stack-based virtual machine instruction sets
 * Dynamic data structures
  * *plist* for linked-lists with build-in hash table support,
  * *parray* for arrays and stacks.
 * Extended string management functions
  * concat, extend, tokenize and short-hand allocation of strings and wide-character strings
  * consistent byte- and wide-character (unicode) function support
  * unicode support for UTF-8 in byte-character functions
 * Universal system-specific functions for platform-independent C software development
  * Unix-style command-line parser
  * Mapping files to strings
 * Debug und trace facilities
 * Consequent object-oriented build-up of all function interfaces (e.g. plist, parray, pregex, pparse, ...)
 * Growing code-base of more and more powerful functions

Please check out http://phorward.phorward-software.com/ continuously to get latest news, documentation, updates and support on the Phorward Toolkit.

# GETTING STARTED 

*phorward* is under heavy development since a few years. It is kept simple, clear and straightforward.
Documentation can be found in the file http://www.phorward-software.com/products/phorward/doc/phorward.html, but also locally after installation.

The documentation is currently in an under-development state and incomplete. It contains a generated functions reference and handles all library parts shortly.

# BUILDING 

Building the Phorward Toolkit is simple as every GNU-style open source program. Extract the downloaded release tarball or clone the hg repository into a directory of your choice.

Then, run

    $ ./configure

to configure the build-system and generate the Makefiles for your current platform. After successful configuration, run

    $ make

and

    $ make install

(properly as root), to install the toolkit into your system.

# LOCAL DEVELOPMENT BUILD-SYSTEM 

Alternatively there is also a simpler method for setting up a local build system for development and testing purposes locally in the file-system.

Once, type

    $ make -f Makefile.gnu make_install

then, a simple run of

    $ make

can be used to simply build the entire library or parts of it.

Note, that changes to the build system then must be done in the local Makefile, the local Makefile.gnu as well as the Makefile.am for the autotools-based build system.

# AUTHOR 

The Phorward Toolkit is developed and maintained by Jan Max Meyer, Phorward Software Technologies.

Some other projects by the author are:

 * *pynetree* (http://pynetree.org): A light-weight parsing toolkit written in pure Python.
 * *UniCC* (http://unicc.phorward-software.com): Universal, target-language independent LALR(1) parser generator.
 * *JS/CC* (http://jscc.brobston.com): The JavaScript parser generator.

# LICENSE 

This software is an open source project released under the terms and conditions of the 3-clause BSD license. See the LICENSE file for more information.

# COPYRIGHT 

Copyright (C) 2006-2017 by Phorward Software Technologies, Jan Max Meyer.

You may use, modify and distribute this software under the terms and conditions of the 3-clause BSD license. The full license terms can be obtained from the file LICENSE.

THIS SOFTWARE IS PROVIDED BY JAN MAX MEYER (PHORWARD SOFTWARE TECHNOLOGIES) AS IS AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL JAN MAX MEYER (PHORWARD SOFTWARE TECHNOLOGIES) BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

