/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/* $Id: polys-impl.cc,v 1.26 1999-09-30 14:09:39 obachman Exp $ */

/***************************************************************
 *
 * File:       polys-impl.cc
 * Purpose:    low-level procuders for polys.
 *
 * If you touch anything here, you better know what you are doing.
 * What is here should not be used directly from other routines -- the
 * encapsulations in polys.[h,cc] should be used, instead.
 *
 ***************************************************************/
#ifndef POLYS_IMPL_CC
#define POLYS_IMPL_CC

#include <stdio.h>
#include <string.h>
#include "mod2.h"
#include "tok.h"
#include "structs.h"
#include "mmprivate.h"
#include "mmemory.h"
#include "febase.h"
#include "numbers.h"
#include "polys.h"
#include "ring.h"
#include "polys-impl.h"

inline void pRingCopy2ExpV(poly dest, poly src, ring src_r)
{
  for (int i=pVariables; i; i--)
    pSetExp(dest, i, pRingGetExp(src_r, src, i));
  pSetComp(dest, pRingGetComp(src_r, src));
}

// Returns a converted copy (in the sense that the returned poly is
// poly of currRing) of poly p which is from ring r -- assumes that
// currRing and r have the same number of variables, i.e. that polys
// from r can be "fetched" into currRing
#ifdef MDEBUG
poly pDBFetchCopy(ring r, poly p,char *f,int l)
#else
poly _pFetchCopy(ring r, poly p)
#endif
{
  poly res, a, w;
  int ii;

  if (p==NULL) return NULL;
  if ((r->N == pVariables)
      && (0==memcmp(r->VarOffset,currRing->VarOffset,
                    (pVariables+1)*sizeof(currRing->VarOffset[0])))
      && r->ExpLSize == currRing->ExpLSize)
  {
    poly w;
#ifdef MDEBUG
    res = (poly) mmDBAllocHeap(mm_specHeap, f,l);
#else
    mmAllocHeap(res, mm_specHeap);
#endif
    pSetCoeff0(res, nCopy(p->coef));
    memcpyW(&(res->exp.l[0]), &(p->exp.l[0]), currRing->ExpLSize);
    a = res;
    pSetm(res);
    pIter(p);
    while (p != NULL)
    {
#ifdef MDEBUG
      w = (poly) mmDBAllocHeap(mm_specHeap, f,l);
#else
      mmAllocHeap(w , mm_specHeap);
#endif
      pSetCoeff0(w, nCopy(p->coef));
      memcpyW(&(w->exp.l[0]), &(p->exp.l[0]), currRing->ExpLSize);
      pSetm(w);
      pNext(a) = w;
      a = w;
      pIter(p);
    }
    pNext(a) = NULL;
  }
  else
  {
#ifdef MDEBUG
    a = res = pDBInit(mm_specHeap, f,l);
#else
    a = res = pInit();
#endif
    res->coef = nCopy(p->coef);
    pRingCopy2ExpV(res, p, r);
    pSetm(res);
    pIter(p);
    while (p != NULL)
    {
      // the VarOffset's are different: Hence we
      // convert betweeen a lex order and a revlex order -- to speed
      // up the sorting, we assemble new poly in inverse order
#ifdef MDEBUG
      w = pDBInit(mm_specHeap, f,l);
#else
      w = pInit();
#endif
      w->coef = nCopy(p->coef);
      pRingCopy2ExpV(w, p, r);
      pSetm(w);
      pNext(a) = w;
      a = w;
      pIter(p);
    }
  }
#ifdef PDEBUG
  res = pOrdPolyMerge(res);
  pTest(res);
  return res;
#else
  return pOrdPolyMerge(res);
#endif
}

#ifdef MDEBUG
poly pDBFetchCopyDelete(ring r, poly p,char *f,int l)
#else
poly _pFetchCopyDelete(ring r, poly p)
#endif
{
  poly res, tmp, a, w;
  int ii;

  if (p==NULL) return NULL;

  if ((r->N == pVariables)
      && (0==memcmp(r->VarOffset,currRing->VarOffset,
                    (pVariables+1)*sizeof(currRing->VarOffset[0])))
      && r->ExpLSize == currRing->ExpLSize)
  {
#ifdef MDEBUG
    res = (poly) mmDBAllocHeap(mm_specHeap, f,l);
#else
    mmAllocHeap(res, mm_specHeap);
#endif
    pCopy2(res, p);
    a = res;
    pSetm(res);
    tmp = p;
    pIter(p);
#ifdef MDEBUG
    mmDBFreeHeap(tmp, r->mm_specHeap, f, l);
#else
    mmFreeHeap(tmp, r->mm_specHeap);
#endif

    while (p != NULL)
    {
      tmp = p;

#ifdef MDEBUG
      w = (poly) mmDBAllocHeap(mm_specHeap, f,l);
#else
      mmAllocHeap(w, mm_specHeap);
#endif
      pCopy2(w, p);
      pSetm(w);
      pNext(a) = w;
      a = w;
      pIter(p);
#ifdef MDEBUG
      mmDBFreeHeap(tmp, r->mm_specHeap, f, l);
#else
      mmFreeHeap(tmp, r->mm_specHeap);
#endif

    }
    pNext(a) = NULL;
  }
  else
  {
#ifdef MDEBUG
    a = res = pDBInit(mm_specHeap, f,l);
#else
    a = res = pInit();
#endif
    res->coef = p->coef;
    pRingCopy2ExpV(res, p, r);
    pSetm(res);
    tmp = p;
    pIter(p);
#ifdef MDEBUG
    mmDBFreeHeap(tmp, r->mm_specHeap, f, l);
#else
    mmFreeHeap(tmp, r->mm_specHeap);
#endif
    while (p != NULL)
    {
      // the VarOffset's are different: Hence we
      // convert betweeen a lex order and a revlex order -- to speed
      // up the sorting, we assemble new poly in inverse order
#ifdef MDEBUG
      w = pDBInit(mm_specHeap, f,l);
#else
      w = pInit();
#endif
      w->coef = p->coef;
      pRingCopy2ExpV(w, p, r);
      pSetm(w);
      pNext(a) = w;
      a = w;
      tmp = p;
      pIter(p);
#ifdef MDEBUG
      mmDBFreeHeap(tmp, r->mm_specHeap, f, l);
#else
      mmFreeHeap(tmp, r->mm_specHeap);
#endif
    }
  }
#ifdef PDEBUG
  res = pOrdPolyMerge(res);
  pTest(res);
  return res;
#else
  return pOrdPolyMerge(res);
#endif
}

/***************************************************************
 *
 * Storage Managament Routines
 *
 ***************************************************************/


/*2
* create a new monomial and init
*/
#ifdef MDEBUG
poly pDBInit(memHeap heap, char * f, int l)
{
  poly p=pDBNew(heap, f,l);
  memset(p,0, pMonomSize);
  nNew(&(p->coef));
  return p;
}
#endif

/*2
* delete a poly, resets pointer
* put the monomials in the freelist
*/
#ifdef MDEBUG
void pDBDelete(poly * p, memHeap heap, char * f, int l)
{
  poly h = *p;

  while (h!=NULL)
  {
#ifdef LDEBUG
    nDBDelete(&(h->coef),f,l);
#else
    nDelete(&(h->coef));
#endif
    pIter(h);
    pDBFree1((ADDRESS)*p, heap, f,l);
    *p=h;
    if (l>0) l= -l;
  }
  *p = NULL;
}
#else
void _pDelete(poly* p, memHeap heap)
{
  poly h = *p;
  poly pp;

  while (h!=NULL)
  {
    nDelete(&(h->coef));
    pp=h;
    pIter(h);
    _pFree1((ADDRESS)pp, heap);
  }
  *p = NULL;
}
#endif

/*2
* remove first monom
*/
#ifdef MDEBUG
void pDBDelete1(poly * p, memHeap heap, char * f, int l)
{
  poly h = *p;

  if (h==NULL) return;
  nDelete(&(h->coef));
  *p = pNext(h);
  pDBFree1((ADDRESS)h, heap, f,l);
}
#else
void _pDelete1(poly* p, memHeap heap)
{
  poly h = *p;

  if (h==NULL) return;
  nDelete(&(h->coef));
  *p = pNext(h);
  _pFree1((ADDRESS)h, heap);
}
#endif


void ppDelete(poly* p, ring rg)
{
  ring origRing = currRing;
  rChangeCurrRing(rg, FALSE);
  pDelete(p);
  rChangeCurrRing(origRing, FALSE);
}

/*2
* creates a copy of p
*/
#ifdef MDEBUG
poly pDBCopy(memHeap d_h, poly s_p, char *f,int l)
#else
poly _pCopy(memHeap d_h, poly s_p)
#endif
{
  spolyrec dp;
  poly d_p = &dp;

  assume(d_h != NULL && (d_h == mm_specHeap) ||
         mmGetHeapBlockSize(d_h) == mmGetHeapBlockSize(mm_specHeap));

  while (s_p != NULL)
  {
#ifdef MDEBUG
    d_p->next = (poly) mmDBAllocHeap(d_h, f, l);
#else
    mmAllocHeap(d_p->next, d_h);
#endif
    d_p = d_p->next;
    pSetCoeff0(d_p, nCopy(pGetCoeff(s_p)));
    memcpyW(&(d_p->exp.l[0]), &(s_p->exp.l[0]), currRing->ExpLSize);
    pIter(s_p);
  }
  pNext(d_p) = NULL;
  pHeapTest(dp.next, d_h);
  return dp.next;
}

#ifdef MDEBUG
poly pDBCopy(poly s_p, char *f,int l)
{
  return pDBCopy(mm_specHeap, s_p, f, l);
}
#else
poly _pCopy(poly s_p)
{
  return _pCopy(mm_specHeap, s_p);
}
#endif


#ifdef MDEBUG
poly pDBShallowCopyDelete(memHeap d_h,poly *p,memHeap s_h, char *f,int l)
#else
poly _pShallowCopyDelete(memHeap d_h, poly *p, memHeap s_h)
#endif
{
  spolyrec dp;
  poly d_p = &dp, tmp;
  poly s_p = *p;

  assume(d_h != NULL && s_h != NULL &&
         mmGetHeapBlockSize(d_h) == mmGetHeapBlockSize(s_h));

  if (currRing->ExpLSize <= 2)
  {
    if (currRing->ExpLSize == 1)
    {
      while (s_p != NULL)
      {
#ifdef MDEBUG
        d_p->next = (poly) mmDBAllocHeap(d_h, f, l);
#else
        mmAllocHeap(d_p->next, d_h);
#endif
        d_p = d_p->next;

        d_p->coef = s_p->coef;
        d_p->exp.l[0] = s_p->exp.l[0];

        tmp = pNext(s_p);
#ifdef MDEBUG
        mmDBFreeHeap(s_p, s_h, f, l);
#else
        mmFreeHeap(s_p, s_h);
#endif
        s_p = tmp;
      }
    }
    else
    {
      while (s_p != NULL)
      {
#ifdef MDEBUG
        d_p->next = (poly) mmDBAllocHeap(d_h, f, l);
#else
        mmAllocHeap(d_p->next, d_h);
#endif
        d_p = d_p->next;

        d_p->coef = s_p->coef;
        d_p->exp.l[0] = s_p->exp.l[0];
        d_p->exp.l[1] = s_p->exp.l[1];

        tmp = pNext(s_p);
#ifdef MDEBUG
        mmDBFreeHeap(s_p, s_h, f, l);
#else
        mmFreeHeap(s_p, s_h);
#endif
        s_p = tmp;
      }
    }
  }
  else
  {
    if (currRing->ExpLSize & 1)
    {
      while (s_p != NULL)
      {

#ifdef MDEBUG
        d_p->next = (poly) mmDBAllocHeap(d_h, f, l);
#else
        mmAllocHeap(d_p->next, d_h);
#endif
        d_p = d_p->next;

        d_p->coef = s_p->coef;
        memcpy_nwODD(&(d_p->exp.l[0]), &(s_p->exp.l[1]), currRing->ExpLSize);

        tmp = pNext(s_p);
#ifdef MDEBUG
        mmDBFreeHeap(s_p, s_h, f, l);
#else
        mmFreeHeap(s_p, s_h);
#endif
        s_p = tmp;
      }
    }
    else
    {
      while (s_p != NULL)
      {

#ifdef MDEBUG
        d_p->next = (poly) mmDBAllocHeap(d_h, f, l);
#else
        mmAllocHeap(d_p->next, d_h);
#endif
        d_p = d_p->next;

        d_p->coef = s_p->coef;
        memcpy_nwEVEN(&(d_p->exp.l[0]), &(s_p->exp.l[1]), currRing->ExpLSize);

        tmp = pNext(s_p);
#ifdef MDEBUG
        mmDBFreeHeap(s_p, s_h, f, l);
#else
        mmFreeHeap(s_p, s_h);
#endif
        s_p = tmp;
      }
    }
  }
  pNext(d_p) = NULL;
  pHeapTest(dp.next, d_h);
  *p = NULL;
  return pNext(dp.next);
}


/*2
* creates a copy of the initial monomial of p
* sets the coeff of the copy to a defined value
*/
#ifdef MDEBUG
poly pDBCopy1(poly p,char *f,int l)
#else
poly _pCopy1(poly p)
#endif
{
  poly w;
#ifdef MDEBUG
  w = pDBNew(mm_specHeap, f,l);
#else
  w = pNew();
#endif
  pCopy2(w,p);
  nNew(&(w->coef));
  pNext(w) = NULL;
  return w;
}

/*2
* returns (a copy of) the head term of a
*/
#ifdef MDEBUG
poly pDBHead(memHeap heap, poly p,char *f, int l)
#else
poly _pHead(memHeap heap, poly p)
#endif
{
  poly w=NULL;

  if (p!=NULL)
  {
    assume(heap != NULL && (heap == mm_specHeap) ||
           mmGetHeapBlockSize(heap) == mmGetHeapBlockSize(mm_specHeap));

#ifdef MDEBUG
    w = (poly) mmDBAllocHeap(heap, f, l);
#else
    mmAllocHeap(w, heap);
#endif
    memcpyW(&(w->exp.l[0]), &(p->exp.l[0]), currRing->ExpLSize);
    pSetCoeff0(w,nCopy(pGetCoeff(p)));
    pNext(w) = NULL;
  }
  return w;
}

#ifdef MDEBUG
poly pDBShallowCopyDeleteHead(memHeap d_h,poly *s_p,memHeap s_h, char *f,int l)
#else
poly _pShallowCopyDeleteHead(memHeap d_h, poly *s_p, memHeap s_h)
#endif
{
  poly w = NULL;
  poly p = *s_p;

  if (p!=NULL)
  {
    assume(d_h != NULL && s_h != NULL &&
           mmGetHeapBlockSize(d_h) == mmGetHeapBlockSize(s_h));

#ifdef MDEBUG
    w = (poly) mmDBAllocHeap(d_h, f, l);
#else
    mmAllocHeap(w, d_h);
#endif
    memcpyW(&(w->exp.l[0]), &(p->exp.l[0]), currRing->ExpLSize);
    pSetCoeff0(w,pGetCoeff(p));
    pNext(w) = NULL;

    *s_p = pNext(p);
#ifdef MDEBUG
    mmDBFreeHeap(p, s_h, f, l);
#else
    mmFreeHeap(p, s_h);
#endif
  }
  return w;
}


/*2
* returns a fetched copy of the head term of a
*/
#ifdef MDEBUG
poly pDBFetchHead(ring r, poly p,char *f, int l)
#else
poly _pFetchHead(ring r, poly p)
#endif
{
  poly w=NULL;

  if (p!=NULL)
  {
#ifdef MDEBUG
    w = pDBInit(mm_specHeap, f,l);
#else
    w = pInit();
#endif
    pRingCopy2ExpV(w, p, r);
    pSetm(w);
    pSetCoeff0(w,nCopy(pGetCoeff(p)));
    pNext(w) = NULL;
  }
  return w;
}

/*2
* returns a fetched copy of the head term of a and deletes a
*/
#ifdef MDEBUG
poly pDBFetchHeadDelete(ring r, poly p, char *f, int l)
#else
poly _pFetchHeadDelete(ring r, poly p)
#endif
{
  poly w=NULL;

  if (p!=NULL)
  {
#ifdef MDEBUG
    w = pDBInit(mm_specHeap, f,l);
#else
    w = pInit();
#endif
    pRingCopy2ExpV(w, p, r);
    pSetm(w);
    pSetCoeff0(w, pGetCoeff(p));
    pNext(w) = NULL;
#ifdef MDEBUG
    pDBFree1(p, r->mm_specHeap, f, l);
#else
    _pFree1(p, r->mm_specHeap);
#endif
  }
  return w;
}

poly pHeadProc(poly p)
{
  return pHead(p);
}

/*2
* returns (a copy of) the head term of a without the coef
*/
#ifdef MDEBUG
poly pDBHead0(poly p,char *f, int l)
#else
poly _pHead0(poly p)
#endif
{
  poly w=NULL;

  if (p!=NULL)
  {
#if defined(PDEBUG) && defined(MDEBUG)
    w = pDBNew(mm_specHeap, f,l);
#else
    w = pNew();
#endif
    pCopy2(w,p);
    pSetCoeff0(w,NULL);
    pNext(w) = NULL;
  }
  return w;
}


/***************************************************************
 *
 * Routines for turned on debugging
 *
 ***************************************************************/

#if defined(PDEBUG) && PDEBUG > 1
Exponent_t pPDSetExp(poly p, int v, Exponent_t e, char* f, int l)
{
  if (v == 0)
  {
    Warn("zero index to exponent in %s:%d\n", f, l);
  }
  if (v > pVariables)
  {
    Warn("index %d to exponent too large in %s:%d\n", v, f, l);
  }
  return (p)->exp.e[_pExpIndex(v)]=(e);
}

Exponent_t pPDGetExp(poly p, int v, char* f, int l)
{
  if (v == 0)
  {
    Warn("zero index to exponent in %s:%d\n", f, l);
  }
  if (v > pVariables)
  {
    Warn("index %d to exponent too large in %s:%d\n", v, f, l);
  }
  return (p)->exp.e[_pExpIndex(v)];
}

Exponent_t pPDRingSetExp(ring r, poly p, int v, Exponent_t e, char* f, int l)
{
  if (v == 0)
  {
    Warn("zero index to exponent in %s:%d\n", f, l);
  }
  if (v > r->N)
  {
    Warn("index %d to exponent too large in %s:%d\n", v, f, l);
  }
  return (p)->exp.e[_pRingExpIndex(r, v)]=(e);
}

Exponent_t pPDRingGetExp(ring r, poly p, int v, char* f, int l)
{
  if (v == 0)
  {
    Warn("zero index to exponent in %s:%d\n", f, l);
  }
  if (v > r->N)
  {
    Warn("index %d to exponent too large in %s:%d\n", v, f, l);
  }
  return (p)->exp.e[_pRingExpIndex(r,v)];
}

Exponent_t pPDIncrExp(poly p, int v, char* f, int l)
{
  if (v == 0)
  {
    Warn("zero index to exponent in %s:%d\n", f, l);
  }
  if (v > pVariables)
  {
    Warn("index %d to exponent too large in %s:%d\n", v, f, l);
  }
  return ((p)->exp.e[_pExpIndex(v)])++;
}

Exponent_t pPDDecrExp(poly p, int v, char* f, int l)
{
  if (v == 0)
  {
    Warn("zero index to exponent in %s:%d\n", f, l);
  }
  if (v > pVariables)
  {
    Warn("index %d to exponent too large in %s:%d\n", v, f, l);
  }
  return ((p)->exp.e[_pExpIndex(v)])--;
}

Exponent_t pPDAddExp(poly p, int v, Exponent_t e, char* f, int l)
{
  if (v == 0)
  {
    Warn("zero index to exponent in %s:%d\n", f, l);
  }
  if (v > pVariables)
  {
    Warn("index %d to exponent too large in %s:%d\n", v, f, l);
  }
  return ((p)->exp.e[_pExpIndex(v)]) += (e);
}

Exponent_t pPDSubExp(poly p, int v, Exponent_t e, char* f, int l)
{
  if (v == 0)
  {
    Warn("zero index to exponent in %s:%d\n", f, l);
  }
  if (v > pVariables)
  {
    Warn("index %d to exponent too large in %s:%d\n", v, f, l);
  }
  return ((p)->exp.e[_pExpIndex(v)]) -= (e);
}

Exponent_t pPDMultExp(poly p, int v, Exponent_t e, char* f, int l)
{
  if (v == 0)
  {
    Warn("zero index to exponent in %s:%d\n", f, l);
  }
  if (v > pVariables)
  {
    Warn("index %d to exponent too large in %s:%d\n", v, f, l);
  }
  return ((p)->exp.e[_pExpIndex(v)]) *= (e);
}

// routines on components
Exponent_t pDBSetComp(poly p, Exponent_t k, int le, char* f, int l)
{
  if (k < 0)
  {
    Warn("set negative component %d in %s:%d", k, f, l);
  }
  if (currRing->order[1] == ringorder_S)
  {
    if (le <= 0) le = currRing->typ[1].data.syzcomp.length;
    if (k > l)
    {
      Warn("component %d larger then max %d in %s:%d",
            k, le, f, l);
    }
  }
  return _pGetComp(p) = (k);
}

Exponent_t pDBDecrComp(poly p, char* f, int l)
{
  if (_pGetComp(p) < 1)
  {
    Warn("decrement to negative component %d in %s:%d\n", _pGetComp(p), f, l);
  }
  return _pGetComp(p)--;
}

Exponent_t pDBAddComp(poly p, Exponent_t k, int le, char* f, int l)
{
  if (_pGetComp(p) + k < 0)
  {
    Warn("add to negative component %d + %d = %d in %s:%d\n", _pGetComp(p),
          k, _pGetComp(p) + k, f, l);
  }
  _pGetComp(p) += (k);

  if (currRing->order[1] == ringorder_S)
  {
    if (le <= 0) le = currRing->typ[1].data.syzcomp.length;
    if (_pGetComp(p) > le)
    {
      Warn("sum of components %d larger then max %d in %s:%d\n",
            _pGetComp(p), le, f, l);
      assume(0);
    }
  }
  return _pGetComp(p);
}

Exponent_t pDBSubComp(poly p, Exponent_t k, char* f, int l)
{
  if (_pGetComp(p) - k < 0)
  {
    Warn("sub to negative component %d - %d = %d in %s:%d\n", _pGetComp(p),
          k, _pGetComp(p) - k, f, l);
  }
  return _pGetComp(p) -= (k);
}

Exponent_t pDBRingSetComp(ring r, poly p, Exponent_t k, char* f, int l)
{
  if (k < 0)
  {
    Warn("set negative component %d in %s:%d\n", k, f, l);
  }
  return _pRingGetComp(r, p) = (k);
}

// checks whether fast monom add did not overflow
void pDBMonAddOn(poly p1, poly p2, char* f, int l)
{
  poly ptemp = pHead0(p1);

  if (pGetComp(p1) != 0 && pGetComp(p2) != 0)
  {
    Warn("Error in pMonAddOn: both components %d:%d !=0 in %s:%d",
         pGetComp(p1), pGetComp(p2), f, l);
  }

  __pMonAddOn(p1, p2);

  for (int i=1; i<=pVariables; i++)
  {
    pAddExp(ptemp, i, pGetExp(p2, i));
  }
  pAddComp(ptemp, pGetComp(p2));
  pSetm(ptemp);

  if (! pEqual(ptemp, p1))
  {
    Warn("Error in pMonAddOn in %s:%d\n", f, l);
  }

  pFree1(ptemp);
}

void pDBMonSubFrom(poly p1, poly p2, char* f, int l)
{
  poly ptemp = pNew();
  pCopy2(ptemp, p1);

  if ((pGetComp(p1) != pGetComp(p2)) && (pGetComp(p2)!=0))
  {
    Warn("Error in pMonSubFrom: components are different %d:%d in %s:%d",
         pGetComp(p1), pGetComp(p2), f, l);
  }

  __pMonSubFrom(p1, p2);

  for (int i=1; i<=pVariables; i++)
  {
    if (pGetExp(ptemp, i) < pGetExp(p2, i))
    {
      Warn("Error in pMonSubFrom: %dth exponent %d of p1 smaller than %d of p2", i, pGetExp(ptemp, i), pGetExp(p2, i));
    }
    pSubExp(ptemp, i, pGetExp(p2, i));
  }
  pSetComp(ptemp, pGetComp(ptemp)-pGetComp(p2));
  pSetm(ptemp);

  if (! pEqual(ptemp, p1))
  {
    Warn("Error in pMonSubFrom in %s:%d", f, l);
  }

  pFree1(ptemp);
}

void pDBMonAdd(poly p1, poly p2, poly p3, char* f, int l)
{
  if (pGetComp(p3) != 0 && pGetComp(p2) != 0)
  {
    Warn("Error in pMonAdd: both components %d:%d !=0 in %s:%d",
         pGetComp(p3), pGetComp(p2), f, l);
  }
  if (p2 == p1 || p3 == p1)
  {
    Warn("Error in pMonAdd: Destination equals source in %s:%d", f, l);
  }
  __pMonAdd(p1, p2, p3);

  
  poly ptemp = pInit();
  for (int i=1; i<=pVariables; i++)
  {
    pSetExp(ptemp, i, pGetExp(p2, i) + pGetExp(p3, i));
    if (pGetExp(ptemp, i) != pGetExp(p1, i))
    {
      Warn("Error in pMonAdd: %th exponent: %d != (%d == %d + %d)",
           i, pGetExp(p1, i), pGetExp(ptemp, i), pGetExp(p2, i), 
           pGetExp(p3, i));
    }
  }
  pSetComp(ptemp, pGetComp(p2) + pGetComp(p3));
  pSetm(ptemp);

  if (! pEqual(ptemp, p1))
    Warn("Error in pMonAdd in %s:%d", f, l);
  pFree1(ptemp);
}

static BOOLEAN OldpDivisibleBy(poly a, poly b)
{
  if ((a!=NULL)&&((pGetComp(a)==0) || (pGetComp(a) == pGetComp(b))))
  {
    for (int i = 1; i<=pVariables; i++)
      if (pGetExp(a, i) > pGetExp(b,i)) return FALSE;
    return TRUE;
  }
  return FALSE;
}


BOOLEAN pDBDivisibleBy(poly a, poly b, char* f, int l)
{
  BOOLEAN istrue = OldpDivisibleBy(a,b);
  BOOLEAN f_istrue = _pDivisibleBy_orig(a, b);

  if (istrue != f_istrue)
  {
    Warn("Error in pDivisibleBy in %s:%d\n", f, l);
    _pDivisibleBy_orig(a, b);
  }
  return istrue;
}

BOOLEAN pDBDivisibleBy1(poly a, poly b, char* f, int l)
{
  BOOLEAN istrue = OldpDivisibleBy(a,b);
  BOOLEAN f_istrue = _pDivisibleBy1_orig(a, b);

  if (istrue != f_istrue)
  {
    Warn("Error in pDivisibleBy1 in %s:%d\n", f, l);
    _pDivisibleBy1_orig(a, b);
  }
  return istrue;
}

BOOLEAN pDBDivisibleBy2(poly a, poly b, char* f, int l)
{
  BOOLEAN istrue = OldpDivisibleBy(a,b);
  BOOLEAN f_istrue = __pDivisibleBy(a, b);

  if (istrue != f_istrue)
  {
    Warn("Error in pDivisibleBy2 in %s:%d\n", f, l);
    __pDivisibleBy(a, b);
  }
  return f_istrue;
}

#endif //  defined(PDEBUG) && PDEBUG > 1

#ifdef PDEBUG
BOOLEAN pDBTest(poly p, char *f, int l)
{
  return pDBTest(p, mm_specHeap, f,l);
}

BOOLEAN pDBTest(poly p, memHeap heap, char *f, int l)
{
  poly old=NULL;
  BOOLEAN ismod=FALSE;
  if (heap == NULL) heap = mm_specHeap;

  while (p!=NULL)
  {
    if (heap != MM_UNKNOWN_HEAP)
    {
#ifdef MDEBUG
    if (!mmDBTestHeapBlock(p, heap, f,l))
      return FALSE;
#elif defined(HEAP_DEBUG)
      if (! mmDebugCheckHeapAddr(p, heap, MM_HEAP_ADDR_USED_FLAG, f, l))
        return FALSE;
#endif
    }

#ifdef LDEBUG
    if (!nDBTest(p->coef,f,l))
      return FALSE;
#endif
    if ((p->coef==NULL)&&(nGetChar()<2))
    {
      Warn("NULL coef in poly in %s:%d\n",f,l);
      return FALSE;
    }
    if (nIsZero(p->coef))
    {
      Warn("zero coef in poly in %s:%d\n",f,l);
      return FALSE;
    }
    int i=pVariables;
    for(;i;i--)
    {
      if (pGetExp(p,i)<0)
      {
        Warn("neg. Exponent %d of x(%d) in %s:%d\n",pGetExp(p,i),i,f,l);
        return FALSE;
      }
    }
    if (pGetComp(p)<0)
    {
      Warn("neg Component in %s:%d\n",f,l);
      return FALSE;
    }
    if (ismod==0)
    {
      if (pGetComp(p)>0) ismod=1;
      else               ismod=2;
    }
    else if (ismod==1)
    {
      if (pGetComp(p)==0)
      {
        Warn("mix vec./poly in %s:%d\n",f,l);
        return FALSE;
      }
    }
    else if (ismod==2)
    {
      if (pGetComp(p)!=0)
      {
        Warn("mix poly/vec. in %s:%d\n",f,l);
        return FALSE;
      }
    }
    if (currRing->order[1] == ringorder_S)
    {
      long c1, cc1, ccc1, ec1;
      sro_ord* o = &(currRing->typ[1]);

      c1 = pGetComp(p);
      cc1 = o->data.syzcomp.Components[c1];
      ccc1 = o->data.syzcomp.ShiftedComponents[cc1];
      if (! (c1 == 0 || cc1 != 0))
      {
        Warn("Component <-> TrueComponent zero mismatch\n", f, l);
        return FALSE;
      }
      if (! (c1 == 0 || ccc1 != 0))
      {
        Warn("Component <-> ShiftedComponent zero mismatch\n", f, l);
        return FALSE;
      }
      ec1 = p->exp.l[currRing->typ[1].data.syzcomp.place];
      if (ec1 != ccc1)
      {
        Warn("Shifted comp out of sync. should %d, is %d", ccc1, ec1);
        return FALSE;
      }
    }

    old=p;
    pIter(p);
    if (pComp(old,p)!=1)
    {
      WarnS("wrong order (");
      wrp(old);
      Warn(") in %s:%d (pComp=%d)\n",f,l,pComp(old,p));
      return FALSE;
    }
  }
  return TRUE;
}
#endif // PDEBUG

static unsigned long GetBitFields(Exponent_t e,
                                  unsigned int s, unsigned int n)
{
  unsigned int i = 0, ev = 0;
  assume(n > 0 && s < BIT_SIZEOF_LONG);
  do
  {
    assume(s+i < BIT_SIZEOF_LONG);
    if (e > (Exponent_t) i) ev |= Sy_bit(s+i);
    else break;
    i++;
  }
  while (i < n);
  return ev;
}

// Short Exponent Vectors are used for fast divisibility tests
// ShortExpVectors "squeeze" an exponent vector into one word as follows:
// Let n = BIT_SIZEOF_LONG / pVariables, then each exponent is
// represented by a bit-field of length n (resp. n+1 for some
// exponents). If the value of an exponent is greater or equal to n, then
// all of its respective n bits are set to 1. If the value of an exponent
// is smaller than n, say m, then only the first m bits of the respective
// n bits are set to 1, the others are set to 0. This way, we have:
// exp1 / exp2 ==> (ev1 & ~ev2) == 0, i.e.,
// if (ev1 & ~ev2) then exp1 does not divide exp2
unsigned long pGetShortExpVector(poly p)
{
  unsigned long ev = 0; // short exponent vector
  unsigned int n = BIT_SIZEOF_LONG / pVariables; // number of bits per exp
  unsigned int m1; // highest bit which is filled with (n+1)
  unsigned int i = 0, j=1;

  if (n == 0)
  {
    n = 1;
    m1 = 0;
  }
  else
  {
    m1 = (n+1)*(BIT_SIZEOF_LONG - n*pVariables);
  }

  n++;
  while (i<m1)
  {
    ev |= GetBitFields(pGetExp(p, j), i, n);
    i += n;
    j++;
  }

  n--;
  while (i<BIT_SIZEOF_LONG)
  {
    ev |= GetBitFields(pGetExp(p, j), i, n);
    i += n;
    j++;
  }
  return ev;
}

#ifdef PDEBUG
BOOLEAN pDBShortDivisibleBy(poly p1, unsigned long sev_1,
                            poly p2, unsigned long not_sev_2, 
                            char* f, int l)
{
  if (pGetShortExpVector(p1) != sev_1)
  {
    Warn("sev1 is %o but should be %o in %s:%d\n", sev_1, 
          pGetShortExpVector(p1), f, l);
    assume(0);
  }
  if (~ pGetShortExpVector(p2) != not_sev_2)
  {
    Warn("not_sev2 is %o but should be %o in %s:%d\n", not_sev_2, 
          ~ pGetShortExpVector(p2), f, l);
    assume(0);
  }
  if (sev_1 & not_sev_2)
  {
    if (pDivisibleBy(p1, p2))
    {
      Warn("p1 divides p2, but sev's are wrong in %s:%d\n", f, l);
      assume(0);
      return TRUE;
    }
    return FALSE;
  }
  return pDivisibleBy(p1, p2);
}
#endif

#endif // POLYS_IMPL_CC
