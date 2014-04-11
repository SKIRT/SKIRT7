/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
//////////////////////////////////////////////////////////////////*/

#ifndef FOAMCELL_HPP
#define FOAMCELL_HPP

class FoamVector;

////////////////////////////////////////////////////////////////////

/** A FoamCell instance represents a cell in the foam created by the Foam class. This class is part
    of the foam classes that have been copied from an external library and adjusted to fit the
    SKIRT mold. There is no documentation other than the sparse comments in the source code. */
class FoamCell
{
private:
    short int    m_kDim;        // Dimension of h-cubical subspace
    short int    m_nDim;        // Dimension of simplical subspace
    short int    m_OptMCell;    // Option Mega-Cell
    short int    m_OptCu1st;    // m_OptCubFirst=1, Numbering of dims starts with h-cubic, =0 simpl.
    int          m_nVert;       // No. of vertices in the simplex = m_nDim+1
    int          m_nPool;       // No of events in the MC pool
    FoamVector*  m_Pool;        // [m_nPool] Linked list of MC wt=1 event pool
    FoamCell**   m_Cell0;       // ! Common root cell, for positioning of pointers in the tree
    FoamVector** m_Vert0;       // ! Common root cell, for positioning of pointers in the tree
    int          m_Serial;      // Serial number (index in m_Cell0)
    int          m_Status;      // Status (active, inactive)
    int          m_Parent;      // Pointer to parent cell
    int          m_Daught0;     // Pointer to daughter 1
    int          m_Daught1;     // Pointer to daughter 2
    double       m_Xdiv;        // Factor for division
    int          m_Best;        // Best Edge for division
    double       m_Volume;      // Cartesian Volume of cell
    double       m_Integral;    // Integral over cell (estimate from exploration)
    double       m_Drive;       // Driver  integral, only for cell build-up
    double       m_Primary;     // Primary integral, only for MC generation
    int*         m_Verts;       // [m_nVert] Pointer to ARRAY of vertices in SIMPLEX subspace
    FoamVector*  m_Posi;        // Pointer to position vector, in H-CUBIC subspace
    FoamVector*  m_Size;        // Pointer to size vector, in H-CUBIC subspace
public:
    FoamCell(int, int, int, int);                           // Constructor
    ~FoamCell();                                            // Destructor
    void Fill(int, int, int, int, int*, FoamVector*, FoamVector*); // Assigns values of attributes
    FoamVector*& operator[](int);                           // Pointer to one of vertices
    int operator()(int);                                    // Integer pointer to one of vertices
    double  GetXdiv() { return m_Xdiv; }                    // Pointer to Xdiv
    int     GetBest() { return m_Best; }                    // Pointer to Best
    void    SetBest(int Best) { m_Best=Best; }              // Set Best edge candidate
    void    SetXdiv(double Xdiv) { m_Xdiv=Xdiv; }           // Set x-division for best edge cand.
    void    GetHcub(FoamVector&, FoamVector&);              // Get position and size vectors (h-cubical subspace)
    void    GetHSize(FoamVector&);                          // Get size only of cell vector  (h-cubical subspace)
    void    GetXSimp(FoamVector&, FoamVector&, int);
    void    CalcVolume();                                   // Calculates volume of cell
    double  GetVolume() { return m_Volume; }                // Volume of cell
    double  GetIntg() { return m_Integral; }                // Get Integral
    double  GetDriv() { return m_Drive; }                   // Get Drive
    double  GetPrim() { return m_Primary; }                 // Get Primary
    void    SetIntg(double Intg) { m_Integral=Intg; }       // Set true integral
    void    SetDriv(double Driv) { m_Drive=Driv; }          // Set driver integral
    void    SetPrim(double Prim) { m_Primary=Prim; }        // Set primary integral
    int     GetStat() { return m_Status; }                  // Get Status
    void    SetStat(int Stat) { m_Status=Stat; }            // Set Status
    int     GetnVert() { return m_nVert; }                  // Get No. of vertices in the list
    FoamCell* GetPare() { return m_Parent>=0 ? m_Cell0[m_Parent] : 0; }   // Get Pointer to pointer of parent vertex
    FoamCell* GetDau0() { return m_Daught0>=0 ? m_Cell0[m_Daught0] : 0; } // Get Pointer to 1-st daughter vertex
    FoamCell* GetDau1() { return m_Daught1>=0 ? m_Cell0[m_Daught1] : 0; } // Get Pointer to 2-nd daughter vertex
    void   SetDau0(int Daug) { m_Daught0=Daug; }            // Set pointer to 1-st daughter
    void   SetDau1(int Daug) { m_Daught1=Daug; }            // Set pointer to 2-nd daughter
    void   SetSerial(int Serial){ m_Serial=Serial; }        // Set serial number
    int    GetSerial() { return m_Serial; }                 // Get serial number
    void   SetCell0(FoamCell** Cell0) { m_Cell0=Cell0; }    // Set root cell pointer
    void   SetVert0(FoamVector**Vert0){ m_Vert0=Vert0; }    // Set root vertex pointer
private:
    FoamCell();
    FoamCell(const FoamCell&);
    FoamCell& operator=(const FoamCell&);
};

/////////////////////////////////////////////////////////////////////////////

#endif // FOAMCELL_HPP
