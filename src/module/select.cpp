/*
 * select.cpp
 * 
 * Copyright 2025 x64x2 <x64x2@mango>
 * 
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
 * MA 02110-1301, USA.
 * 
 * 
 */


#include "../interp.h"
#include "select.h"

using namespace noise::module;

Select::Select ():
  Module (GetSourceModuleCount ()),
  m_lowerBound (DEFAULT_SELECT_LOWER_BOUND),
  m_upperBound (DEFAULT_SELECT_UPPER_BOUND),
  m_edgeFalloff (DEFAULT_SELECT_EDGE_FALLOFF)
{
}

double Select::GetValue (double x, double y, double z) const
{
  assert (m_pSourceModule[0] != NULL);
  assert (m_pSourceModule[1] != NULL);
  assert (m_pSourceModule[2] != NULL);

  double controlValue = m_pSourceModule[2]->GetValue (x, y, z);
  double alpha;
  if (m_edgeFalloff > 0.0) {
    if (controlValue < (m_lowerBound - m_edgeFalloff)) {
      /// The output value from the control module is below the selector
      /// threshold; return the output value from the first source module.
      return m_pSourceModule[0]->GetValue (x, y, z);

    } else if (controlValue < (m_lowerBound + m_edgeFalloff)) {
      /// The output value from the control module is near the lower end of the
      /// selector threshold and within the smooth curve. Interpolate between
      /// the output values from the first and second source modules.
      double lowerCurve = (m_lowerBound - m_edgeFalloff);
      double upperCurve = (m_lowerBound + m_edgeFalloff);
      alpha = SCurve3 (
        (controlValue - lowerCurve) / (upperCurve - lowerCurve));
      return LinearInterp (m_pSourceModule[0]->GetValue (x, y, z),
        m_pSourceModule[1]->GetValue (x, y, z),
        alpha);

    } else if (controlValue < (m_upperBound - m_edgeFalloff)) {
      /// The output value from the control module is within the selector
      /// threshold; return the output value from the second source module.
      return m_pSourceModule[1]->GetValue (x, y, z);

    } else if (controlValue < (m_upperBound + m_edgeFalloff)) {
      /// The output value from the control module is near the upper end of the
      /// selector threshold and within the smooth curve. Interpolate between
      /// the output values from the first and second source modules.
      double lowerCurve = (m_upperBound - m_edgeFalloff);
      double upperCurve = (m_upperBound + m_edgeFalloff);
      alpha = SCurve3 (
        (controlValue - lowerCurve) / (upperCurve - lowerCurve));
      return LinearInterp (m_pSourceModule[1]->GetValue (x, y, z),
        m_pSourceModule[0]->GetValue (x, y, z),
        alpha);

    } else {
      /// Output value from the control module is above the selector threshold;
      /// return the output value from the first source module.
      return m_pSourceModule[0]->GetValue (x, y, z);
    }
  } else {
    if (controlValue < m_lowerBound || controlValue > m_upperBound) {
      return m_pSourceModule[0]->GetValue (x, y, z);
    } else {
      return m_pSourceModule[1]->GetValue (x, y, z);
    }
  }
}

void Select::SetBounds (double lowerBound, double upperBound)
{
  assert (lowerBound < upperBound);

  m_lowerBound = lowerBound;
  m_upperBound = upperBound;

  /// Make sure that the edge falloff curves do not overlap.
  SetEdgeFalloff (m_edgeFalloff);
}

void Select::SetEdgeFalloff (double edgeFalloff)
{
  /// Make sure that the edge falloff curves do not overlap.
  double boundSize = m_upperBound - m_lowerBound;
  m_edgeFalloff = (edgeFalloff > boundSize / 2)? boundSize / 2: edgeFalloff;
}
