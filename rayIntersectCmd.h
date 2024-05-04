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
    rayIntersect() : argDataPtr{nullptr} {};
    ~rayIntersect(){ delete argDataPtr; }

    MStatus doIt( const MArgList& args);
    static MSyntax newSyntax();
    static void* creator();
private:
    MVector origin, dir;
    MArgDatabase* argDataPtr;

    void setOrigin(MVector val){ origin = val; }
    void setDirection(MVector val){ dir = val; }
    void setArgDataPtr(const MArgList& args){
        argDataPtr = new MArgDatabase{syntax(), args};
    }
    MStatus parseOriginArgs();
    MStatus parseDirArgs();
    MVector doIntersect(const MFnMesh& meshFn) const;
};

void createLocator(const MVector& intersection);
MStatus getCurrentDagPath(MItSelectionList& selListIter, MDagPath& dagPath);
MStatus setFnObj(MFnTransform& tranFn, const MDagPath& dagPath);
MStatus setFnObj(MFnMesh& meshFn, MDagPath& dagPath);
MStatus validateSelection(const MSelectionList& selList);
