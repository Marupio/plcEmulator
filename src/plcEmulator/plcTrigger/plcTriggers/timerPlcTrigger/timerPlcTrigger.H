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

Class
    Foam::timerPlcTrigger

Description
    A plcTrigger that operates a timer.
    
SourceFiles
    timerPlcTrigger.C

Author
    David L. F. Gaden

\*---------------------------------------------------------------------------*/

#ifndef timerPlcTrigger_H
#define timerPlcTrigger_H

#include "plcTrigger.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace Foam
{

/*---------------------------------------------------------------------------*\
                    Class timerPlcTrigger Declaration
\*---------------------------------------------------------------------------*/

class timerPlcTrigger
:
    public plcTrigger
{

public:

    // Public data

        // Enumeration for startAt setting
        enum startAtEnum
        {
            saunknown,
            satrigger,
            sarunStart
        };

private:

    // Private member data
    
        // Data that does not change during a run

            //- Duration
            const scalar duration_;
            
            //- Value when active
            const bool valueWhenActive_;
            
            //- Round off tolerance
            const scalar roundOff_;
            
            //- "Start at" behaviour
            startAtEnum startAt_;
            
            //- Trigger name, if startAt == satrigger
            word startAtTriggerName_;
            
            //- Start at trigger pointer (set on first evaluate())
            const plcTrigger * startAtTriggerPtr_;
            
            //- Number of times this timer will repeat:
            //     -1 = always, 0 = run once, etc..
            const label nRepeat_;
        
        // Data that changes during a run
        
            //- True when running
            bool active_;
            
            //- Latest global time when this timer started running
            scalar lastGlobalStartTime_;
            
            //- Number of repeats used
            label nRepeatsUsed_;

public:

    //- Runtime type information
    TypeName("timer");


    // Constructors

        //- Construct from dictionary, given name
        timerPlcTrigger
        (
            plcEmulator& control,
            const word& triggerName,
            const dictionary& inputDict
        );

    // Destructor

        virtual ~timerPlcTrigger()
        {}


    // Member functions
    
        // Access

            //- Duration
            inline const scalar& duration() const;
            
            //- Value when active
            inline const bool& valueWhenActive() const;
        
            //- Round-off tolerance
            inline const scalar& roundOff() const;

            //- startAt behaviour
            inline const startAtEnum& startAt() const;
            
            //- Trigger name, if startAt == satrigger
            inline const word& startAtTriggerName() const;
            
            //- Start at trigger pointer (set on first evaluate())
            inline const plcTrigger * startAtTriggerPtr() const;
            
            //- Number of times this timer will repeat:
            //     -1 = always, 0 = run once, etc..
            inline const label& nRepeat() const;
        
            //- True when running
            inline const bool& active() const;
            
            //- Latest global time when this timer started running
            inline const scalar& lastGlobalStartTime() const;
            
            //- Number of repeats used
            inline const label& nRepeatsUsed() const;


        // Operation

            //- Evaluate
            virtual bool evaluate(const Time& runTime);
            
            //- Reset the running trigger
            void reset();
            
            //- End time of current timer - returns VGREAT if not running
            scalar globalEndTime() const;

        // Read and write
        
            //- Read the outputDict, used to get latest values for local data
            //  when resuming a simulation mid-run
            virtual void readOutputDict(const dictionary& outputSubDict);
            
            //- Write local data to a dictionary
            virtual dictionary writeOutputDict() const;
};


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace Foam

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#include "timerPlcTriggerI.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#endif

// ************************************************************************* //
