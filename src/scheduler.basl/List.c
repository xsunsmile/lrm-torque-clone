/*
*         OpenPBS (Portable Batch System) v2.3 Software License
*
* Copyright (c) 1999-2000 Veridian Information Solutions, Inc.
* All rights reserved.
*
* ---------------------------------------------------------------------------
* For a license to use or redistribute the OpenPBS software under conditions
* other than those described below, or to purchase support for this software,
* please contact Veridian Systems, PBS Products Department ("Licensor") at:
*
*    www.OpenPBS.org  +1 650 967-4675                  sales@OpenPBS.org
*                        877 902-4PBS (US toll-free)
* ---------------------------------------------------------------------------
*
* This license covers use of the OpenPBS v2.3 software (the "Software") at
* your site or location, and, for certain users, redistribution of the
* Software to other sites and locations.  Use and redistribution of
* OpenPBS v2.3 in source and binary forms, with or without modification,
* are permitted provided that all of the following conditions are met.
* After December 31, 2001, only conditions 3-6 must be met:
*
* 1. Commercial and/or non-commercial use of the Software is permitted
*    provided a current software registration is on file at www.OpenPBS.org.
*    If use of this software contributes to a publication, product, or
*    service, proper attribution must be given; see www.OpenPBS.org/credit.html
*
* 2. Redistribution in any form is only permitted for non-commercial,
*    non-profit purposes.  There can be no charge for the Software or any
*    software incorporating the Software.  Further, there can be no
*    expectation of revenue generated as a consequence of redistributing
*    the Software.
*
* 3. Any Redistribution of source code must retain the above copyright notice
*    and the acknowledgment contained in paragraph 6, this list of conditions
*    and the disclaimer contained in paragraph 7.
*
* 4. Any Redistribution in binary form must reproduce the above copyright
*    notice and the acknowledgment contained in paragraph 6, this list of
*    conditions and the disclaimer contained in paragraph 7 in the
*    documentation and/or other materials provided with the distribution.
*
* 5. Redistributions in any form must be accompanied by information on how to
*    obtain complete source code for the OpenPBS software and any
*    modifications and/or additions to the OpenPBS software.  The source code
*    must either be included in the distribution or be available for no more
*    than the cost of distribution plus a nominal fee, and all modifications
*    and additions to the Software must be freely redistributable by any party
*    (including Licensor) without restriction.
*
* 6. All advertising materials mentioning features or use of the Software must
*    display the following acknowledgment:
*
*     "This product includes software developed by NASA Ames Research Center,
*     Lawrence Livermore National Laboratory, and Veridian Information
*     Solutions, Inc.
*     Visit www.OpenPBS.org for OpenPBS software support,
*     products, and information."
*
* 7. DISCLAIMER OF WARRANTY
*
* THIS SOFTWARE IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND. ANY EXPRESS
* OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
* OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE, AND NON-INFRINGEMENT
* ARE EXPRESSLY DISCLAIMED.
*
* IN NO EVENT SHALL VERIDIAN CORPORATION, ITS AFFILIATED COMPANIES, OR THE
* U.S. GOVERNMENT OR ANY OF ITS AGENCIES BE LIABLE FOR ANY DIRECT OR INDIRECT,
* INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
* LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA,
* OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
* LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
* NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
* EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*
* This license will be governed by the laws of the Commonwealth of Virginia,
* without reference to its choice of law rules.
*/

#include <pbs_config.h>   /* the master config generated by configure */

#ifndef _POSIX_SOURCE
#define _POSIX_SOURCE 1
#endif  /* _POSIX_SOURCE */

#include <stdio.h>
#include <ctype.h>
#include <malloc.h>
#include <string.h>
#include <setjmp.h>
#include <stdlib.h>

#include "List.h"

static char ident[] = "@(#) $RCSfile$ $Revision: 2367 $";
/* Global variables */
FILE *ListFpOut;

/* File Scope Variables */
static char * ListErrors[] =
  {
  "0 no such error msg",
  "1 illegal operation on empty List",
  "2 nxp = NULL",
  "3 lexem ptr = NULL",
  "4 NodeNew error got returned NULL",
  "5 node not in List",
  "6 lexem not in List",
  "7 fname *main* not in List -- it should be",
  ""
  };

static int ListMaxErrors = 9;

static char *ListName = "List";

static int ListDF = 0;


void
ListPutDF(int df)
  {
  ListCondPrint("ListPutDF");

  ListDF = df;
  }


void
ListCondPrint(char *str)
  {
  if (ListFpOut == NULL)
    ListErr(7);

  if (ListDF == 1)
    {
    fprintf(ListFpOut, "%s\t", ListName);
    fprintf(ListFpOut, "%s\n", str);
    }
  }

int ListIsEmpty(L)
List L;
  {
  ListCondPrint("ListIsEmpty");

  return(L == NULL);
  }

void ListPrint(L)
List L;
  {
  if (ListFpOut == NULL)
    ListErr(7);

  fprintf(ListFpOut, "ListPrint\n");

  if (L == NULL)
    {
    fprintf(ListFpOut, "\nend of list\n");
    return;
    }
  else
    {
    NodePrint(L);
    ListPrint(L->rptr);
    }
  }


List ListInsertFront(L, nxp)
List  L;
Np  nxp;
  {
  ListCondPrint("ListInsertFront");

  /*L can be NULL to insert to empty list*/

  if (nxp == NULL) ListErr(2);

  nxp->rptr = L;

  return(nxp);
  }


void ListParamLink(funNp, parNp)
Np  funNp;
Np  parNp;
  {
  Np paramPt;
  ListCondPrint("ListParamLink");

  /*L cannot be NULL */

  if (funNp == NULL) ListErr(2);

  if (parNp == NULL) ListErr(2);

  /*insert the param immed after FunNp-- insert end so that parameters */
  /* are listed in order */
  /*
   parNp->funDescr.paramPtr = funNp->funDescr.paramPtr;
   funNp->funDescr.paramPtr = parNp;
  */
  paramPt = funNp->funDescr.paramPtr;

  if (paramPt == NULL)
    funNp->funDescr.paramPtr = parNp;
  else
    {
    while (paramPt->funDescr.paramPtr != NULL)
      {
      paramPt = paramPt->funDescr.paramPtr;
      }

    paramPt->funDescr.paramPtr = parNp;
    }

  parNp->funDescr.paramPtr = NULL;



  }

List ListInsertSortedN(L, nxp)
List  L;
Np  nxp;
  {
  ListCondPrint("ListInsertSortedN");

  /*L can be NULL to insert to empty list*/

  if (nxp == NULL) ListErr(2);

  if (L == NULL)
    {
    L = nxp;
    return(L);
    }
  else
    {
    /*increasing order*/
    if (strcmp(nxp->lexeme, L->lexeme) < 0)
      {
      /*insert here*/
      nxp->rptr = L;
      L = nxp;
      return(L);
      }
    else
      {
      L->rptr = ListInsertSortedN(L->rptr, nxp);
      return(L);

      }
    }
  }


List ListInsertSortedD(L, lexem, typ, lineDe, leve, funFla)
List  L;
char  *lexem;
int  typ;
int  lineDe;
int  leve;
int  funFla;
  {
  Np nxp;

  ListCondPrint("ListInsertSortedD");

  if (lexem == NULL)
    ListErr(3);

  /*L can be NULL to insert to empty list*/

  nxp = NodeNew(lexem, typ, lineDe, leve, funFla);

  if (nxp == NULL)
    ListErr(4);

  return(ListInsertSortedN(L, nxp));
  }

int ListIsMember(L, nxp)
List  L;
Np  nxp;
  {
  int s;

  ListCondPrint("ListIsMember");

  if (nxp == NULL)
    ListErr(2);

  if (L == NULL)
    {
    return(0);
    }
  else
    {
    if (nxp == L)
      {
      return(1);
      }
    else
      {
      s = ListIsMember(L->rptr, nxp);
      return(s);
      }
    }
  }


Np ListGetLast(L)
List L;
  {
  Np last;

  ListCondPrint("ListGetLast");

  if (L == NULL)
    ListErr(1);

  if (L->rptr == NULL)
    {
    return(L);
    }
  else
    {
    last = ListGetLast(L->rptr);
    return(last);
    }
  }


Np ListGetSucc(L, nxp)
List  L;
Np  nxp;
  {
  ListCondPrint("ListGetSucc");

  if (L == NULL)
    ListErr(1);

  if (nxp == NULL)
    ListErr(2);

  if (ListIsMember(L, nxp) == 1)
    return(nxp->rptr);
  else
    ListErr(5);

  /*never get here -- keep compiler happy*/
  return(NULL);

  }


List ListDeleteNode(L, nxp)
List  L;
Np  nxp;
  {
  ListCondPrint("ListDeleteNode");

  if (L == NULL)
    ListErr(1);

  if (nxp == NULL)
    ListErr(2);

  if (ListIsMember(L, nxp) == 0)
    ListErr(5); /*nxp not in list*/

  if (nxp == L) /*delete head of sublist*/
    {
    /*delete here*/
    L = L->rptr;
    free(nxp);
    return(L); /*returning adjsted rptr of pred*/
    }
  else
    {
    L->rptr = ListDeleteNode(L->rptr, nxp);
    return(L);
    }
  }


List ListDelete(L)
List L;
  {
  List t;

  ListCondPrint("ListDelete");

  if (L == NULL)
    {
    return(NULL);
    }
  else
    {
    t = L->rptr;
    free(L);
    return(ListDelete(t));
    }
  }


List ListDeleteLevel(L, leve)
List  L;
int  leve;
  {
  List t;

  ListCondPrint("ListDeleteLevel");
  /*level is block -- cannot be discontinuos in SymTab*/

  if (L == NULL)
    return(NULL);

  if (L->level == leve)
    {
    t = L->rptr;
    free(L);
    return(ListDeleteLevel(t, leve));
    }
  else
    {
    /*got out of the block*/
    return(L);
    }
  }


Np ListFindNodeByLexeme(L, lexem)
List  L;
char  *lexem;
  {
  Np foundp;

  ListCondPrint("ListFindNodeByLexeme");

  if (L == NULL)
    return(NULL);

  if (lexem == NULL)
    ListErr(3);

  if (strcmp(L->lexeme, lexem) == 0)
    {
    return(L);
    }
  else
    {
    foundp = ListFindNodeByLexeme(L->rptr, lexem);
    }

  return(foundp); /*NULL means not found*/
  }


Np ListFindFunProtoByLexemeInProg(L, lexem)
List  L;
char  *lexem;
  {
  Np foundp;

  ListCondPrint("ListFindFunProtoByLexemeInProg");

  if (L == NULL)
    return(NULL);

  if (lexem == NULL)
    ListErr(3);

  while (L != NULL)
    {
    if ((strcmp(L->lexeme, "main") == 0))
      break;

    L = L->rptr;
    }

  if (L == NULL)
    ListErr(7);

  foundp = ListFindNodeByLexeme(L, lexem);

  return(foundp);

  }


Np ListFindNodeByLexemeInLevel(L, lexem, leve)
List  L;
char  *lexem;
int  leve;
  {
  Np foundp;

  ListCondPrint("ListFindNodeByLexemeInLevel");

  if (L == NULL)
    return(NULL);

  if (lexem == NULL)
    ListErr(3);

  /* This is no longer true since we can have */
  /* global variable declarations appearing */
  /* before functions that can contain local */
  /* variables in a different level */
  /*
   if (NodeGetLevel(L) != leve)
    return(NULL);
  */
  /*block cannot discontinuous*/

  if (strcmp(L->lexeme, lexem) == 0 && \
      NodeGetLevel(L) == leve)
    {
    return(L);
    }
  else
    {
    foundp = ListFindNodeByLexemeInLevel(L->rptr, lexem, leve);
    return(foundp); /*NULL means not found*/
    }
  }

Np ListFindNodeByLexemeInLine(L, lexem, line)
List  L;
char  *lexem;
int  line;
  {
  Np foundp;

  ListCondPrint("ListFindNodeByLexemeInLine");

  if (L == NULL)
    return(NULL);

  if (lexem == NULL)
    ListErr(3);

  if (strcmp(NodeGetLexeme(L), lexem) == 0 && NodeGetLineDef(L) == line)
    {
    return(L);
    }
  else
    {
    foundp = ListFindNodeByLexemeInLine(L->rptr, lexem, line);
    return(foundp); /*NULL means not found*/
    }
  }

/* Same as ListFindNodeByLexemeInLine except that it does "strpbrk" instead of
   doing "strcmp". */
Np ListMatchNodeByLexemeInLine(L, lexem, line)
List  L;
char  *lexem;
int  line;
  {
  Np foundp;

  ListCondPrint("ListMatchNodeByLexemeInLine");

  if (L == NULL)
    return(NULL);

  if (lexem == NULL)
    ListErr(3);

  if (strpbrk(NodeGetLexeme(L), lexem) != NULL && \
      NodeGetLineDef(L) == line)
    {
    return(L);
    }
  else
    {
    foundp = ListMatchNodeByLexemeInLine(L->rptr, lexem, line);
    return(foundp); /*NULL means not found*/
    }
  }

/* Gets the node that is before the node containing "lexem" inserted at */
/* "line". */
/* NOTE: If the returned np contains the "lexem" and "line", then it */
/*       means it has no before node (i.e. the head of the list. */
Np ListFindNodeBeforeLexemeInLine(L, lexem, line)
List  L;
char  *lexem;
int  line;
  {
  Np foundp;

  ListCondPrint("ListFindNodeBeforeLexemeInLine");

  if (L == NULL)
    return(NULL);

  if (lexem == NULL)
    ListErr(3);

  /* If current node itself matches the lexem and line */
  if (strcmp(NodeGetLexeme(L), lexem) == 0 && NodeGetLineDef(L) == line)
    {
    return(L);
    }

  if (L->rptr == NULL)
    return(NULL);

  if (strcmp(NodeGetLexeme(L->rptr), lexem) == 0 && NodeGetLineDef(L->rptr) == line)
    {
    return(L);
    }
  else
    {
    foundp = ListFindNodeBeforeLexemeInLine(L->rptr, lexem, line);
    return(foundp); /*NULL means not found*/
    }
  }

/* Similar to ListFindNodeBeforeLexemeInLine except instead of doing a "strcmp",
   a "strpbrk" is done. */
Np ListMatchNodeBeforeLexemeInLine(L, lexem, line)
List  L;
char  *lexem;
int  line;
  {
  Np foundp;

  ListCondPrint("ListFindNodeBeforeLexemeInLine");

  if (L == NULL)
    return(NULL);

  if (lexem == NULL)
    ListErr(3);

  /* If current node itself matches the lexem and line */
  if (ListDF == 1)
    printf("Matching %s with %s and linedef: %d with %d\n",
           NodeGetLexeme(L), lexem, NodeGetLineDef(L), line);

  if (strpbrk(NodeGetLexeme(L), lexem) != NULL && NodeGetLineDef(L) == line)
    {
    return(L);
    }

  if (L->rptr == NULL)
    return(NULL);

  if (ListDF == 1)
    printf("Matching %s with %s and linedef: %d with %d\n",
           NodeGetLexeme(L), lexem, NodeGetLineDef(L), line);

  if (strpbrk(NodeGetLexeme(L->rptr), lexem) != NULL && NodeGetLineDef(L->rptr) == line)
    {
    return(L);
    }
  else
    {
    foundp = ListMatchNodeBeforeLexemeInLine(L->rptr, lexem, line);
    return(foundp); /*NULL means not found*/
    }
  }


/* Similar to ListFindNodeByLexemeInLevel except instead of doing a "strcmp" on */
/* the lexemes, the supplied "compare_func" is used instead. */
Np ListFindNodeByLexemeAndTypeInLevel(L, lexem, leve, type, compare_func)
List  L;
char  *lexem;
int  leve;
int type;
int (*compare_func)();
  {
  Np foundp;

  ListCondPrint("ListFindNodeByLexemeInLevel2");

  if (L == NULL)
    return(NULL);

  if (lexem == NULL)
    ListErr(3);

  /* No longer true */
  /*
   if (NodeGetLevel(L) != leve)
    return(NULL);
  */
  /*block cannot discontinuous*/

  if ((NodeGetType(L) == type) && (compare_func(L->lexeme, lexem) == 0) && \
      NodeGetLevel(L) == leve)
    {
    return(L);
    }
  else
    {
    foundp = ListFindNodeByLexemeAndTypeInLevel(L->rptr, lexem, leve, type, compare_func);
    return(foundp); /*NULL means not found*/
    }
  }

Np ListFindAnyNodeInLevelOfType(L, leve, type)
List  L;
int  leve;
int type;
  {
  Np foundp;

  ListCondPrint("ListFindAnyNodeInLevelOfType");

  if (L == NULL)
    return(NULL);

  /* No longer true */
  /*
   if (NodeGetLevel(L) != leve)
    return(NULL);
  */
  /*block cannot discontinuous*/

  if (NodeGetType(L) == type && \
      NodeGetLevel(L) == leve)
    {
    return(L);
    }
  else
    {
    foundp = ListFindAnyNodeInLevelOfType(L->rptr, leve, type);
    return(foundp); /*NULL means not found*/
    }
  }


void
ListErr(int e)
  {
  if (ListFpOut == NULL)
    ListErr(7);

  fprintf(ListFpOut, "ListErr\n");

  if (e >= ListMaxErrors)
    e = 0;

  fprintf(ListFpOut, "rs: %s\n", ListErrors[e]);

  exit(1);
  }
