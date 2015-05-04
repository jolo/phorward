/* -MODULE----------------------------------------------------------------------
Phorward Foundation Toolkit
Copyright (C) 2006-2015 by Phorward Software Technologies, Jan Max Meyer
http://www.phorward-software.com ++ contact<at>phorward<dash>software<dot>com
All rights reserved. See LICENSE for more information.

File:	lex.c
Usage:	Writing lexical analyzers.
----------------------------------------------------------------------------- */

#include "phorward.h"

/** Constructor function to create a new plex object.

//flags// can be a combination of compile- and runtime-flags and are merged
with special compile-time flags provided for each pattern.

|| Flag | Usage |
| PREGEX_COMP_WCHAR	| The regular expressions are provided as wchar_t. |
| PREGEX_COMP_NOANCHORS	| Ignore anchor tokens, handle them as normal \
characters |
| PREGEX_COMP_NOREF | Don't compile references. |
| PREGEX_COMP_NONGREEDY | Compile all patterns to be forced nongreedy. |
| PREGEX_COMP_NOERRORS | Don't report errors, and try to compile as much as \
possible |
| PREGEX_COMP_INSENSITIVE | Parse regular expressions as case insensitive. |
| PREGEX_COMP_STATIC | The regular expressions passed should be converted 1:1 \
as it where a string-constant. Any regex-specific symbols will be ignored and \
taken as they where escaped. |
| PREGEX_RUN_WCHAR | Run regular expressions with wchar_t as input. |
| PREGEX_RUN_NOANCHORS | Ignore anchors while processing the lexer. |
| PREGEX_RUN_NOREF | Don't create references. |
| PREGEX_RUN_NONGREEDY | Force run lexer nongreedy. |
| PREGEX_RUN_DEBUG | Debug mode; output some debug to stderr. |


On success, the function returns the allocated pointer to a plex-object.
This must be freed later using plex_free().
*/
plex* plex_create( int flags )
{
	plex*	lex;

	PROC( "plex_create" );
	PARMS( "flags", "%d", flags );

	lex = (plex*)pmalloc( sizeof( plex ) );
	lex->ptns = plist_create( 0, PLIST_MOD_PTR );
	lex->flags = flags;

	RETURN( lex );
}

/** Destructor function for a plex-object.

//lex// is the pointer to a plex-structure that will be released.

Returns always (plex*)NULL.
*/
plex* plex_free( plex* lex )
{
	plistel*	e;

	PROC( "pregex_free" );
	PARMS( "lex", "%p", lex );

	if( !lex )
		RETURN( (plex*)NULL );

	plist_for( lex->ptns, e )
		pregex_ptn_free( (pregex_ptn*)plist_access( e ) );

	plist_free( lex->ptns );

	plex_reset( lex );
	pfree( lex );

	RETURN( (plex*)NULL );
}

/** Resets the DFA state machine of a plex-object //lex//. */
pboolean plex_reset( plex* lex )
{
	int		i;

	PROC( "plex_reset" );
	PARMS( "lex", "%p", lex );

	if( !lex )
	{
		WRONGPARAM;
		RETURN( FALSE );
	}

	/* Drop out the dfatab */
	for( i = 0; i < lex->trans_cnt; i++ )
		pfree( lex->trans[ i ] );

	lex->trans_cnt = 0;
	lex->trans = pfree( lex->trans );

	RETURN( TRUE );
}

/** Prepares the DFA state machine of a plex-object //lex// for execution. */
pboolean plex_prepare( plex* lex )
{
	plistel*	e;
	pregex_nfa*	nfa;
	pregex_dfa*	dfa;

	PROC( "plex_prepare" );
	PARMS( "lex", "%p", lex );

	if( !lex )
	{
		WRONGPARAM;
		RETURN( FALSE );
	}

	if( !plist_count( lex->ptns ) )
	{
		MSG( "Can't construct a DFA from nothing!" );
		RETURN( FALSE );
	}

	plex_reset( lex );

	/* Create a NFA from patterns */
	nfa = pregex_nfa_create();

	plist_for( lex->ptns, e )
		if( !pregex_ptn_to_nfa( nfa, (pregex_ptn*)plist_access( e ) ) )
		{
			pregex_nfa_free( nfa );
			RETURN( FALSE );
		}

	/* Create a minimized DFA from NFA */
	dfa = pregex_dfa_create();

	if( pregex_dfa_from_nfa( dfa, nfa ) <= 0
			&& pregex_dfa_minimize( dfa ) <= 0 )
	{
		pregex_nfa_free( nfa );
		pregex_dfa_free( dfa );
		RETURN( FALSE );
	}

	pregex_nfa_free( nfa );

	/* Compile significant DFA table into dfatab array */
	if( ( lex->trans_cnt = pregex_dfa_to_dfatab( &lex->trans, dfa ) ) <= 0 )
	{
		pregex_dfa_free( dfa );
		RETURN( FALSE );
	}

	pregex_dfa_free( dfa );

	RETURN( TRUE );
}

/** Defines and parses a regular expression pattern into the plex-object.

//pat// is the regular expression string.

//match_id// must be a token match ID, a value > 0. The lower the match ID is,
the higher precedence takes the appended expression when there are multiple
matches.
matches.

//flags// may ONLY contain compile-time flags, and is combined with the
compile-time flags of the plex-object provided at plex_create().

|| Flag | Usage |
| PREGEX_COMP_WCHAR	| The regular expressions are provided as wchar_t. |
| PREGEX_COMP_NOANCHORS	| Ignore anchor tokens, handle them as normal \
characters |
| PREGEX_COMP_NOREF | Don't compile references. |
| PREGEX_COMP_NONGREEDY | Compile all patterns to be forced nongreedy. |
| PREGEX_COMP_NOERRORS | Don't report errors, and try to compile as much as \
possible |
| PREGEX_COMP_INSENSITIVE | Parse regular expressions as case insensitive. |
| PREGEX_COMP_STATIC | The regular expressions passed should be converted 1:1 \
as it where a string-constant. Any regex-specific symbols will be ignored and \
taken as they where escaped. |

*/
pboolean plex_define( plex* lex, char* pat, int match_id, int flags )
{
	pregex_ptn*	ptn;

	PROC( "plex_define" );
	PARMS( "lex", "%p", lex );

	if( !( lex && pat && match_id > 0 ) )
	{
		WRONGPARAM;
		RETURN( FALSE );
	}

	if( !pregex_ptn_parse( &ptn, pat, lex->flags | flags ) )
		RETURN( FALSE );

	ptn->accept->accept = match_id;
	plist_push( lex->ptns, ptn );

	plex_reset( lex );
	RETURN( TRUE );
}


/** Tries to match the lexer //lex// at pointer //start//.

If the expression can be matched, the function returns TRUE and //end// receives
the pointer to the last matched character. */
int plex_match( plex* lex, char* start, char** end )
{
	int		i;
	int		state		= 0;
	int		next_state;
	char*	match		= (char*)NULL;
	char*	ptr			= start;
	wchar_t	ch;
	int		id			= 0;

	PROC( "plex_match" );
	PARMS( "lex", "%p", lex );
	PARMS( "start", "%s", start );
	PARMS( "end", "%p", end );

	if( !( lex && start ) )
	{
		WRONGPARAM;
		RETURN( 0 );
	}

	if( !lex->trans_cnt )
		plex_prepare( lex );

	memset( lex->ref, 0, PREGEX_MAXREF * sizeof( prange ) );

	while( state >= 0 )
	{
		/* State accepts? */
		if( lex->trans[ state ][ 1 ] )
		{
			MSG( "This state accepts the input" );
			match = ptr;
			id = lex->trans[ state ][ 1 ];

			if( ( lex->flags & PREGEX_RUN_NONGREEDY
					|| lex->trans[ state ][ 2 ] & PREGEX_FLAG_NONGREEDY ) )
				break;
		}

		/* References */
		if( lex->trans[ state ][ 3 ] )
		{
			for( i = 0; i < PREGEX_MAXREF; i++ )
			{
				if( lex->trans[ state ][ 3 ] & ( 1 << i ) )
				{
					if( !lex->ref[ i ].begin )
						lex->ref[ i ].begin = ptr;

					lex->ref[ i ].end = ptr;
				}
			}
		}

		/* Get next character */
		if( lex->flags & PREGEX_RUN_WCHAR )
		{
			VARS( "pstr", "%ls", (wchar_t*)ptr );
			ch = *( (wchar_t*)ptr );
			ptr += sizeof( wchar_t );
		}
		else
		{
			VARS( "pstr", "%s", ptr );

			if( ( lex->flags & PREGEX_RUN_UCHAR ) )
				ch = (unsigned char)*ptr++;
			else
			{
#ifdef UTF8
				ch = u8_char( ptr );
				ptr += u8_seqlen( ptr );
#else
				ch = *ptr++;
#endif
			}
		}

		/* Initialize default transition */
		next_state = lex->trans[ state ][ 4 ];

		/* Find transition according to current character */
		for( i = 5; i < lex->trans[ state ][ 0 ]; i += 3 )
		{
			if( lex->trans[ state ][ i ] <= ch
					&& lex->trans[ state ][ i + 1 ] >= ch )
			{
				next_state = lex->trans[ state ][ i + 2 ];
				break;
			}
		}

		if( next_state == lex->trans_cnt )
			break;

		if( lex->flags & PREGEX_RUN_DEBUG )
		{
			if( lex->flags & PREGEX_RUN_WCHAR )
				fprintf( stderr,
					"state %d, wchar_t %d (>%lc<), next state %d\n",
						state, ch, ch, next_state );
			else
				fprintf( stderr,
					"state %d, char %d (>%c<), next state %d\n",
						state, ch, ch, next_state );
		}

		state = next_state;
	}

	if( match )
	{
		if( end )
			*end = match;

		/*
		for( i = 0; i < PREGEX_MAXREF; i++ )
			if( lex->ref[ i ].begin )
				fprintf( stderr, "%2d: >%.*s<\n",
					i, lex->ref[ i ].end - lex->ref[ i ].begin,
						lex->ref[ i ].begin );
		*/

		RETURN( id );
	}

	RETURN( 0 );
}

/** Find a match for the lexer //lex// from begin of pointer //start//.

//start// has to be a zero-terminated string or wide-character string (according
to the configuration of the plex-object).

If the expression can be matched, the function returns the pointer to the
position where the match begins. //id// receives the matching ID, when provided,
//end// receives the end pointer of the match, when provided.

The function returns (char*)NULL in case that there is no match.
*/
char* plex_find( plex* lex, char* start, int* id, char** end )
{
	wchar_t		ch;
	char*		ptr 	= start;
	char*		lptr;
	int			i;
	int			mid;

	PROC( "plex_find" );
	PARMS( "lex", "%p", lex );
	PARMS( "start", "%s", start );
	PARMS( "end", "%p", end );

	if( !( lex && start ) )
	{
		WRONGPARAM;
		RETURN( (char*)NULL );
	}

	if( !lex->trans_cnt )
		plex_prepare( lex );

	do
	{
		lptr = ptr;

		/* Get next character */
		if( lex->flags & PREGEX_RUN_WCHAR )
		{
			ch = *( (wchar_t*)ptr );
			ptr += sizeof( wchar_t );
		}
		else
		{
#ifdef UTF8
			ch = u8_char( ptr );
			ptr += u8_seqlen( ptr );
#else
			ch = *ptr++;
#endif
		}

		if( !ch )
			break;

		/* Find a transition according to current character */
		for( i = 5; i < lex->trans[ 0 ][ 0 ]; i += 3 )
			if( ( ( lex->trans[ 0 ][ 4 ] < lex->trans_cnt )
					|| ( lex->trans[ 0 ][ i ] <= ch
							&& lex->trans[ 0 ][ i + 1 ] >= ch ) )
					&& ( mid = plex_match( lex, lptr, end ) ) )
					{
						if( id )
							*id = mid;

						RETURN( lptr );
					}
	}
	while( ch );

	RETURN( (char*)NULL );
}

/** Find all matches for the regular expression //lex// from begin of pointer
//start//, and optionally return matches as an array.

//start// has to be a zero-terminated string or wide-character string (according
to the configuration of the plex-object).

The function fills the array //matches//, if provided, with items of size
prange. It returns the total number of matches.
*/
int plex_findall( plex* lex, char* start, parray** matches )
{
	char*			end;
	int				count	= 0;
	int				id;
	prange*	r;

	PROC( "plex_findall" );
	PARMS( "lex", "%p", lex );
	PARMS( "start", "%s", start );
	PARMS( "matches", "%p", matches );

	if( !( lex && start ) )
	{
		WRONGPARAM;
		RETURN( -1 );
	}

	if( matches )
		*matches = (parray*)NULL;

	while( ( start = plex_find( lex, start, &id, &end ) ) )
	{
		if( matches )
		{
			if( ! *matches )
				*matches = parray_create( sizeof( prange ), 0 );

			r = (prange*)parray_malloc( *matches );
			r->id = id;
			r->begin = start;
			r->end = end;
		}

		start = end;
		count++;
	}

	RETURN( count );
}
