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

#include "objectRegistrySearcher.H"

// * * * * * * * * * * * * * Private Member Functions  * * * * * * * * * * * //

template<class Type>
void Foam::objectRegistrySearcher<Type>::decomposePaths()
{
    forAll(objectRegistryPaths_, pathI)
    {
        fileName workingPath(objectRegistryPaths_[pathI]);
        wordList reversePath;

        while ( workingPath != "." )
        {
            label newIndex(reversePath.size());
            reversePath.setSize(newIndex + 1);
            reversePath[newIndex] = workingPath.name();
            workingPath = workingPath.path();
        }
        decomposedPaths_.set(pathI, new wordList(0));
        decomposedPaths_[pathI].setSize(reversePath.size());

        forAllReverse(reversePath, rPathsI)
        {
            decomposedPaths_[pathI][rPathsI] = reversePath[rPathsI];
        }
    }
}

// * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * * //

template<class Type>
Foam::objectRegistrySearcher<Type>::objectRegistrySearcher
(
    const fileNameList objectRegistryPaths
)
:
    objectRegistryPaths_(objectRegistryPaths),
    decomposedPaths_(objectRegistryPaths_.size())
{
    decomposePaths();
}

// * * * * * * * * * * * * * * * Destructor  * * * * * * * * * * * * * * * * //

template<class Type>
Foam::objectRegistrySearcher<Type>::~objectRegistrySearcher()
{}

// * * * * * * * * * * * * * * * Member Functions  * * * * * * * * * * * * * //

template<class Type>
const Type * Foam::objectRegistrySearcher<Type>::lookupObject
(
    const word& lookupName,
    const Time& runTime
)
{
    const Type * typePtr(NULL);

    forAll(decomposedPaths_, pathI)
    {
        const objectRegistry * objRegPtr(&runTime);
        forAll(decomposedPaths_[pathI], regI)
        {
            const word& findMe(decomposedPaths_[pathI][regI]);
            HashTable<const objectRegistry*> hTable
            (
                objRegPtr->lookupClass<objectRegistry>()
            );
            if (hTable.found(findMe))
            {
                objRegPtr = hTable[findMe];
            }
            else
            {
                objRegPtr = NULL;
                break;
            }
        }
        if
        (
            (objRegPtr)
         && (objRegPtr->foundObject<Type>(lookupName))
        )
        {
            typePtr =
                &objRegPtr->lookupObject<Type>(lookupName);
            break;
        }
    }
    
    return typePtr;
}


template<class Type>
void Foam::objectRegistrySearcher<Type>::showNames(const Time& runTime)
{
    forAll(decomposedPaths_, pathI)
    {
        Info << "objectRegistry " << objectRegistryPaths_[pathI] << ":"
            << endl;
        const objectRegistry * objRegPtr(&runTime);
        forAll(decomposedPaths_[pathI], regI)
        {
            const word& findMe(decomposedPaths_[pathI][regI]);
            HashTable<const objectRegistry*> hTable
            (
                objRegPtr->lookupClass<objectRegistry>()
            );
            if (hTable.found(findMe))
            {
                objRegPtr = hTable[findMe];
            }
            else
            {
                objRegPtr = NULL;
                break;
            }
        }
        if (objRegPtr)
        {
            Info << objRegPtr->names<Type>();
        }
        else
        {
            Info << "not valid" << endl;
        }
    }
}

// ************************************************************************* //
