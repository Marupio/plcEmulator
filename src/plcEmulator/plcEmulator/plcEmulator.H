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
    Foam::plcEmulator

Description
    A crude PLC type of plcEmulator for multiSolver.
    
SourceFiles
    plcEmulatorI.H
    plcEmulator.C

Author
    David L. F. Gaden

\*---------------------------------------------------------------------------*/


#ifndef plcEmulator_H
#define plcEmulator_H

#include "plcTrigger.H"
#include "plcLogic.H"
#include "multiSolver.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace Foam
{

// Forward declarations
class solverSignalPlcTrigger;
class timerPlcTrigger;

/*---------------------------------------------------------------------------*\
                           Class plcEmulator Declaration
\*---------------------------------------------------------------------------*/

class plcEmulator
{

    // Private data
        
        //- multiSolver reference
        multiSolver& multiRun_;

        //- Main input dictionary user-supplied details
        const IOdictionary plcEmulatorDict_;
        
        //- Output dictionary - copied into IOobject at output time
        dictionary outputDict_;

        //- Output dictionary name
        word outputDictName_;

        //- Output dict has been read - flag needed because we cannot read the
        //  output dictionary to get the latest trigger data until after the
        //  first solver domain comes into scope and we get the runTime object
        bool outputDictRead_;

        // Triggers
        
            //- All triggers
            PtrList<plcTrigger> triggers_;
        
            //- Trigger subsets
            UPtrList<timerPlcTrigger> timerTriggers_;
            UPtrList<solverSignalPlcTrigger> solverSignalTriggers_;
        
        //- Logic tests
        PtrList<plcLogic> logic_;

        // Solver domains and algorithms

            //- Solver domains - full list of valid solver domains
            const wordList& solverDomains_;

            //- Algorithm names
            wordList algorithms_;

            //- Algorithm groups: algorithm_[i] is used by
            //  solverDomains_[algorithmGroups[i][j]] for all j
            labelListList algorithmGroups_;

        // Current state
        
            //- Current solver domain
            label currentSolverDomain_;
            
            //- Current algorithm
            label currentAlgorithm_;

        // Data that changes during a run
            
            //- Global time at which the next timer will expire
            scalar nextTimerEnd_;

            //- Round-off tolerance of next expiring timer
            scalar nextTimerEndRoundOff_;
            
            //- True when a timer expired this iteration (set by a
            //  timerPlcTrigger)
            bool timerExpired_;
            
            //- True if endTime was adjusted to meet a timer trigger end
            bool endTimeChanged_;
            
            //- Stores the unaltered endTime
            scalar oldEndTime_;
            
            //- True if the deltaT was adjusted to prevent overrunning a timer
            //  trigger end
            bool deltaTChanged_;
            
            //- Stores the unaltered deltaT
            scalar oldDeltaT_;
            
            //- True if the write interval was adjusted (a bug work around to
            //  keep runTime from fighting a deltaT change when writeControl is
            //  set to adjustableRunTime)
            bool writeIntervalChanged_;
            

    // Private member functions

        //- Disallow default bitwise copy construct and assignment
        void operator=(const plcEmulator&);
        plcEmulator(const plcEmulator&);

        //- Read input dictionary
        void readInputDict();
        
        //- Read output dictionary to get latest values (for initialization)
        void readOutputDict(Time& runTime);
        
        //- Check for timers that have expired and reset them
        void resetTimers(Time& runTime);
        
        //- Update triggers state in output dictionary
        void updateOutputDict();
        
        //- Given a solverDomain index, return the associated algorithm index
        label findAssociatedAlgorithm(label solverDomainIndex);

public:

    TypeName("plcEmulator");

    // Constructors

        //- Construct reading from dictionary
        plcEmulator
        (
            multiSolver& multiRun,
            word inputDictName = "plcEmulatorDict",
            word outputDictName = "plcTriggers"
        );

    // Destructor
    
        virtual ~plcEmulator()
        {}


    // Member Functions

        // Access

            //- multiSolver
            inline const multiSolver& multiRun() const;
            inline multiSolver& multiRun();

            //- plcEmulator dictionary
            inline const IOdictionary& plcEmulatorDict() const;

            //- plcEmulator output dictionary
            inline const dictionary& outputDict() const;
            
            //- Name of output dictionary
            inline const word& outputDictName() const;

            //- Triggers
            inline const PtrList<plcTrigger>& triggers() const;
            inline PtrList<plcTrigger>& triggers();

            //- Logic
            inline const PtrList<plcLogic>& logic() const;
            inline PtrList<plcLogic>& logic();

            //- Solver domains
            inline const wordList& solverDomains() const;

            //- Algorithm names
            inline const wordList& algorithms() const;

            //- Algorithm groups
            inline const labelListList& algorithmGroups() const;

            //- Current solver domain index
            inline const label& currentSolverDomainIndex() const;

            //- Current solver domain name
            inline word currentSolverDomainName() const;
            
            //- Current algorithm index
            inline const label& currentAlgorithmIndex() const;

            //- Current algorithm name
            inline word currentAlgorithmName() const;

        // Signals
        
            //- Signal from the solver - associated trigger's value is toggled
            void signal(word signalName);

            //- Signal from the solver - associated trigger is given newValue
            void signal(word signalName, bool newValue);
            
            //- Signal from a plcTimerTrigger that it has expired
            void timerExpired();

        // Running the plcEmulator

            //- Initializes triggers, adjustTime, then checks for a change in
            //  solver domain.  Use this at the start of a solver, before the
            //  main solver loop.  Returns true if the solverDomain changed.
            bool initializeAndAdjustTime(Time& runTime);

            //- Update the triggers - iterates until no more changes, fails
            //  when same state is encountered twice.
            void updateTriggers(const Time& runTime);
            
            //- Run user-supplied logic.  Returns true if solverDomain changes.
            bool applyLogic(Time& runTime);

            //- Set timestep to properly meet a timer trigger.  Returns true if
            //  adjustments were made.
            bool adjustTime(Time& runTime);
            
            //- Performs updateTriggers(), applyLogic() and adjustTime()
            //  and runTime.write() or runTime.writeAndEnd().
            void updateAndWrite(Time& runTime);
        
        // Debug
        
            void displayCurrentState();
};


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace Foam

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#include "plcEmulatorI.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#endif

// ************************************************************************* //
