/*---------------------------------------------------------------------------*\
  =========                 |
  \\      /  F ield         | OpenFOAM: The Open Source CFD Toolbox
   \\    /   O peration     |
    \\  /    A nd           | Copyright held by original author
     \\/     M anipulation  |
-------------------------------------------------------------------------------
License
    This file is part of OpenFOAM.

    OpenFOAM is free software; you can redistribute it and/or modify it
    under the terms of the GNU General Public License as published by the
    Free Software Foundation; either version 2 of the License, or (at your
    option) any later version.

    OpenFOAM is distributed in the hope that it will be useful, but WITHOUT
    ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
    FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
    for more details.

    You should have received a copy of the GNU General Public License
    along with OpenFOAM; if not, write to the Free Software Foundation,
    Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA

\*---------------------------------------------------------------------------*/

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace Foam
{

// * * * * * * * * * * * * * * * Member Functions  * * * * * * * * * * * * * //

inline const scalar& timerPlcTrigger::duration() const
{
    return duration_;
}


inline const bool& timerPlcTrigger::valueWhenActive() const
{
    return valueWhenActive_;
}


inline const scalar& timerPlcTrigger::roundOff() const
{
    return roundOff_;
}


inline const timerPlcTrigger::startAtEnum& timerPlcTrigger::startAt() const
{
    return startAt_;
}


inline const word& timerPlcTrigger::startAtTriggerName() const
{
    return startAtTriggerName_;
}


inline const plcTrigger * timerPlcTrigger::startAtTriggerPtr() const
{
    return startAtTriggerPtr_;
}


inline const label& timerPlcTrigger::nRepeat() const
{
    return nRepeat_;
}


inline const bool& timerPlcTrigger::active() const
{
    return active_;
}


inline const scalar& timerPlcTrigger::lastGlobalStartTime() const
{
    return lastGlobalStartTime_;
}


inline const label& timerPlcTrigger::nRepeatsUsed() const
{
    return nRepeatsUsed_;
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace Foam

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

// ************************************************************************* //
