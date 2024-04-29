#include <maya/MArgDatabase.h>
#include <maya/MIntArray.h>
#include <maya/MPxCommand.h>
#include <maya/MSyntax.h>
#include <maya/MStatus.h>
#include <maya/MVector.h>

#pragma once
class rayIntersect : public MPxCommand
{
    public:
        ~rayIntersect(){ delete argDataPtr; }

        MStatus doIt( const MArgList& args);
        static MSyntax newSyntax();
        static void* creator();

        void setOrigin(MVector val){ origin = val; }
        void setDirection(MVector val){ dir = val; }
    private:
        MArgDatabase* argDataPtr{nullptr};
        void setArgDataPtr(const MArgList& args){
            argDataPtr = new MArgDatabase{syntax(), args};
        }
        MStatus parseOriginArgs();
        MStatus parseDirArgs();
        MVector origin, dir;
};
