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

#include "timerPlcTrigger.H"
#include "addToRunTimeSelectionTable.H"
#include "plcEmulator.H"

// * * * * * * * * * * * * * * Static Data Members * * * * * * * * * * * * * //

namespace Foam
{
    defineTypeNameAndDebug(timerPlcTrigger, 0);

    addToRunTimeSelectionTable
    (
        plcTrigger,
        timerPlcTrigger,
        dictionary
    );
}

// * * * * * * * * * * * * * Private Member Functions  * * * * * * * * * * * //


// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //

Foam::timerPlcTrigger::timerPlcTrigger
(
    plcEmulator& control,
    const word& triggerName,
    const dictionary& inputDict
)
:
    plcTrigger(control, triggerName, inputDict),

    duration_(readScalar(inputDict.lookup("duration"))),
    valueWhenActive_
    (
        inputDict.found("valueWhenActive")
      ? Switch(inputDict.lookup("valueWhenActive"))
      : Switch(true)
    ),
    roundOff_(readScalar(inputDict.lookup("roundOff"))),
    startAt_(saunknown),
    startAtTriggerName_(word::null),
    startAtTriggerPtr_(NULL),
    nRepeat_
    (
        inputDict.found("nRepeat")
      ? readLabel(inputDict.lookup("nRepeat"))
      : -1
    ),
    active_(value_ == valueWhenActive_),
    lastGlobalStartTime_(VGREAT),
    nRepeatsUsed_(0)
{
    word startAtWord(inputDict.lookup("startAt"));
    if (startAtWord == "runStart")
    {
        startAt_ = sarunStart;
        active_ = true;
        value_ = valueWhenActive_;
    }
    else if (startAtWord == "trigger")
    {
        startAt_ = satrigger;
        startAtTriggerName_ = word(inputDict.lookup("trigger"));
    }
    else
    {
        FatalIOErrorIn("timerPlcTrigger::timerPlcTrigger", inputDict)
            << "timerPlcTrigger " << name_ << " has unrecognized startAt "
            << "option '" << startAtWord << "'. Available startAt options "
            << "are: ( runStart trigger )"
            << exit(FatalIOError);
    }

    if (active_)
    {
        lastGlobalStartTime_ = 0;
    }
}

// * * * * * * * * * * * * * * * Member Functions  * * * * * * * * * * * * * //

bool Foam::timerPlcTrigger::evaluate(const Time& runTime)
{
    // Check if startAtTriggerPtr needs to be initialized
    if ((startAt_ == satrigger) && !startAtTriggerPtr_)
    {
        forAll(control_.triggers(), triggerI)
        {
            const plcTrigger& nextTrigger(control_.triggers()[triggerI]);
            if (nextTrigger.name() == startAtTriggerName_)
            {
                startAtTriggerPtr_ = & nextTrigger;
                break;
            }
        }
    }
    
    scalar globalTime
    (
        runTime.value() + control_.multiRun().globalTimeOffset()
    );

    // Pseudo code for the function of this trigger. Tricky bit is getting a
    // sarunTime trigger to send a "pulse" of !valueWhenActive_ because it is
    // always active.
    //
    //  is active_ true?
    //  yes
    //      have i expired?
    //      yes
    //          set active_ to false
    //          value_ = !valueWhenActive_
    //      no
    //          am i sarunTime? && globalTime != lastGlobalStartTime?
    //          yes
    //              value_ = valueWhenActive_
    //          no
    //              do nothing
    //  no
    //      do nothing
    //  is active_ false && nRepeats != 0?
    //  yes
    //      am i a sarunTime or is my startTrigger true?
    //      yes
    //          set active_ to true
    //          set lastGlobalStartTime to globalTime
    //          nRepeatsUsed++
    //          if i am a satrigger, set value_ to valueWhenActive_
    //      no
    //          do nothing
    //  no
    //      do nothing

    if (active_)
    {
        if (lastGlobalStartTime_ + duration_ - globalTime < roundOff_)
        {
            reset();
        }
        else if
        (
            (startAt_ == sarunStart)
         && (mag(globalTime - lastGlobalStartTime_) > roundOff_)
        )
        {
            value_ = valueWhenActive_;
        }
    }
    if
    (
        !active_
     && ((nRepeat_ - nRepeatsUsed_) != 0)
     && ((startAt_ == sarunStart) || (startAtTriggerPtr_->value()))
    )
    {
        active_ = true;
        lastGlobalStartTime_ = globalTime;
        nRepeatsUsed_++;
        if (startAt_ == satrigger)
        {
            value_ = valueWhenActive_;
        }
    }

    return value_;
}


void Foam::timerPlcTrigger::reset()
{
    active_ = false;
    value_ = !valueWhenActive_;
    control_.timerExpired();
}

Foam::scalar Foam::timerPlcTrigger::globalEndTime() const
{
    if (!active_) return VGREAT;
    return (lastGlobalStartTime_ + duration_);
}


void Foam::timerPlcTrigger::readOutputDict
(
    const dictionary& outputSubDict
)
{
    active_ = readBool(outputSubDict.lookup("active"));
    value_ = (valueWhenActive_ == active_);

    lastGlobalStartTime_ = readScalar
    (
        outputSubDict.lookup("lastGlobalStartTime")
    );
    nRepeatsUsed_ = readLabel(outputSubDict.lookup("nRepeatsUsed"));
}


Foam::dictionary Foam::timerPlcTrigger::writeOutputDict() const
{
    dictionary returnMe;
    returnMe.set("active", active_);
    returnMe.set("lastGlobalStartTime", lastGlobalStartTime_);
    returnMe.set("nRepeatsUsed", nRepeatsUsed_);
    return returnMe;
}

// ************************************************************************* //
