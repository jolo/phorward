/* -MODULE----------------------------------------------------------------------
Phorward Foundation Libraries :: Regular Expression Library, Version 2
Copyright (C) 2009-2011 by Phorward Software Technologies, Jan Max Meyer
http://www.phorward-software.com ++ contact<at>phorward<dash>software<dot>com
All rights reserved. See $PHOME/LICENSE for more information.

File:	comp.c
Author:	Jan Max Meyer
Usage:	Compilation of multiple patterns into one state machine structure
----------------------------------------------------------------------------- */

/*
 * Includes
 */
#include <phorward.h>

#define IS_EXECUTABLE( stat )	( (stat) == REGEX_STAT_NFA || \
									(stat) == REGEX_STAT_DFA )

/*
 * Global variables
 */

/*
 * Functions
 */

/* -FUNCTION--------------------------------------------------------------------
	Function:		pregex_comp_init()
	
	Author:			Jan Max Meyer
	
	Usage:			Initializes a pregex-structure. This structure can be used
					to compile multiple regular expressions into one machine.
					
	Parameters:		pregex*			machine		Pointer to a pregex-structure,
												which will be initizalized for
												further usage.
					int				flags		Flags to modifiy compiler- and
												execution-options. Use the
												REGEX_MOD_-flags. To specify
												more than one flag, use the 
												or-operator (|).
																	
	Returns:		void
  
	~~~ CHANGES & NOTES ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	Date:		Author:			Note:
----------------------------------------------------------------------------- */
void pregex_comp_init( pregex* machine, int flags )
{
	PROC( "pregex_comp_init" );
	PARMS( "machine", "%p", machine );

	memset( machine, 0, sizeof( pregex ) );
	machine->stat = REGEX_STAT_NONE;
	machine->flags = flags;

	VOIDRET;
}

/* -FUNCTION--------------------------------------------------------------------
	Function:		pregex_comp_compile()
	
	Author:			Jan Max Meyer
	
	Usage:			Compiles a regular expression into an NFA within
					a machine.
					
	Parameters:		pregex*			machine		Pointer to a pregex-structure,
												that will hold the compiled 
												pattern.
					uchar*			pattern		The pattern to be compiled.
					int				accept		Accepting ID if the pattern
												is correctly matched.
																	
	Returns:		int				ERR_OK		on success
									ERR_FAILURE	if the machine is not
												initialized or already turned
												into a different state.
									ERR...		error define else
  
	~~~ CHANGES & NOTES ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	Date:		Author:			Note:
----------------------------------------------------------------------------- */
int pregex_comp_compile( pregex* machine, uchar* pattern, int accept )
{
	int		ret;

	PROC( "pregex_comp_compile" );
	PARMS( "machine", "%p", machine );
	PARMS( "pattern", "%s", pattern );
	PARMS( "accept", "%d",accept );

	if( !( machine->stat == REGEX_STAT_NONE 
			|| machine->stat == REGEX_STAT_NFA ) )
	{
		MSG( "The machine is not inizialized or already turned into DFA!" );
		RETURN( ERR_FAILURE );
	}

	if( machine->stat == REGEX_STAT_NONE )
	{
		MSG( "Machine is only self-inizialized yet - inizializing NFA!" );
		memset( &( machine->machine.nfa ), 0, sizeof( pregex_nfa ) );
		machine->stat = REGEX_STAT_NFA;
	}
	
	ret = pregex_compile_to_nfa( pattern,
				&( machine->machine.nfa ), machine->flags, accept );

	RETURN( ret );
}

/* -FUNCTION--------------------------------------------------------------------
	Function:		pregex_comp_finalize()
	
	Author:			Jan Max Meyer
	
	Usage:			Finalizes a pregex-compiled machine to a minimized DFA.
					After finalization, new expressions can't be added.
					
	Parameters:		pregex*			machine		Pointer to a pregex-structure,
												that will be finalized.
																	
	Returns:		int				ERR_OK		on success
									ERR_FAILURE	if the machine is not in
												compiled-state
									ERR...		error define else
  
	~~~ CHANGES & NOTES ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	Date:		Author:			Note:
	17.02.2011	Jan Max Meyer	Finalize NFA to DFA now activated.
----------------------------------------------------------------------------- */
int pregex_comp_finalize( pregex* machine )
{
	pregex_dfa	dfa;
	int			ret;

	PROC( "pregex_comp_finalize" );
	PARMS( "machine", "%p", machine );
	
	/* This will be enabled later! */

	if( !( machine->stat == REGEX_STAT_NFA ) )
	{
		MSG( "The machine must be in compiled state." );
		RETURN( ERR_FAILURE );
	}
	
	memset( &dfa, 0, sizeof( pregex_dfa ) );

	/* Perform subset construction algorithm */
	if( ( ret = pregex_dfa_from_nfa( &dfa, &( machine->machine.nfa ) ) )
			< ERR_OK )
	{
		MSG( "Subset construction failed" );
		pregex_dfa_free( &dfa );

		RETURN( ret );
	}

	/* Perform DFA minimization */
	/*
	if( ( ret = pregex_dfa_minimize( &dfa ) ) != ERR_OK )
	{
		MSG( "DFA minimization failed" );
		pregex_dfa_free( &dfa );

		RETURN( ret );
	}
	*/

	/* Delete NFA */
	/*
	pregex_nfa_print( &( machine->machine.nfa ) );
	*/
	pregex_nfa_free( &( machine->machine.nfa ) );

	/* Set new machine status */
	/*
	pregex_dfa_print( stderr, &dfa );
	*/
	memcpy( &( machine->machine.dfa ), &dfa, sizeof( pregex_dfa ) );
	machine->stat = REGEX_STAT_DFA;

	RETURN( ERR_OK );
}

/* -FUNCTION--------------------------------------------------------------------
	Function:		pregex_comp_free()
	
	Author:			Jan Max Meyer
	
	Usage:			Frees a pregex-structure and resets it.
					
	Parameters:		pregex*			machine		Pointer to a pregex-structure,
												that will be reset.
																	
	Returns:		void
  
	~~~ CHANGES & NOTES ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	Date:		Author:			Note:
----------------------------------------------------------------------------- */
void pregex_comp_free( pregex* machine )
{
	PROC( "pregex_comp_free" );
	PARMS( "machine", "%p", machine );
	
	switch( machine->stat )
	{
		case REGEX_STAT_NFA:
			pregex_nfa_free( &( machine->machine.nfa ) );
			break;

		case REGEX_STAT_DFA:
			pregex_dfa_free( &( machine->machine.dfa ) );
			break;

		default:
			break;
	}

	pregex_comp_init( machine, REGEX_MOD_NONE );

	VOIDRET;
}

/* -FUNCTION--------------------------------------------------------------------
	Function:		pregex_comp_match()
	
	Author:			Jan Max Meyer
	
	Usage:			Performs a regular expression search with a pre-compiled
					regex on a string, and returns an array of matches via a
					pregex_result-structure, which holds pointers to the begin-
					and end-addresses of the matches.
					
	Parameters:		pregex*			machine		Pointer to a pre-compiled
												regex state machine.
					uchar*			str			Pointer to input string where
												the pattern will be executed on.
					pregex_callback	fn			A callback-function which
												is called for each match.
												The returned value is set for
												the match within the results
												array, if it is greater or equal
												to 0. If the return value is
												negative, the match will be
												ignored. Use the macro
												REGEX_NO_CALLBACK to disable a
												callback-function usage.
					pregex_result**	results		Array of results to the
												matched substrings within
												str. results must be freed
												after usage. It will even hold
												the accepting IDs of the
												matching expressions.
																	
	Returns:		int							Returns the amount of matches,
												which is the amount of items
												within the returned
												results-array. If the 
												value is negative,
												it is an error define.
  
	~~~ CHANGES & NOTES ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	Date:		Author:			Note:
	17.02.2011	Jan Max Meyer	Allowed to run both NFA and DFA machines
----------------------------------------------------------------------------- */
int pregex_comp_match( pregex* machine, uchar* str, pregex_callback fn,
							pregex_result** results )
{
	pregex_result	tmp_result;
	int				match;
	int				matches	= 0;
	int				anchors;
	psize			len;
	uchar*			pstr;

	PROC( "pregex_comp_match" );
	PARMS( "machine", "%p", machine );

#ifdef __WITH_TRACE
	if( machine->flags & REGEX_MOD_WCHAR )
		PARMS( "str", "%ls", str );
	else
		PARMS( "str", "%s", str );
#endif

	PARMS( "fn", "%p", fn );
	PARMS( "results", "%p", results );

	if( !IS_EXECUTABLE( machine->stat ) )
		RETURN( ERR_UNIMPL );
	
	if( results )
		*results = (pregex_result*)NULL;

	for( pstr = str; *pstr; )
	{
		if( machine->flags & REGEX_MOD_WCHAR )
			PARMS( "pstr", "%ls", pstr );
		else
			PARMS( "pstr", "%s", pstr );

		if( machine->stat == REGEX_STAT_NFA )
			match = pregex_nfa_match( &( machine->machine.nfa ), pstr,
						&len, &anchors, (pregex_result**)NULL, (int*)NULL,
							machine->flags );
		else
			match = pregex_dfa_match( &( machine->machine.dfa ), pstr,
						&len, &anchors, (pregex_result**)NULL, (int*)NULL,
							machine->flags );
	
		if( match >= 0 && pregex_check_anchors( str, pstr, len,
							anchors, machine->flags ) )
		{
			MSG( "pregex_nfa_match found a match!" );
			VARS( "match", "%d", match );
			VARS( "len", "%ld", len );
			
			if( results || fn )
			{							
				memset( &tmp_result, 0, sizeof( pregex_result ) );
		
				tmp_result.accept = match;
				tmp_result.begin = pstr;
				tmp_result.end = pstr + len;
				tmp_result.pbegin = (pchar*)pstr;
				tmp_result.pend = (pchar*)pstr + len;
			
				if( machine->flags & REGEX_MOD_WCHAR )
					tmp_result.pos = (pchar*)pstr - (pchar*)str;
				else
					tmp_result.pos = pstr - str;

				tmp_result.len = len;
				VARS( "len of result", "%ld", tmp_result.len );
			}
			
			if( fn )
			{
				MSG( "Calling callback-function" );
				match = (*fn)( &tmp_result );
			}
			
			VARS( "match", "%d", match );
			if( match >= 0 )
			{
				if( results )
				{
					tmp_result.accept = match;

					if( !matches )
						*results = (pregex_result*)pmalloc( REGEX_ALLOC_STEP
										* sizeof( pregex_result ) );
					else if( !( matches % REGEX_ALLOC_STEP ) )
						*results = (pregex_result*)prealloc(
										(pregex_result*)*results,
										( ( ( matches / REGEX_ALLOC_STEP ) + 1 )
											* REGEX_ALLOC_STEP )
												*  sizeof( pregex_result ) );

					if( !*results )
						RETURN( ERR_MEM );
					
					memcpy( *results + matches, &tmp_result,
								sizeof( pregex_result ) );
				}

				if( machine->flags & REGEX_MOD_WCHAR )
					pstr += len * sizeof( pchar );
				else
				{
#ifdef UTF8
					for( ; len > 0; len-- )
						pstr += u8_seqlen( pstr );
#else
					pstr += len;
#endif
				}

				if( results )
				{
					(*results)[ matches ].end = pstr;
#ifdef UTF8
					if( !( machine->flags & REGEX_MOD_WCHAR ) )
						(*results)[ matches ].len =
								pstr - (*results)[ matches ].begin;
#endif
				}

				matches++;

				if( !( machine->flags & REGEX_MOD_GLOBAL ) )
					break;
				
				continue;
			}
		}

		if( machine->flags & REGEX_MOD_WCHAR )
			pstr += sizeof( pchar );
		else
		{
#ifdef UTF8
			pstr += u8_seqlen( pstr );
#else
			pstr++;
#endif
		}
	}

	VARS( "matches", "%d", matches );
	RETURN( matches );
}

/* -FUNCTION--------------------------------------------------------------------
	Function:		pregex_comp_split()

	Author:			Jan Max Meyer
	
	Usage:			Performs a regular expression with a pre-compiled machine
					on a string and uses the expression as separator; All
					strings that where split are returned as results-array.
					
					
	Parameters:		pregex*			machine		Pointer to a pre-compiled
												regex state machine.
					uchar*			str			Searchstring the pattern
												will be ran on.
					pregex_callback	fn			A callback-function which
												is called for each split match.
												If the return value is negative,
												the match will be ignored.
												Use the macro REGEX_NO_CALLBACK
												to disable a callback-function
												usage.
					pregex_result**	results		Array of results to the
												split substrings. Each element
												of this array contains begin-
												and end-pointer to the
												related strings within the
												input-string str.
																	
	Returns:		int							Returns the amount of matches,
												which is the amount of items
												within the returned
												results-array. If the 
												value is negative,
												it is an error define.
  
	~~~ CHANGES & NOTES ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	Date:		Author:			Note:
	17.02.2011	Jan Max Meyer	Allowed to run both NFA and DFA machines
----------------------------------------------------------------------------- */
int pregex_comp_split( pregex* machine, uchar* str, pregex_callback fn,
							pregex_result** results )
{
	pregex_result	tmp_result;
	int				match;
	int				matches	= 0;
	int				anchors;
	psize			len;
	uchar*			pstr;
	uchar*			prev;

	PROC( "pregex_comp_split" );
	PARMS( "machine", "%p", machine );
#ifdef __WITH_TRACE
	if( machine->flags & REGEX_MOD_WCHAR )
		PARMS( "str", "%ls", str );
	else
		PARMS( "str", "%s", str );
#endif
	PARMS( "fn", "%p", fn );
	PARMS( "results", "%p", results );

	if( !IS_EXECUTABLE( machine->stat ) )
		RETURN( ERR_UNIMPL );

	*results = (pregex_result*)NULL;

	for( prev = pstr = str; *pstr; )
	{
		VARS( "pstr", "%s", pstr );

		if( machine->stat == REGEX_STAT_NFA )
			match = pregex_nfa_match( &( machine->machine.nfa ), pstr,
						&len, &anchors, (pregex_result**)NULL, (int*)NULL,
							machine->flags );
		else
			match = pregex_dfa_match( &( machine->machine.dfa ), pstr,
						&len, &anchors, (pregex_result**)NULL, (int*)NULL,
							machine->flags );

		if( match >= 0 && pregex_check_anchors( str, pstr, len,
							anchors, machine->flags ) )
		{
			MSG( "Write information into temporary result structure" );
			
			if( fn )
			{
				memset( &tmp_result, 0, sizeof( pregex_result ) );

				tmp_result.accept = match;
				tmp_result.begin = pstr;
				tmp_result.end = pstr + len;
				tmp_result.pbegin = (pchar*)pstr;
				tmp_result.pend = (pchar*)pstr + len;
				tmp_result.len = len;

				if( machine->flags & REGEX_MOD_WCHAR )
					tmp_result.pos = (pchar*)pstr - (pchar*)str;
				else
					tmp_result.pos = pstr - str;
			
				MSG( "Calling callback-function" );
				match = (*fn)( &tmp_result );
			}
			
			VARS( "match", "%d", match );
			if( match >= 0 )
			{
				if( !matches )
					*results = (pregex_result*)pmalloc(
									REGEX_ALLOC_STEP *
										sizeof( pregex_result ) );
				else if( !( matches % REGEX_ALLOC_STEP ) )
					*results = (pregex_result*)prealloc(
									(pregex_result*)*results,
										( ( ( matches / REGEX_ALLOC_STEP ) + 1 )
											* REGEX_ALLOC_STEP )
												*  sizeof( pregex_result ) );
				if( !*results )
					RETURN( ERR_MEM );
					
				(*results)[ matches ].accept = match;
				(*results)[ matches ].begin = prev;
				(*results)[ matches ].end = pstr;
				(*results)[ matches ].pbegin = (pchar*)prev;
				(*results)[ matches ].pend = (pchar*)pstr;
				if( machine->flags & REGEX_MOD_WCHAR )
				{
					(*results)[ matches ].pos = (pchar*)prev - (pchar*)str;
					(*results)[ matches ].len = (pchar*)pstr - (pchar*)prev;
				}
				else
				{
					(*results)[ matches ].pos = prev - str;	
					(*results)[ matches ].len = pstr - prev;
				}
				(*results)[ matches ].user = tmp_result.user;

				matches++;

				if( machine->flags & REGEX_MOD_WCHAR )
					pstr += len * sizeof( pchar );
				else
				{
#ifdef UTF8
					for( ; len > 0; len-- )
						pstr += u8_seqlen( pstr );
#else
					pstr += len;
#endif
				}
				prev = pstr;

				if( !( machine->flags & REGEX_MOD_GLOBAL ) )
				{
					pstr += pstrlen( pstr );
					break;
				}
			
				continue;
			}
		}

		if( machine->flags & REGEX_MOD_WCHAR )
			pstr += sizeof( pchar );
		else
		{
#ifdef UTF8
			pstr += u8_seqlen( pstr );
#else
			pstr++;
#endif
		}
	}
	
	/* Put last one if required! */
	if( prev != pstr )
	{
		if( !matches )
			*results = (pregex_result*)pmalloc(
							REGEX_ALLOC_STEP * sizeof( pregex_result ) );
		else if( !( matches % REGEX_ALLOC_STEP ) )
			*results = (pregex_result*)prealloc(
							(pregex_result*)*results,
								( ( ( matches / REGEX_ALLOC_STEP ) + 1 )
									* REGEX_ALLOC_STEP )
										*  sizeof( pregex_result ) );

		if( !*results )
			RETURN( ERR_MEM );

		(*results)[ matches ].accept = -1;
		(*results)[ matches ].begin = prev;
		(*results)[ matches ].end = pstr;
		(*results)[ matches ].pbegin = (pchar*)prev;
		(*results)[ matches ].pend = (pchar*)pstr;
		if( machine->flags & REGEX_MOD_WCHAR )
		{
			(*results)[ matches ].pos = (pchar*)prev - (pchar*)str;
			(*results)[ matches ].len = (pchar*)pstr - (pchar*)prev;
		}
		else
		{
			(*results)[ matches ].pos = prev - str;	
			(*results)[ matches ].len = pstr - prev;
		}

		matches++;
	}
	
	VARS( "matches", "%d", matches );
	RETURN( matches );
}

/* -FUNCTION--------------------------------------------------------------------
	Function:		pregex_comp_replace()
	
	Author:			Jan Max Meyer
	
	Usage:			Replaces all matches of a regular expression pattern within
					a string with the replacement. Backreferences can be used
					with $x for each opening bracket within the regular 
					expression.
					
	Parameters:		pregex*			machine		The regular expression
												pattern
					uchar*			str			String the pattern
												will be ran on.
					uchar*			replacement	String that will be inserted
												as replacement for each pattern
												match. $x backreferences
												can be used
					pregex_callback	fn			An optional callback function.
												If the return value is negative,
												it ignores the current match.
												If there is an alternative
												replacement string wanted, put
												a string pointer in the member
												variable 'user' of the
												pregex_result-object provided.
												The alternative replacement
												string can contain reference
												wildcards, if not disabled.
												Use the macro REGEX_NO_CALLBACK
												to hand over an empty callback
												function.
					uchar**			result		Returns a pointer to the result
												string. This must be freed by
												the caller after its use.
																	
	Returns:		int							Returns the amount of matches.
												If the value is negative,
												it is an error define.
  
	~~~ CHANGES & NOTES ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	Date:		Author:			Note:
	17.02.2011	Jan Max Meyer	Allowed to run both NFA and DFA machines
----------------------------------------------------------------------------- */
int pregex_comp_replace( pregex* machine, uchar* str, uchar* replacement,
							pregex_callback fn, uchar** result )
{
	pregex_result*	refs		= (pregex_result*)NULL;
	pregex_result	tmp_result;
	int				refs_cnt	= 0;
	int				matches		= 0;
	int				match;
	int				charsize	= sizeof( uchar );
	int				ref;
	int				anchors;
	psize			len;
	uchar*			pstr;
	uchar*			prev;
	uchar*			rpstr;
	uchar*			rbegin;
	uchar*			rprev;
	uchar*			replace;
	uchar*			use_replacement;

	PROC( "pregex_comp_replace" );
	PARMS( "machine", "%p", machine );
	PARMS( "str", "%s", str );
	PARMS( "result", "%p", result );
	PARMS( "fn", "%p", fn );

	if( !IS_EXECUTABLE( machine->stat ) )
		RETURN( ERR_UNIMPL );

	*result = (uchar*)NULL;

	if( machine->flags & REGEX_MOD_WCHAR )
		charsize = sizeof( pchar );

	for( prev = pstr = str; *pstr; )
	{
		VARS( "pstr", "%s", pstr );

		if( machine->stat == REGEX_STAT_NFA )
			match = pregex_nfa_match( &( machine->machine.nfa ), pstr,
						&len, &anchors,
							( ( machine->flags & REGEX_MOD_NO_REFERENCES ) ?
								(pregex_result**)NULL : &refs ), &refs_cnt,
									machine->flags );
		else
			match = pregex_dfa_match( &( machine->machine.dfa ), pstr,
						&len, &anchors,
							( ( machine->flags & REGEX_MOD_NO_REFERENCES ) ?
								(pregex_result**)NULL : &refs ), &refs_cnt,
									machine->flags );

		if( match >= 0 && pregex_check_anchors( str, pstr, len,
											anchors, machine->flags ) )
		{
			use_replacement = (uchar*)NULL;

			if( fn )
			{
				MSG( "Prepare pregex_result-structure for callback" );

				memset( &tmp_result, 0, sizeof( pregex_result ) );
				tmp_result.accept = match;
				tmp_result.begin = pstr;
				tmp_result.end = pstr + len;
				tmp_result.pbegin = (pchar*)pstr;
				tmp_result.pend = (pchar*)pstr + len;
				tmp_result.len = len;

				if( machine->flags & REGEX_MOD_WCHAR )
					tmp_result.pos = (pchar*)pstr - (pchar*)str;
				else
					tmp_result.pos = pstr - str;
				
				MSG( "Calling callback-function" );
				if( (*fn)( &tmp_result ) < 0 )
				{
					MSG( "Callback function returns negative value, will"
							"discard this match" );
#ifdef UTF8
					for( ; len > 0; len-- )
						pstr += u8_seqlen( pstr );
#else
					pstr += len * charsize;
#endif
					continue;
				}
				
				/* An alternative replacement string can be saved into the
					regex_result-structure's user-pointer */					
				use_replacement = (uchar*)tmp_result.user;
			}
			
			matches++;

			if( !use_replacement )
				use_replacement = replacement;
		
			if( machine->flags & REGEX_MOD_NO_REFERENCES )
			{
				MSG( "No references wanted by caller" );
				replace = use_replacement;
			}
			else
			{
				VARS( "refs_cnt", "%d", refs_cnt );
				MSG( "Constructing replacement" );
				for( rprev = rpstr = use_replacement, replace = (uchar*)NULL;
						*rpstr; )
				{
					VARS( "*rpstr", "%c", *rpstr );
					if( *rpstr == '$' )
					{
						rbegin = rpstr;

						if( machine->flags & REGEX_MOD_WCHAR )
						{
							pchar*		end;
							pchar*		_rpstr = (pchar*)rpstr;

							MSG( "Switching to wide-character mode" );

							if( Pisdigit( *( ++_rpstr ) ) )
							{
								ref = Pstrtol( _rpstr, &end, 0 );

								VARS( "ref", "%d", ref );
								VARS( "end", "%ls", end );

								/* Skip length of the number */
								_rpstr = end;

								/* Extend first from prev of replacement */
								if( !( replace = (uchar*)Pstr_append_nchar(
										(pchar*)replace, (pchar*)rprev,
											(pchar*)rbegin - (pchar*)rprev ) ) )
									RETURN( ERR_MEM );

								VARS( "replace", "%ls", (pchar*)replace );
								
								VARS( "refs[ ref ].pbegin", "%p",
										refs[ ref ].pbegin );
								VARS( "refs[ ref ].pend", "%p",
										refs[ ref ].pend );
										
								VARS( "ref", "%d", ref );

								/* Obtain reference information */
								if( refs[ ref ].pbegin && refs[ ref ].pend )
								{
									MSG( "There is a reference!" );
									VARS( "refs[ ref ].pbegin", "%s",
											refs[ ref ].pbegin );
									VARS( "len", "%d",
										refs[ ref ].pend - refs[ ref ].pbegin );

									if( !( replace = (uchar*)Pstr_append_nchar(
											(pchar*)replace, refs[ ref ].pbegin,
												refs[ ref ].len ) ) )
										RETURN( ERR_MEM );
								}
								
								VARS( "replace", "%ls", (pchar*)replace );
								rprev = rpstr = (uchar*)_rpstr;
							}
							else
								rpstr = (uchar*)_rpstr;
						}
						else
						{
							uchar*		end;

							MSG( "Byte-character mode (Standard)" );

							if( pisdigit( *( ++rpstr ) ) )
							{
								ref = pstrtol( rpstr, &end, 0 );

								VARS( "ref", "%d", ref );
								VARS( "end", "%s", end );

								/* Skip length of the number */
								rpstr = end;

								/* Extend first from prev of replacement */
								if( !( replace = pstr_append_nchar( replace,
											rprev, rbegin - rprev ) ) )
									RETURN( ERR_MEM );

								VARS( "replace", "%s", replace );
								
								VARS( "refs[ ref ].begin", "%p",
										refs[ ref ].begin );
								VARS( "refs[ ref ].end", "%p",
										refs[ ref ].end );
										
								VARS( "ref", "%d", ref );

								/* Obtain reference information */
								if( refs[ ref ].begin && refs[ ref ].end )
								{
									MSG( "There is a reference!" );
									VARS( "refs[ ref ].begin", "%s",
											refs[ ref ].begin );
									VARS( "len", "%d",
										refs[ ref ].end - refs[ ref ].begin );

									if( !( replace = pstr_append_nchar(
										replace, refs[ ref ].begin,
											refs[ ref ].len ) ) )
										RETURN( ERR_MEM );
								}
								
								VARS( "replace", "%s", replace );						
								rprev = rpstr;
							}
						}
					}
					else
						rpstr += charsize;
				}

				VARS( "rpstr", "%p", rpstr );
				VARS( "rprev", "%p", rprev );

				if( machine->flags & REGEX_MOD_WCHAR )
				{
					if( rpstr != rprev &&
							!( replace = (uchar*)Pstr_append_str(
									(pchar*)replace, (pchar*)rprev, FALSE ) ) )
						RETURN( ERR_MEM );
				}
				else
				{
					if( rpstr != rprev && !( replace = pstr_append_str(
												replace, rprev, FALSE ) ) )
						RETURN( ERR_MEM );
				}
			}

			MSG( "Extend result string" );

			if( machine->flags & REGEX_MOD_WCHAR )
			{
				MSG( "Switching to wide-character mode" );
				VARS( "replace", "%ls", replace );		
				
				/* Extend result */
				if( !( *result = (uchar*)Pstr_append_nchar(
						(pchar*)*result, (pchar*)prev,
							(pchar*)pstr - (pchar*)prev ) ) )
					RETURN( ERR_MEM );

				if( !( *result = (uchar*)Pstr_append_str( (pchar*)*result,
						(pchar*)replace,( ( replace == use_replacement )
								? FALSE : TRUE ) ) ) )
					RETURN( ERR_MEM );
				
				VARS( "len", "%d", len );
				VARS( "pstr", "%ls", (pchar*)pstr );

				pstr += len * charsize;
				prev = pstr;
				
				VARS( "my pstr", "%ls", (pchar*)pstr );
			}
			else
			{
				MSG( "Switching to byte-character mode" );
				VARS( "replace", "%s", replace );		
				
				/* Extend result */
				if( !( *result = pstr_append_nchar(
						*result, prev, pstr - prev ) ) )
					RETURN( ERR_MEM );

				if( !( *result = pstr_append_str( *result, replace,
						( ( replace == use_replacement ) ? FALSE : TRUE ) ) ) )
					RETURN( ERR_MEM );
				
				VARS( "len", "%d", len );
				VARS( "pstr", "%s", pstr );

#ifdef UTF8
				for( ; len > 0; len-- )
					pstr += u8_seqlen( pstr );
#else
				pstr += len;
#endif
				prev = pstr;
				
				VARS( "my pstr", "%s", pstr );
			}
			
			if( !( machine->flags & REGEX_MOD_GLOBAL ) )
				break;
		}
		else
		{
			if( machine->flags & REGEX_MOD_WCHAR )
				pstr += charsize;
			else
#ifdef UTF8
				pstr += u8_seqlen( pstr );
#else
				pstr += charsize;
#endif
		}
	}
	
	if( refs_cnt )
		pfree( refs );

	if( machine->flags & REGEX_MOD_WCHAR )
	{
		VARS( "*result", "%s", *result );
		if( prev != pstr && !( *result = (uchar*)Pstr_append_str(
						(pchar*)*result, (pchar*)prev, FALSE ) ) )
			RETURN( ERR_MEM );

		VARS( "*result", "%s", *result );
	}
	else
	{
		if( prev != pstr && !( *result = pstr_append_str(
						*result, prev, FALSE ) ) )
			RETURN( ERR_MEM );
	}

	RETURN( matches );
}
