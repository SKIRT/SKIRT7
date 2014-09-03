/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
///////////////////////////////////////////////////////////////// */

#include "FatalError.hpp"
#include "FoamCell.hpp"
#include "FoamVector.hpp"

///////////////////////////////////////////////////////////////////////////////

FoamCell::FoamCell(int nDim, int kDim, int OptMCell, int OptCu1st)
{
    // Constructor of a single Cell, vertices assigned later on
    if (nDim+kDim > 0)
    {
        m_nDim     = nDim;
        m_kDim     = kDim;
        m_OptMCell = OptMCell;   // MegaCell option, slim memory for h-cubes
        m_OptCu1st = OptCu1st;   // =1 Numbering of dimens starts with h-cubic, DIPSWITCH

        m_nPool    = 0;
        m_Pool     = NULL;
        m_Status   = 1;
        m_Parent   =-1;
        m_Daught0  =-1;
        m_Daught1  =-1;
        m_Xdiv     = 0.0;
        m_Best     = 0;
        m_Volume   = 0.0;
        m_Integral = 0.0;
        m_Drive    = 0.0;
        m_Primary  = 0.0;
        // m_Verts is allocated  and filled in later on in Fill !!!!!
        m_nVert = 0;
        m_Verts = NULL;
        //--- hyp-cubical subspace ---
        m_Posi = NULL;
        m_Size = NULL;
        if( (m_kDim>0) && (m_OptMCell == 0) )
        {
            m_Posi = new FoamVector(m_kDim);
            m_Size = new FoamVector(m_kDim);
        }
    }
    else throw FATALERROR("Total dimesion has to be > 0");
}

///////////////////////////////////////////////////////////////////////////////

FoamCell::~FoamCell()
{
    delete[] m_Verts;
    delete m_Posi;
    delete m_Size;
}

////////////////////////////////////////////////////////////////////////////////

FoamVector*&
FoamCell::operator[](int i)
{
    // Getter of vertex true pointer
    if ((i>=0) && (i<=m_nDim)) return m_Vert0[ m_Verts[i] ];
    else throw FATALERROR("Index out of range");
}

////////////////////////////////////////////////////////////////////////////////

int
FoamCell::operator()(int i)
{
    // Getter of vertex integer pointer
    if ((i>=0) && (i<=m_nDim)) return m_Verts[i];
    else throw FATALERROR("Index out of range");
}

////////////////////////////////////////////////////////////////////////////////

void
FoamCell::Fill(int Status, int Parent, int Daugh1, int Daugh2,
               int* Vertices, FoamVector* Posi, FoamVector* Size)
{
    // Fills Cell which has to exist already
    int i;
    m_Status  = Status;
    m_Parent  = Parent;
    m_Daught0 = Daugh1;
    m_Daught1 = Daugh2;
    if(Vertices!=NULL)
    {
        if( (m_Verts != NULL) || (m_nVert != 0) )
            throw FATALERROR("m_Verts already allocated, m_nVert=" + QString::number(m_nVert));
        m_nVert = m_nDim+1;
        m_Verts = new int[m_nVert];
        for(i=0; i<m_nVert; i++) m_Verts[i] = Vertices[i];  // Pure Pointers
    }
    if(m_OptMCell == 0)
    {
        if(Posi!=NULL) (*m_Posi) = (*Posi);
        if(Size!=NULL) (*m_Size) = (*Size);
    }
}

////////////////////////////////////////////////////////////////////////////////

void
FoamCell::GetHcub(FoamVector& Posi, FoamVector& Size)
{
    // Sophisticated Getter for Hyper-Cubic dimensions and size
    if(m_kDim<1) return;
    if(m_OptMCell==0)
    {
        Posi=*m_Posi;
        Size=*m_Size;
    }
    else
    {
        FoamCell *pCell,*dCell;
        Posi = 0.0; Size=1.0; // load all components

        dCell = this;
        while(dCell != NULL)
        {
            pCell = dCell->GetPare();
            if( pCell== NULL) break;
            if((pCell->GetDau0()==NULL) && (pCell->GetDau1()==NULL)) break; //case nDim>0, kDim>0
            int    kBest = pCell->m_Best;
            double xDivi = pCell->m_Xdiv;
            int kShift; if(m_OptCu1st) kShift = 0; else  kShift=m_nDim*(m_nDim+1)/2 ;
            if( ( kShift <= kBest) && (kBest < kShift+m_kDim) )
            {    // only if division was in h-cubic subspace
                int kDiv = kBest-kShift;
                if( dCell == pCell->GetDau0() )
                {
                    Size[kDiv]=Size[kDiv]*xDivi;
                    Posi[kDiv]=Posi[kDiv]*xDivi;
                }
                else if( dCell == pCell->GetDau1() )
                {
                    Size[kDiv]=Size[kDiv]*(1.0-xDivi);
                    Posi[kDiv]=Posi[kDiv]*(1.0-xDivi)+xDivi;
                }
                else throw FATALERROR("Inconsistency in GetHcub");
            }
            dCell=pCell;
        }
    }
}

////////////////////////////////////////////////////////////////////////////////

void
FoamCell::GetHSize(FoamVector& Size)
{
    // Sophisticated Getter for size vector of h-cubic cell
    if(m_kDim<1) return;
    if(m_OptMCell==0)
    {
        Size=*m_Size;
    }
    else
    {
        FoamCell *pCell,*dCell;
        Size=1.0; // load all components
        dCell = this;
        while(dCell != NULL)
        {
            pCell = dCell->GetPare();
            if( pCell== NULL) break;
            if((pCell->GetDau0()==NULL) && (pCell->GetDau1()==NULL)) break; //case nDim>0, kDim>0
            int    kBest = pCell->m_Best;
            double xDivi = pCell->m_Xdiv;
            int kShift; if(m_OptCu1st) kShift = 0; else  kShift=m_nDim*(m_nDim+1)/2 ;
            if( ( kShift <= kBest) && (kBest < kShift+m_kDim) )    // only if division was in h-cubic subspace
            {
                int kDiv = kBest-kShift;
                if (dCell == pCell->GetDau0())
                    Size[kDiv]=Size[kDiv]*xDivi;
                else if (dCell == pCell->GetDau1())
                    Size[kDiv]=Size[kDiv]*(1.0-xDivi);
                else
                    throw FATALERROR("Inconsistency in GetHSize");
            }
            dCell=pCell;
        }
    }
}

////////////////////////////////////////////////////////////////////////////////

void
FoamCell::GetXSimp(FoamVector& X, FoamVector& lambda, int kVert)
{
    // Translates internal simplicial coordinates into absolute
    FoamCell *pCell,*dCell;
    FoamVector *p;
    int i,j,k,iDiv,jDiv,kVold,kBest;
    long *PtsDiv;
    double sum,xDivi;
    if (m_nDim<2) throw FATALERROR("GetXSimp: no simplicial subspace");
    PtsDiv = new long [(m_nDim+1)*m_nDim/2];
    k=0;
    X=0.;
    for(i=0; i<m_nDim+1; i++)
        for(j=i+1; j<m_nDim+1; j++)
            PtsDiv[k++]=(m_nDim+1)*i+j;

    dCell = this;
    sum=0.;
    lambda[kVert]=0.;
    for(i=0; i< m_nDim+1; i++) sum+=lambda[i];
    int kShift;
    if(m_OptCu1st) kShift = m_kDim; else  kShift=0 ;
    while(dCell != NULL)
    {
        pCell = dCell->GetPare();
        if( pCell== NULL) break;
        if((pCell->GetDau0()==NULL) && (pCell->GetDau1()==NULL))
            break; //case of starting cubic divided into simplices
        kBest = pCell->m_Best;
        if(kBest>=kShift )  // if division was in simplicial subspace
        {
            xDivi = pCell->m_Xdiv;
            iDiv=PtsDiv[kBest-kShift]/(m_nDim+1);
            jDiv=PtsDiv[kBest-kShift] % (m_nDim+1);
            kVold=kVert;
            if( dCell == pCell->GetDau0() )
            {
                kVert=jDiv;
                if(kVert!=kVold)
                {
                    lambda[kVold]=1.-sum;
                    sum=1.-lambda[kVert];
                    lambda[kVert]=0.;
                }
                sum+=(xDivi-1)*lambda[iDiv];
                lambda[iDiv]*=xDivi;
            }
            else if( dCell == pCell->GetDau1() )
            {
                kVert=iDiv;
                if(kVert!=kVold)
                {
                    lambda[kVold]=1.-sum;
                    sum=1.-lambda[kVert];
                    lambda[kVert]=0.;
                }
                sum+=-xDivi*lambda[jDiv];
                lambda[jDiv]*=(1.-xDivi);
            }
        }
        if(dCell != pCell->GetDau0() && dCell != pCell->GetDau1())
            throw FATALERROR("Inconsistency in GetXSimp");
        dCell=pCell;
    }
    for(i=0; i<m_nDim+1; i++)
    {
        p=(*dCell)[i];
        if(i==kVert)
        {
            for(j=0; j<m_nDim; j++)
                X[j]+=(1.-sum+lambda[kVert])*p->GetCoord(j);
        }
        else
        {
            for(j=0; j<m_nDim; j++)
                X[j]+=lambda[i]*p->GetCoord(j);
        }
    }
    delete PtsDiv;
    return;
}

//////////////////////////////////////////////////////////////////////////////////

void
FoamCell::CalcVolume()
{
    FoamCell *pCell,*dCell;
    int i,k,kBest;
    double xDivi,volu;
    int kShift;
    volu=1.0;
    if(m_nDim>0)
    {
        if(m_OptCu1st) kShift = m_kDim; else  kShift=0 ;
        dCell = this;
        while(dCell != NULL)
        {
            pCell = dCell->GetPare();
            if( pCell== NULL) break;
            if((pCell->GetDau0()==NULL) && (pCell->GetDau1()==NULL))
                break; //case of starting cubic divided into simplices
            kBest = pCell->m_Best;
            if(kBest>=kShift )  //If division was in simplicial subspace
            {
                xDivi = pCell->m_Xdiv;
                if( dCell == pCell->GetDau0() )
                {
                    volu*=xDivi;
                }
                else if( dCell == pCell->GetDau1() )
                {
                    volu*=(1.-xDivi);
                }
            }
            if(dCell != pCell->GetDau0() && dCell != pCell->GetDau1())
                throw FATALERROR("Inconsistency in CalcVolume");
            dCell=pCell;
        }
        for(i=2; i<=m_nDim; i++) volu /= i;  // divide by factorial
    }
    if(m_kDim>0)         // h-cubical subspace
    {
        if(m_OptMCell == 0)
        {
            for(k=0; k<m_kDim; k++) volu *= (*m_Size)[k];
        }
        else
        {
            FoamVector Size(m_kDim);
            (this)->GetHSize(Size);
            for(k=0; k<m_kDim; k++) volu *= Size[k];
        }
    }
    m_Volume =volu;
}

///////////////////////////////////////////////////////////////////////////////////////
