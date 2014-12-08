/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
///////////////////////////////////////////////////////////////// */

#include "FoamPartition.hpp"

///////////////////////////////////////////////////////////////////////////////////////

FoamPartition::FoamPartition(int len)
{
    int k;
    m_len = len;
    if (m_len>0)
    {
        m_digits = new int[m_len];
        for (k=0;k<m_len;k++) m_digits[k]=0;
    }
    else
    {
        m_digits = 0;
    }
}

///////////////////////////////////////////////////////////////////////////////////////

FoamPartition::~FoamPartition()
{
    delete[] m_digits;
}

///////////////////////////////////////////////////////////////////////////////////////

void
FoamPartition::Reset()
{
    for (int k=0; k<m_len; k++)
    {
        m_digits[k]=0;
    }
}

///////////////////////////////////////////////////////////////////////////////////////

int
FoamPartition::Next()
{
    int j,status;
    if (m_len>0)
    {
        status=0;
        m_digits[m_len-1]++;           // Basic increment
        for (j=m_len-1; j>=0; j--)
        {
            if (m_digits[j]==2)        // Overflow goes to left-most digits
            {
                m_digits[j]=0;
                if (j>0) m_digits[j-1]++;
            }
            status +=m_digits[j];
        }
    }
    else
    {
        status=0;
    }
    return status;        // returns 0 AFTER last partition
}

///////////////////////////////////////////////////////////////////////////////////////

int
FoamPartition::Digit(int i)
{
    if (m_len>0)
        return m_digits[i];
    else
        return m_len;
}

///////////////////////////////////////////////////////////////////////////////////////
