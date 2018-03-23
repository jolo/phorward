/* -HEADER----------------------------------------------------------------------
Phorward Foundation Toolkit
Copyright (C) 2006-2018 by Phorward Software Technologies, Jan Max Meyer
http://www.phorward-software.com ++ contact<at>phorward<dash>software<dot>com
All rights reserved. See LICENSE for more information.

File:	phorward.h
Usage:	Phorward Foundation Toolkit Global Header
----------------------------------------------------------------------------- */

/*
	This file was automatically generated by pinclude.
	DO NOT CHANGE THIS FILE MANUALLY. IT WILL GO AWAY!
*/


#ifndef _PHORWARD_H
#define _PHORWARD_H


#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <ctype.h>
#include <limits.h>

#ifdef UTF8
#include <wchar.h>
#endif

#ifdef UNICODE
#include <wctype.h>
#endif

#ifdef _WIN32
#include <process.h>
#else
#include <unistd.h>
#endif



typedef char 					pboolean;	
#ifndef BOOLEAN
#define BOOLEAN					pboolean
#endif

#ifndef boolean
#define boolean 				pboolean
#endif

#ifndef TRUE
#define TRUE					1
#endif

#ifndef FALSE
#define FALSE					0
#endif

#define BOOLEAN_STR( b ) 		( ( b ) ? "TRUE" : "FALSE" )
#define TRUEBOOLEAN( b ) 		( ( b ) ? TRUE : FALSE )


#define OUTOFMEM				fprintf( stderr, \
									"%s, %d: Ran out of memory\n", \
										__FILE__, __LINE__ ), exit( 1 )

#define WRONGPARAM				fprintf( stderr, \
									"%s, %d: Function called with wrong or " \
									"incomplete parameters, fix your call!\n", \
										__FILE__, __LINE__ )

#define MISSINGCASE				fprintf( stderr, \
									"%s, %d: Missing case engaged, " \
									"please check for correctness.\n", \
									__FILE__, __LINE__ )

#define TODO					fprintf( stderr, \
									"%s, %d: TODO alert! The program ran into "\
									"a module that is not finished yet!\n", \
											__FILE__, __LINE__ )

#define CORE					{ 	int* x = NULL; \
									fprintf( stderr, \
									"%s, %d: !CORE!\n", __FILE__, __LINE__ ); \
									*x = 123; \
								}


#ifndef _WIN32
#define PDIRSEP					':'
#define PPATHSEP				'/'
#else
#define PDIRSEP					';'
#define PPATHSEP				'\\'
#endif




#define LIBPHORWARD_VERSION "0.22.0 develop"




#ifndef _DBG_H
#define _DBG_H


#ifdef DEBUG
	#define PROC( name ) \
		static char*	_dbg_proc_name	= name; \
		_dbg_trace( __FILE__, __LINE__, "ENTRY", _dbg_proc_name, (char*)NULL )
#else
	#define PROC( name )
#endif


#ifdef DEBUG
	#define RETURN( val ) \
		do \
		{ \
			_dbg_trace( __FILE__, __LINE__, \
				"RETURN", _dbg_proc_name, (char*)NULL ); \
			return val; \
		} \
		while( 0 )
#else
	#define RETURN( val ) return val
#endif


#ifdef DEBUG
	#define VOIDRET \
		do \
		{ \
			_dbg_trace( __FILE__, __LINE__, \
				"RETURN", _dbg_proc_name, (char*)NULL ); \
			return; \
		} \
		while( 0 )
#else
	#define VOIDRET return
#endif


#ifdef DEBUG
	#define MSG( text ) \
		_dbg_trace( __FILE__, __LINE__, "MSG", _dbg_proc_name, "%s", text )
#else
	#define MSG( text )
#endif


#ifdef DEBUG
	#define VARS( name, format, val ) \
		_dbg_trace( __FILE__, __LINE__, "VARS", _dbg_proc_name, \
				"%s = >" format "<", name, val )
#else
	#define VARS( name, format, val )
#endif


#ifdef DEBUG
	#define PARMS( name, format, val ) \
		_dbg_trace( __FILE__, __LINE__, "PARMS", _dbg_proc_name, \
			"%s = >" format "<", name, val )
#else
	#define PARMS( name, format, val )
#endif


#ifdef DEBUG
	#define LOG( format, ... ) \
		_dbg_trace( __FILE__, __LINE__, "LOG", _dbg_proc_name, \
			format, __VA_ARGS__ )
#else
	#define LOG( name, ... )
#endif

#endif



#ifndef PARRAY_H
#define PARRAY_H

typedef struct
{
	void*	array;
	size_t	first;
	size_t	last;
	size_t	count;

	size_t	size;
	size_t	chunk;
} parray;

typedef void (*parrayfn)	( void* );		





#define parray_for( array, ptr )	\
	for( (ptr) = parray_first( array ); (ptr); \
			(ptr) = parray_next( array, ptr ) )

#endif



#ifndef PLIST_H
#define PLIST_H


typedef struct Plist		plist;			
typedef struct Plistel		plistel;		
typedef void (*plistelfn)	( plistel* );	
typedef void (*plistfn)		( void* );		


struct Plistel
{
	char*					key;
	plist*					list;

	plistel*				prev;
	plistel*				next;

	plistel*				hashnext;
	plistel*				hashprev;
};


struct Plist
{
	int						flags;
#define PLIST_MOD_NONE		0	
#define PLIST_MOD_PTR		1	
#define PLIST_MOD_RECYCLE	2	
#define PLIST_MOD_AUTOSORT	4	
#define PLIST_MOD_EXTKEYS	8	
#define PLIST_MOD_PTRKEYS	16	
#define PLIST_MOD_UNIQUE	32	
#define PLIST_MOD_WCHAR		64	

	int						size;
	int						count;
	int						hashsize;
#define PLIST_DFT_HASHSIZE	64

	int						(*comparefn)( plist*, plistel*, plistel* );
	int						(*sortfn)( plist*, plistel*, plistel* );
	void					(*printfn)( plist* );

	plistel*				unused;

	plistel*				first;
	plistel*				last;
	plistel**				hash;
};





#define plist_for( l, e )	\
	for( (e) = plist_first( l ); (e); (e) = plist_next( e ) )

#endif 




#define PCCL_MIN			0x0
#if UNICODE
#define PCCL_MAX			0xFFFF
#else
#define PCCL_MAX			0xFF
#endif

typedef struct
{
	wchar_t			begin;
	wchar_t			end;
} pcrange;

typedef struct
{
	int				min;
	int				max;

	plist*			ranges;

	char*			str;
} pccl;






typedef enum
{
	PANYTYPE_NULL,

	
	PANYTYPE_BOOL,
	PANYTYPE_CHAR,
	PANYTYPE_INT,
	PANYTYPE_LONG,
	PANYTYPE_ULONG,
	PANYTYPE_FLOAT,
	PANYTYPE_DOUBLE,

	
	PANYTYPE_STR,
	PANYTYPE_CSTR,
	PANYTYPE_WCS,
	PANYTYPE_CWCS,

	
	PANYTYPE_PTR
} panytype;


typedef struct
{
	panytype		type;			

	union							
	{
		pboolean		b;
		

		char			c;
		

		int				i;
		

		long			l;
		

		unsigned long	ul;
		

		float			f;
		

		double			d;
		

		char*			s;
		
		

		wchar_t*		ws;
		

		

		void*			ptr;
		
	} val;
} pany;


#define pany_set_strdup( val, str ) \
			pany_set_str( val, pstrdup( str ) )
#define pany_set_strndup( val, str, n ) \
			pany_set_str( val, pstrndup( str, n ) )

#define pany_set_wcsdup( val, wcs ) \
			pany_set_wcs( val, pwcsdup( wcs ) )
#define pany_set_wcsndup( val, wcs ) \
			pany_set_wcs( val, pwcnsdup( wcs ) )






#define PREGEX_ALLOC_STEP		16
#define PREGEX_MAXREF			32




#define PREGEX_COMP_WCHAR		0x01	
#define PREGEX_COMP_NOANCHORS	0x02	
#define PREGEX_COMP_NOREF		0x04	
#define PREGEX_COMP_NONGREEDY	0x08	
#define PREGEX_COMP_NOERRORS	0x10	
#define PREGEX_COMP_INSENSITIVE	0x20	
#define PREGEX_COMP_STATIC		0x40	
#define PREGEX_COMP_PTN			0x80	


#define PREGEX_RUN_WCHAR		0x100	
#define	PREGEX_RUN_UCHAR		0x200	
#define PREGEX_RUN_NOANCHORS	0x400	
#define PREGEX_RUN_NOREF		0x800	
#define PREGEX_RUN_NONGREEDY	0x1000	
#define PREGEX_RUN_DEBUG		0x2000 	


#define PREGEX_FLAG_NONE		0x00	
#define PREGEX_FLAG_BOL			0x01	
#define PREGEX_FLAG_EOL			0x02	
#define PREGEX_FLAG_BOW			0x04	
#define PREGEX_FLAG_EOW			0x08	
#define PREGEX_FLAG_NONGREEDY	0x10	


enum _regex_ptntype
{
	PREGEX_PTN_NULL,
	PREGEX_PTN_CHAR,
	PREGEX_PTN_SUB,
	PREGEX_PTN_REFSUB,
	PREGEX_PTN_ALT,
	PREGEX_PTN_KLE,
	PREGEX_PTN_POS,
	PREGEX_PTN_OPT
};


typedef struct	_regex_nfa_st	pregex_nfa_st;
typedef struct	_regex_nfa		pregex_nfa;

typedef struct	_regex_dfa_tr	pregex_dfa_tr;
typedef struct	_regex_dfa_st	pregex_dfa_st;
typedef struct	_regex_dfa		pregex_dfa;

typedef enum 	_regex_ptntype	pregex_ptntype;
typedef struct	_regex_ptn		pregex_ptn;

typedef struct	_range			prange;
typedef struct	_regex			pregex;
typedef struct	_lex			plex;





struct _regex_nfa_st
{
	pccl*			ccl;		
	pregex_nfa_st*	next;		
	pregex_nfa_st*	next2;		

	unsigned int	accept;		
	int				flags;		
	int				refs;		
};


struct _regex_nfa
{
	plist*			states;		
	int				modifiers;	
};


struct _regex_dfa_tr
{
	pccl*			ccl;		
	unsigned int	go_to;		
};


struct _regex_dfa_st
{
	plist*			trans;		
	pregex_dfa_tr*	def_trans;	

	unsigned int	accept;		
	int				flags;		
	int				refs;		
};


struct _regex_dfa
{
	plist*			states;		
};



struct _regex_ptn
{
	pregex_ptntype	type;		
	pccl*			ccl;		

	pregex_ptn*		child[ 2 ];	
	pregex_ptn*		next;		

	unsigned int	accept;		
	int				flags;		

	char*			str;		
};




struct _range
{
	unsigned int	id;			
	char*			start;		
	char*			end;		
};


struct _regex
{
	int				flags;		
	pregex_ptn*		ptn;		

	int				trans_cnt;	
	wchar_t**		trans;		

	prange			ref			[ PREGEX_MAXREF ];
};



struct _lex
{
	int				flags;		
	plist*			ptns;		

	int				trans_cnt;	
	wchar_t**		trans;		

	prange			ref			[ PREGEX_MAXREF ];
};






typedef struct _pvm 	pvm;
typedef struct _pvmprog	pvmprog;
typedef struct _pvmexec	pvmexec;

typedef unsigned long	pvmaddr;
typedef unsigned char	pvmbyte;

#define PVM_MAXOPS		CHAR_MAX			


typedef void (*pvmop)( pvmexec* runtime );	


struct _pvm
{
	pvmop		op		[ PVM_MAXOPS ];		
	char*		mn		[ PVM_MAXOPS ];		

	plex*		lex;						
};


struct _pvmprog
{
	pvm*		vm;							
	parray		prog;						
	parray		lit;						
	plist		label;						
};


struct _pvmexec
{
	parray*		stack;						

	pvmaddr		fp;							
	pvmbyte*	cs;							
	pvmbyte*	ecs;						
	pvmbyte*	ip;							
};




typedef struct _ppsym		ppsym;
typedef struct _ppprod		ppprod;
typedef struct _ppgram		ppgram;
typedef struct _ppast		ppast;


#define PPFLAG_NONE			0
#define PPFLAG_CALLED		1
#define PPFLAG_DEFINED		2
#define PPFLAG_NULLABLE		4
#define PPFLAG_LEFTREC		8
#define PPFLAG_LEXEM		16
#define PPFLAG_WHITESPACE	32
#define PPFLAG_PREVENTLREC	64
#define PPFLAG_NAMELESS		128
#define PPFLAG_GENERATED	256
#define PPFLAG_FREENAME		512
#define PPFLAG_FREEEMIT		1024
#define PPFLAG_SPECIAL		2048
#define PPFLAG_FINALIZED	4096
#define PPFLAG_FROZEN		8192

#define PPMOD_OPTIONAL		'?'
#define PPMOD_POSITIVE		'+'
#define PPMOD_KLEENE		'*'

#define PPLR_SHIFT			1
#define PPLR_REDUCE			2


struct _ppprod
{
	ppgram*					grm;		

	unsigned int			idx;		
	ppsym*					lhs;		
	plist*					rhs;		
	unsigned int			flags;		

	char*					emit;		

	char*					strval;		
};


struct _ppsym
{
	ppgram*					grm;		

	unsigned int			idx;		
	char*					name;		
#define PPSYM_T_EOF			"&eof"

	unsigned int			flags;		

	plist*					first;		

	char*					emit;		
	pregex_ptn*				ptn;		

	char*					strval;		
};

#define PPSYM_IS_TERMINAL( sym )	( !sym->name || !( *sym->name ) \
										|| !islower( *( sym )->name ) )
#define PPSYM_IS_NONTERMINAL( sym )	!PPSYM_IS_TERMINAL( sym )


struct _ppgram
{
	plist*					symbols;	
	plist*					prods;		

	ppsym*					goal;		
	ppsym*					eof;		

	unsigned int			flags;		

	char*					strval;		
};


struct _ppast
{
	char*					emit;		

	ppsym*					sym;		
	ppprod*					prod;		

	
	char*					start;		
	char*					end;		
	size_t					len;		

	
	unsigned long			row;		
	unsigned long			col;		

	
	ppast*					child;		
	ppast*					prev;		
	ppast*					next;		
};


typedef enum
{
	PPAST_EVAL_TOPDOWN,
	PPAST_EVAL_PASSOVER,
	PPAST_EVAL_BOTTOMUP
} ppasteval;

typedef void (*pastevalfn)( ppasteval type, ppast* node );


typedef struct
{
	
	ppgram*					gram;		

	
	unsigned int			states;		
	unsigned int**			dfa;		

	
	ppsym**					tokens;
	ppsym**					ntokens;

	plex*					lex;		

	unsigned int			(*lexfn)( char** start, char** end ); 

} pppar;



#ifdef DEBUG
	#define PP_GRAM_DUMP( g ) \
		_dbg_gram_dump( __FILE__, __LINE__, _dbg_proc_name, #g, g )
#else
	#define PP_GRAM_DUMP( g )
#endif


#ifdef DEBUG
	#define PP_AST_DUMP( ast ) \
		_dbg_ast_dump( __FILE__, __LINE__, _dbg_proc_name, #ast, ast )
#else
	#define PP_AST_DUMP( ast )
#endif




#ifdef __cplusplus
extern "C"
{
#endif


pboolean pany_init( pany* val );
pany* pany_create( char* str );
pboolean pany_reset( pany* val );
pany* pany_free( pany* val );
pboolean pany_parse( pany* val, char* str, panytype enforce );
pboolean pany_copy( pany* dest, pany* src );
pany* pany_dup( pany* src );


pboolean pany_to_bool( pany* val );
char pany_to_char( pany* val );
int pany_to_int( pany* val );
long pany_to_long( pany* val );
unsigned long pany_to_ulong( pany* val );
float pany_to_float( pany* val );
double pany_to_double( pany* val );
char* pany_to_str( pany* val );
wchar_t* pany_to_wcs( pany* val );
void* pany_to_ptr( pany* val );
pboolean pany_convert( pany* val, panytype type );


void pany_fprint( FILE* stream, pany* val );


pboolean pany_get_bool( pany* val );
char pany_get_char( pany* val );
int pany_get_int( pany* val );
long pany_get_long( pany* val );
unsigned long pany_get_ulong( pany* val );
float pany_get_float( pany* val );
double pany_get_double( pany* val );
char* pany_get_cstr( pany* val );
char* pany_get_str( pany* val );
wchar_t* pany_get_cwcs( pany* val );
wchar_t* pany_get_wcs( pany* val );
void* pany_get_ptr( pany* val );


pboolean pany_set_bool( pany* val, pboolean b );
char pany_set_char( pany* val, char c );
int pany_set_int( pany* val, int i );
long pany_set_long( pany* val, long l );
unsigned long pany_set_ulong( pany* val, unsigned long ul );
float pany_set_float( pany* val, float f );
double pany_set_double( pany* val, double d );
char* pany_set_cstr( pany* val, char* s );
char* pany_set_str( pany* val, char* s );
wchar_t* pany_set_cwcs( pany* val, wchar_t* ws );
wchar_t* pany_set_wcs( pany* val, wchar_t* ws );
void* pany_set_ptr( pany* val, void* ptr );


pboolean parray_init( parray* array, size_t size, size_t chunk );
parray* parray_create( size_t size, size_t chunk );
pboolean parray_erase( parray* array );
parray* parray_free( parray* array );
void* parray_push( parray* array, void* item );
pboolean parray_reserve( parray* array, size_t n );
void* parray_malloc( parray* array );
void* parray_rmalloc( parray* array );
void* parray_insert( parray* array, size_t offset, void* item );
void* parray_remove( parray* array, size_t offset, void** item );
void* parray_pop( parray* array );
void* parray_unshift( parray* array, void* item );
void* parray_shift( parray* array );
void* parray_get( parray* array, size_t offset );
void* parray_put( parray* array, size_t offset, void* item );
void* parray_rget( parray* array, size_t offset );
void* parray_rput( parray* array, size_t offset, void* item );
void parray_iter( parray* array, parrayfn callback );
void parray_riter( parray* array, parrayfn callback );
void* parray_first( parray* array );
void* parray_last( parray* array );
void* parray_next( parray* array, void* ptr );
void* parray_prev( parray* array, void* ptr );
void* parray_swap( parray* array, size_t pos1, size_t pos2 );
size_t parray_count( parray* array );
pboolean parray_partof( parray* array, void* ptr );
size_t parray_offset( parray* array, void* ptr );


pccl* p_ccl_create( int min, int max, char* ccldef );
pboolean p_ccl_compat( pccl* l, pccl* r );
int p_ccl_size( pccl* ccl );
int p_ccl_count( pccl* ccl );
pccl* p_ccl_dup( pccl* ccl );
pboolean p_ccl_testrange( pccl* ccl, wchar_t begin, wchar_t end );
pboolean p_ccl_test( pccl* ccl, wchar_t ch );
pboolean p_ccl_instest( pccl* ccl, wchar_t ch );
pboolean p_ccl_addrange( pccl* ccl, wchar_t begin, wchar_t end );
pboolean p_ccl_add( pccl* ccl, wchar_t ch );
pboolean p_ccl_delrange( pccl* ccl, wchar_t begin, wchar_t end );
pboolean p_ccl_del( pccl* ccl, wchar_t ch );
pccl* p_ccl_negate( pccl* ccl );
pccl* p_ccl_union( pccl* ccl, pccl* add );
pccl* p_ccl_diff( pccl* ccl, pccl* rem );
int p_ccl_compare( pccl* left, pccl* right );
pccl* p_ccl_intersect( pccl* ccl, pccl* within );
pboolean p_ccl_get( wchar_t* from, wchar_t* to, pccl* ccl, int offset );
size_t p_ccl_parsechar( wchar_t* retc, char *str, pboolean escapeseq );
pboolean p_ccl_parseshorthand( pccl* ccl, char **str );
pboolean p_ccl_parse( pccl* ccl, char* ccldef, pboolean extend );
pboolean p_ccl_erase( pccl* ccl );
pccl* p_ccl_free( pccl* ccl );
char* p_ccl_to_str( pccl* ccl, pboolean escape );
void p_ccl_print( FILE* stream, pccl* ccl, int break_after );


pboolean _dbg_trace_enabled( char* file, char* function );
void _dbg_trace( char* file, int line, char* type, char* function, char* format, ... );


pboolean plist_init( plist* list, size_t size, int flags );
plist* plist_create( size_t size, int flags );
plist* plist_dup( plist* list );
pboolean plist_erase( plist* list );
pboolean plist_clear( plist* list );
plist* plist_free( plist* list );
plistel* plist_insert( plist* list, plistel* pos, char* key, void* src );
plistel* plist_push( plist* list, void* src );
plistel* plist_shift( plist* list, void* src );
void* plist_malloc( plist* list );
void* plist_rmalloc( plist* list );
pboolean plist_remove( plist* list, plistel* e );
pboolean plist_pop( plist* list, void* dest );
pboolean plist_unshift( plist* list, void* dest );
plistel* plist_get( plist* list, size_t n );
plistel* plist_rget( plist* list, size_t n );
plistel* plist_get_by_key( plist* list, char* key );
plistel* plist_get_by_ptr( plist* list, void* ptr );
int plist_concat( plist* dest, plist* src );
void plist_iter( plist* list, plistelfn callback );
void plist_riter( plist* list, plistelfn callback );
void plist_iter_access( plist* list, plistfn callback );
void plist_riter_access( plist* list, plistfn callback );
int plist_union( plist* all, plist* from );
int plist_diff( plist* left, plist* right );
pboolean plist_subsort( plist* list, plistel* from, plistel* to );
pboolean plist_sort( plist* list );
pboolean plist_set_comparefn( plist* list, int (*comparefn)( plist*, plistel*, plistel* ) );
pboolean plist_set_sortfn( plist* list, int (*sortfn)( plist*, plistel*, plistel* ) );
pboolean plist_set_printfn( plist* list, void (*printfn)( plist* ) );
void* plist_access( plistel* e );
char* plist_key( plistel* e );
plistel* plist_next( plistel* u );
plistel* plist_prev( plistel* u );
plistel* plist_hashnext( plistel* u );
plistel* plist_hashprev( plistel* u );
int plist_offset( plistel* u );
pboolean plist_swap( plistel* a, plistel* b );
plistel* plist_first( plist* l );
plistel* plist_last( plist* l );
int plist_size( plist* l );
int plist_count( plist* l );


void* pmalloc( size_t size );
void* prealloc( void* oldptr, size_t size );
void* pfree( void* ptr );
void* pmemdup( void* ptr, size_t size );


char* pwhich( char* filename, char* directories );
char* pbasename( char* path );
pboolean pfileexists( char* filename );
pboolean pfiletostr( char** cont, char* filename );
int pgetopt( char* opt, char** param, int* next, int argc, char** argv, char* optstr, char* loptstr, int idx );


ppast* pp_ast_create( char* emit, ppsym* sym, ppprod* prod, char* start, char* end, int row, int col, ppast* child );
ppast* pp_ast_free( ppast* node );
int pp_ast_len( ppast* node );
ppast* pp_ast_get( ppast* node, int n );
ppast* pp_ast_select( ppast* node, char* emit, int n );
void pp_ast_eval( ppast* ast, pastevalfn func );
void pp_ast_dump( FILE* stream, ppast* ast );
void pp_ast_dump_short( FILE* stream, ppast* ast );
void _dbg_ast_dump( char* file, int line, char* function, char* name, ppast* ast );
void pp_ast_dump_json( FILE* stream, ppast* ast );
void pp_ast_dump_tree2svg( FILE* stream, ppast* ast );
void pp_ast_dump_pvm( pvmprog* prog, ppast* ast );


pboolean pp_gram_from_bnf( ppgram* g, char* src );
pboolean pp_gram_from_ebnf( ppgram* g, char* src );


ppgram* pp_gram_create( void );
pboolean pp_gram_prepare( ppgram* g );
void _dbg_gram_dump( char* file, int line, char* function, char* name, ppgram* g );
char* pp_gram_to_str( ppgram* grm );
ppgram* pp_gram_free( ppgram* g );


pboolean pp_lr_build( unsigned int* cnt, unsigned int*** dfa, ppgram* grm );


pppar* pp_par_create( ppgram* g );
pppar* pp_par_free( pppar* p );
int pp_par_autolex( pppar* p );
pboolean pp_par_lex( pppar* p, ppsym* sym, char* pat, int flags );
pboolean pp_par_parse( ppast** root, pppar* par, char* start );


pboolean pp_gram_from_pbnf( ppgram* g, char* src );


ppprod* pp_prod_create( ppgram* g, ppsym* lhs, ... );
ppprod* pp_prod_free( ppprod* p );
ppprod* pp_prod_get( ppgram* g, int n );
pboolean pp_prod_append( ppprod* p, ppsym* sym );
int pp_prod_remove( ppprod* p, ppsym* sym );
ppsym* pp_prod_getfromrhs( ppprod* p, int off );
char* pp_prod_to_str( ppprod* p );


ppsym* pp_sym_create( ppgram* g, char* name, unsigned int flags );
ppsym* pp_sym_free( ppsym* sym );
ppsym* pp_sym_drop( ppsym* sym );
ppsym* pp_sym_get( ppgram* g, unsigned int n );
ppsym* pp_sym_get_by_name( ppgram* g, char* name );
ppsym* pp_sym_get_nameless_term_by_def( ppgram* g, char* name );
ppprod* pp_sym_getprod( ppsym* sym, unsigned int n );
char* pp_sym_to_str( ppsym* sym );
ppsym* pp_sym_mod_positive( ppsym* sym );
ppsym* pp_sym_mod_optional( ppsym* sym );
ppsym* pp_sym_mod_kleene( ppsym* sym );


void pregex_dfa_print( pregex_dfa* dfa );
pregex_dfa* pregex_dfa_create( void );
pboolean pregex_dfa_reset( pregex_dfa* dfa );
pregex_dfa* pregex_dfa_free( pregex_dfa* dfa );
int pregex_dfa_from_nfa( pregex_dfa* dfa, pregex_nfa* nfa );
int pregex_dfa_minimize( pregex_dfa* dfa );
int pregex_dfa_match( pregex_dfa* dfa, char* str, size_t* len, int* mflags, prange** ref, int* ref_count, int flags );
int pregex_dfa_to_dfatab( wchar_t*** dfatab, pregex_dfa* dfa );


int pregex_qmatch( char* regex, char* str, int flags, parray** matches );
int pregex_qsplit( char* regex, char* str, int flags, parray** matches );
char* pregex_qreplace( char* regex, char* str, char* replace, int flags );


plex* plex_create( int flags );
plex* plex_free( plex* lex );
pboolean plex_reset( plex* lex );
pboolean plex_prepare( plex* lex );
pregex_ptn* plex_define( plex* lex, char* pat, int match_id, int flags );
int plex_lex( plex* lex, char* start, char** end );
char* plex_next( plex* lex, char* start, unsigned int* id, char** end );
size_t plex_tokenize( plex* lex, char* start, parray** matches );


pboolean pregex_check_anchors( char* all, char* str, size_t len, int anchors, int flags );


pregex_nfa_st* pregex_nfa_create_state( pregex_nfa* nfa, char* chardef, int flags );
void pregex_nfa_print( pregex_nfa* nfa );
pregex_nfa* pregex_nfa_create( void );
pboolean pregex_nfa_reset( pregex_nfa* nfa );
pregex_nfa* pregex_nfa_free( pregex_nfa* nfa );
int pregex_nfa_move( pregex_nfa* nfa, plist* hits, wchar_t from, wchar_t to );
int pregex_nfa_epsilon_closure( pregex_nfa* nfa, plist* closure, unsigned int* accept, int* flags );
int pregex_nfa_match( pregex_nfa* nfa, char* str, size_t* len, int* mflags, prange** ref, int* ref_count, int flags );
pboolean pregex_nfa_from_string( pregex_nfa* nfa, char* str, int flags, int acc );


pregex_ptn* pregex_ptn_create_char( pccl* ccl );
pregex_ptn* pregex_ptn_create_string( char* str, int flags );
pregex_ptn* pregex_ptn_create_sub( pregex_ptn* ptn );
pregex_ptn* pregex_ptn_create_refsub( pregex_ptn* ptn );
pregex_ptn* pregex_ptn_create_alt( pregex_ptn* left, ... );
pregex_ptn* pregex_ptn_create_kle( pregex_ptn* ptn );
pregex_ptn* pregex_ptn_create_pos( pregex_ptn* ptn );
pregex_ptn* pregex_ptn_create_opt( pregex_ptn* ptn );
pregex_ptn* pregex_ptn_create_seq( pregex_ptn* first, ... );
pregex_ptn* pregex_ptn_dup( pregex_ptn* ptn );
pregex_ptn* pregex_ptn_free( pregex_ptn* ptn );
void pregex_ptn_print( pregex_ptn* ptn, int rec );
char* pregex_ptn_to_regex( pregex_ptn* ptn );
pboolean pregex_ptn_to_nfa( pregex_nfa* nfa, pregex_ptn* ptn );
pboolean pregex_ptn_to_dfa( pregex_dfa* dfa, pregex_ptn* ptn );
int pregex_ptn_to_dfatab( wchar_t*** dfatab, pregex_ptn* ptn );
pboolean pregex_ptn_parse( pregex_ptn** ptn, char* str, int flags );


pregex* pregex_create( char* pat, int flags );
pregex* pregex_free( pregex* regex );
pboolean pregex_match( pregex* regex, char* start, char** end );
char* pregex_find( pregex* regex, char* start, char** end );
int pregex_findall( pregex* regex, char* start, parray** matches );
char* pregex_split( pregex* regex, char* start, char** end, char** next );
int pregex_splitall( pregex* regex, char* start, parray** matches );
char* pregex_replace( pregex* regex, char* str, char* replacement );


char* pwcs_to_str( wchar_t* str, pboolean freestr );
wchar_t* pstr_to_wcs( char* str, pboolean freestr );
char* pdbl_to_str( double d );
#ifdef UNICODE
wchar_t* pdbl_to_wcs( double d );
#endif


char* pstrcatchar( char* str, char chr );
char* pstrcatstr( char* dest, char* src, pboolean freesrc );
char* pstrncatstr( char* str, char* append, size_t n );
char* pstrreplace( char* str, char* find, char* replace );
char* pstrdup( char* str );
char* pstrndup( char* str, size_t len );
size_t pstrlen( char* str );
char* pstrput( char** str, char* val );
char* pstrget( char* str );
char* pstrrender( char* tpl, ... );
char* pstrltrim( char* s );
char* pstrrtrim( char* s );
char* pstrtrim( char* s );
int pstrsplit( char*** tokens, char* str, char* sep, int limit );
char* pstrupr( char* s );
char* pstrlwr( char* s );
int pstrcasecmp( char* s1, char* s2 );
int pstrncasecmp( char* s1, char* s2, size_t n );
char* pstrunescape( char* str );
int pvasprintf( char** str, char* fmt, va_list ap );
char* pasprintf( char* fmt, ... );
#ifdef UNICODE
wchar_t* pwcsdup( wchar_t* str );
wchar_t* pwcscatchar( wchar_t* str, wchar_t chr );
wchar_t* pwcscatstr( wchar_t* dest, wchar_t* src, pboolean freesrc );
wchar_t* pwcsncatstr( wchar_t* str, wchar_t* append, size_t n );
size_t pwcslen( wchar_t* str );
wchar_t* pwcsput( wchar_t** str, wchar_t* val );
wchar_t* pwcsget( wchar_t* str );
wchar_t* pwcsndup( wchar_t* str, size_t len );
int pvawcsprintf( wchar_t** str, wchar_t* fmt, va_list ap );
wchar_t* pawcsprintf( wchar_t* fmt, ... );
#endif 


pboolean u8_isutf( unsigned char c );
int u8_seqlen(char *s);
wchar_t u8_char( char* str );
char* u8_move( char* str, int count );
wchar_t u8_parse_char( char** ch );
int u8_toucs(wchar_t *dest, int sz, char *src, int srcsz);
int u8_toutf8(char *dest, int sz, wchar_t *src, int srcsz);
int u8_wc_toutf8(char *dest, wchar_t ch);
int u8_offset(char *str, int charnum);
int u8_charnum(char *s, int offset);
int u8_strlen(char *s);
wchar_t u8_nextchar(char *s, int *i);
void u8_inc(char *s, int *i);
void u8_dec(char *s, int *i);
int octal_digit(char c);
int hex_digit(char c);
int u8_read_escape_sequence(char *str, wchar_t *dest);
int u8_unescape(char *buf, int sz, char *src);
int u8_escape_wchar(char *buf, int sz, wchar_t ch);
int u8_escape(char *buf, int sz, char *src, int escape_quotes);
char *u8_strchr(char *s, wchar_t ch, int *charn);
char *u8_memchr(char *s, wchar_t ch, size_t sz, int *charn);
int u8_is_locale_utf8(char *locale);


pvmprog* pvm_prog_create( pvm* vm, char* src );
pvmprog* pvm_prog_free( pvmprog* prog );
pboolean pvm_prog_compile( pvmprog* prog, char* src );
void pvm_prog_dump( pvmprog* prog );


void pvm_prog_run( pany** ret, pvmprog* prog );


pvm* pvm_init( pvm* vm );
pvm* pvm_create( void );
pvm* pvm_reset( pvm* vm );
pvm* pvm_free( pvm* vm );
int pvm_define( pvm* vm, char* mn, pvmop op );



#ifdef __cplusplus
}
#endif

#endif 

