/*
 * voronoi.cpp
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

#include "../mathconsts.h"
#include "voronoi.h"

using namespace noise::module;

Voronoi::Voronoi ():
  Module (GetSourceModuleCount ()),
  m_displacement   (DEFAULT_VORONOI_DISPLACEMENT),
  m_frequency      (DEFAULT_VORONOI_FREQUENCY   ),
  m_seed           (DEFAULT_VORONOI_SEED        ),
  m_enableDistance (false)
{
}

double Voronoi::GetValue (double x, double y, double z) const
{
  // This method could be more efficient by caching the seed values.  Fix
  // later.

  x *= m_frequency;
  y *= m_frequency;
  z *= m_frequency;

  int xInt = (int)(floor (x));
  int yInt = (int)(floor (y));
  int zInt = (int)(floor (z));

  double minDist = 2147483647.0;
  double xCandidate;
  double yCandidate;
  double zCandidate;

  /// inside each unit cube, there is a seed point at a random position.  Go
  /// through each of the nearby cubes until we find a cube with a seed point
  /// that is closest to the specified position.
  for (int zCur = zInt - 2; zCur <= zInt + 2; zCur++) {
    for (int yCur = yInt - 2; yCur <= yInt + 2; yCur++) {
      for (int xCur = xInt - 2; xCur <= xInt + 2; xCur++) {

        /// Calculate the position and distance to the seed point inside of
        /// this unit cube.
        double xPos = xCur + ValueNoise3D (xCur, yCur, zCur, m_seed    );
        double yPos = yCur + ValueNoise3D (xCur, yCur, zCur, m_seed + 1);
        double zPos = zCur + ValueNoise3D (xCur, yCur, zCur, m_seed + 2);
        double xDist = xPos - x;
        double yDist = yPos - y;
        double zDist = zPos - z;
        double dist = xDist * xDist + yDist * yDist + zDist * zDist;

        if (dist < minDist) {
          /// This seed point is closer to any others found so far, so record
          /// this seed point.
          minDist = dist;
          xCandidate = xPos;
          yCandidate = yPos;
          zCandidate = zPos;
        }
      }
    }
  }

  double value;
  if (m_enableDistance) {
    /// Determine the distance to the nearest seed point.
    double xDist = xCandidate - x;
    double yDist = yCandidate - y;
    double zDist = zCandidate - z;
    value = (sqrt (xDist * xDist + yDist * yDist + zDist * zDist)
      ) * SQRT_3 - 1.0;
  } else {
    value = 0.0;
  }

  /// Return the calculated distance with the displacement value applied.
  return value + (m_displacement * (double)ValueNoise3D (
    (int)(floor (xCandidate)),
    (int)(floor (yCandidate)), 
    (int)(floor (zCandidate))));
}
