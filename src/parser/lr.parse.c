/* -MODULE----------------------------------------------------------------------
Phorward Foundation Toolkit
Copyright (C) 2006-2014 by Phorward Software Technologies, Jan Max Meyer
http://www.phorward-software.com ++ contact<at>phorward<dash>software<dot>com
All rights reserved. See LICENSE for more information.

File:	lr.parse.c
Usage:	Parse LR/LALR parser
----------------------------------------------------------------------------- */

#include "local.h"

/* Stack Element */
typedef struct
{
	pgsymbol*		symbol;	/* Symbol */

	pglrstate*		state;	/* State */
	pgtoken*		token;	/* Token */
} pglrse;

/* Parser Command Block */
typedef struct
{
	pgparser*		p;		/* Parser */
	pggrammar*		g;		/* Grammar */

	pstack*			st;		/* Stack */
	pglrse*			tos;	/* Top of stack */

	pgtoken*		la;		/* Current look ahead */

	/* State */
	pglrstate*		shift;	/* Shift to state */
	pgproduction*	reduce;	/* Reduce by production */
	pgsymbol*		lhs;	/* Default left-hand side */
}
pglrpcb;

static void print_stack( pglrpcb* pcb )
{
	int		i;
	pglrse*	e;

	for( i = 0; i < pstack_count( pcb->st ); i++ )
	{
		e = (pglrse*)pstack_access( pcb->st, i );

		fprintf( stderr, "%02d: sym: '%s' state: %d token: %s(%s)\n", i,
				e->symbol ? pg_symbol_get_name( e->symbol ) : "(X)",
				e->state ? plist_offset( plist_get_by_ptr(
								pcb->p->states, e->state ) ) : -1,
				e->token ?
					pg_symbol_get_name( pg_token_get_symbol( e->token ) )
						: "(X)",
				e->token ?
					pg_token_get_lexem( e->token )
						: "(X)" );
	}

}

/* Push state on stack */
static pboolean push( pglrpcb* pcb, pgsymbol* sym, pglrstate* st, pgtoken* tok )
{
	pglrse	e;

	e.state = st;

	if( ( e.token = tok ) )
		e.symbol = pg_token_get_symbol( tok );
	else
		e.symbol = sym;

	if( !( pcb->tos = (pglrse*)pstack_push( pcb->st, &e ) ) )
		return FALSE;

	fprintf( stderr, ">>>\n" );
	print_stack( pcb );

	return TRUE;
}

/* Pop elements off the stack */
static pboolean pop( pglrpcb* pcb, int n )
{
	int		i;

	for( i = 0; i < n; i++ )
	{
		fprintf( stderr, "<<<\n" );
		pstack_pop( pcb->st );
	}

	pcb->tos = (pglrse*)pstack_top( pcb->st );

	print_stack( pcb );

	return TRUE;
}

/* Get action on current look-ahead */
static pboolean get_action( pglrpcb* pcb, pgsymbol* sym )
{
	pglrcolumn*	col;
	plistel*	e;

	pcb->shift = (pglrstate*)NULL;
	pcb->reduce = (pgproduction*)NULL;

	plist_for( pcb->tos->state->actions, e )
	{
		col = (pglrcolumn*)plist_access( e );

		if( col->symbol == sym )
		{
			pcb->shift = col->shift;
			pcb->reduce = col->reduce;

			if( !( pcb->shift || col->reduce ) )
				return FALSE; /* Forced parse error! */

			return TRUE;
		}
	}

	/* If no entry was found, reduce by default production */
	if( ( pcb->reduce = pcb->tos->state->def_prod ) )
		return TRUE;

	return FALSE;
}

/* Get goto on current left-hand side */
static pboolean get_goto( pglrpcb* pcb )
{
	pglrcolumn*	col;
	plistel*	e;

	pcb->shift = (pglrstate*)NULL;
	pcb->reduce = (pgproduction*)NULL;

	plist_for( pcb->tos->state->gotos, e )
	{
		col = (pglrcolumn*)plist_access( e );

		if( col->symbol == pcb->lhs )
		{
			pcb->shift = col->shift;
			pcb->reduce = col->reduce;

			return TRUE;
		}
	}

	return FALSE;
}

pboolean pg_parser_lr_parse( pgparser* parser )
{
	pglrpcb		PCB;
	pglrpcb*	pcb = &PCB;
	pgtoken*	tok;

	PROC( "pg_parser_eval" );

	if( !pg_parser_is_lr( parser ) )
	{
		WRONGPARAM;
		RETURN( FALSE );
	}

	/* Initialize parser control block */
	memset( pcb, 0, sizeof( pglrpcb ) );

	pcb->p = parser;
	pcb->g = pg_parser_get_grammar( pcb->p );
	pcb->st = pstack_create( sizeof( pglrse ), 64 );

	push( pcb, (pgsymbol*)NULL,
				(pglrstate*)plist_access( plist_get( parser->states, 0 ) ),
					(pgtoken*)NULL );

	do
	{
		fprintf( stderr, "get token\n" );

		if( !pcb->la && !( pcb->la = pg_lexer_fetch( parser->lexer ) ) )
		{
			pcb->la = pg_token_create(
						pg_grammar_get_eoi( pcb->g ), (char*)NULL );
		}

		fprintf( stderr, "got token '%s' lexem '%s'\n",
			pg_symbol_get_name( pg_token_get_symbol( pcb->la ) ),
				pg_token_get_lexem( pcb->la ) );

		if( !get_action( pcb, pg_token_get_symbol( pcb->la ) ) )
		{
			/* TODO: Error recovery */
			return FALSE;
		}

		if( pcb->shift )
		{
			if( pcb->reduce )
				fprintf( stderr, "shift/reduce by production %d\n",
					pg_production_get_id( pcb->reduce ) );
			else
				fprintf( stderr, "shift to state %d\n",
					plist_offset( plist_get_by_ptr(
						parser->states, pcb->shift ) ) );

			push( pcb, (pgsymbol*)NULL,
					pcb->reduce ? (pglrstate*)NULL : pcb->shift, pcb->la );

			pcb->la = (pgtoken*)NULL;
		}

		while( pcb->reduce )
		{
			fprintf( stderr,
				"reduce by production %d\n"
				"popping %d items off the stack, replacing by '%s'\n",
				pg_production_get_id( pcb->reduce ),
				pg_production_get_rhs_length( pcb->reduce ),
				pg_symbol_get_name( pg_production_get_lhs( pcb->reduce ) ) );

			pcb->lhs = pg_production_get_lhs( pcb->reduce );
			pop( pcb, pg_production_get_rhs_length( pcb->reduce ) );

			/* Goal symbol reduced? */
			if( pcb->lhs == pg_grammar_get_goal( pcb->g )
					&& pstack_count( pcb->st ) == 1 )
				break;

			get_goto( pcb );
			push( pcb, pcb->lhs, pcb->shift, (pgtoken*)NULL );
		}
	}
	while( !pcb->reduce ); /* Break on goal */

	fprintf( stderr, "goal symbol reduced!\n" );
	return TRUE;
}
