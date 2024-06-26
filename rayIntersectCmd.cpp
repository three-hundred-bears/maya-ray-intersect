
#include <stdio.h>

#include <maya/MArgList.h>
#include <maya/MIOStream.h>

#include <maya/MSelectionList.h>
#include <maya/MItSelectionList.h>
#include <maya/MGlobal.h>
#include <maya/MFnMesh.h>
#include <maya/MDagPath.h>
#include <maya/MObject.h>
#include <maya/MString.h>
#include <maya/MVector.h>
#include <maya/MPointArray.h>
#include <maya/MFnTransform.h>
#include <maya/MSyntax.h>

#include <rayIntersectCmd.h>

#define kOriginXFlag "-ox"
#define kOriginXFlagLong "-originX"
#define kOriginYFlag "-oy"
#define kOriginYFlagLong "-originY"
#define kOriginZFlag "-oz"
#define kOriginZFlagLong "-originZ"

#define kDirectionXFlag "-dx"
#define kDirectionXFlagLong "-dirX"
#define kDirectionYFlag "-dy"
#define kDirectionYFlagLong "-dirY"
#define kDirectionZFlag "-dz"
#define kDirectionZFlagLong "-dirZ"


MStatus rayIntersect::doIt(const MArgList& args){
    MStatus status{MStatus::kFailure};
    setArgDataPtr(args);
    // TODO: come up with better way to handle error display, displayError doesn't even work
    MSelectionList selList;
    MGlobal::getActiveSelectionList(selList);
    const MString errorMsg{"Invalid selection - select your ray origin, endpoint, and collision mesh, in that order"};

    // VALIDATE SELECTION
    if (!validateSelection(selList)){ return MS::kFailure; }

    MItSelectionList selListIter(selList);
    MDagPath dagPath;
    MFnMesh meshFn;
    MFnTransform originFn, dirFn;

    // GET ORIGIN
    if (!getCurrentDagPath(selListIter, dagPath)){ return MS::kFailure; }
    if (!setFnObj(originFn, dagPath)){ return MS::kFailure; }
    selListIter.next();

    // GET DIR
    if (!getCurrentDagPath(selListIter, dagPath)){ return MS::kFailure; }
    if (!setFnObj(dirFn, dagPath)){ return MS::kFailure; }
    selListIter.next();

    // GET MESH
    if (!getCurrentDagPath(selListIter, dagPath)){ return MS::kFailure; }
    if (!setFnObj(meshFn, dagPath)){ return MS::kFailure; }

    // FETCH ORIGIN/DIRECTION
    setOrigin(originFn.getTranslation(MSpace::kWorld));
    status = parseOriginArgs();
    if (!status){
        MGlobal::displayError(MString{"Failed to parse args"});
        return status;
    }
    setDirection(dirFn.getTranslation(MSpace::kWorld) - origin);
    status = parseDirArgs();
    if (!status){
        MGlobal::displayError(MString{"Failed to parse args"});
        return status;
    }

    // DO RAY INTERSECT
    MVector intersection{doIntersect(meshFn)};

    // CREATE RESULTING LOCATOR
    createLocator(intersection);

    return MS::kSuccess;
}

void createLocator(const MVector& intersection){
    MFnTransform xformFn;
    MObject xform{xformFn.create()};
    xformFn.setObject(xform);
    MString xformName{"intersection1"};
    xformFn.setName(xformName);
    xformFn.setTranslation(intersection, MSpace::kPreTransform);
}

MStatus getCurrentDagPath(
    MItSelectionList& selListIter, 
    MDagPath& dagPath)
{
    MStatus status {selListIter.getDagPath(dagPath)};
    if (!status){
        MGlobal::displayError(MString{"Failed to retrieve dag path from second object"});
        return status;
    }
    return MS::kSuccess;
}

MStatus setFnObj(MFnTransform& tranFn, const MDagPath& dagPath){
    if (!dagPath.hasFn(MFn::kTransform)){
        MGlobal::displayError(MString{"Invalid selection - select your ray origin, endpoint, and collision mesh, in that order"});
        return MS::kFailure;
    }
    MStatus status{tranFn.setObject(dagPath)};
    if (!status){
        MGlobal::displayError(MString{"Failed to set function set object"});
        return status;
    }
    return status;
}

MStatus setFnObj(MFnMesh& meshFn, MDagPath& dagPath){
    if (!dagPath.extendToShape() || !dagPath.hasFn(MFn::kMesh)){
        MGlobal::displayError(MString{"Invalid selection - select your ray origin, endpoint, and collision mesh, in that order"});
        return MS::kFailure;
    }
    MStatus status{meshFn.setObject(dagPath)};
    if (!status){
        MGlobal::displayError(MString{"Failed to set function set object"});
        return status;
    }
    return status;
}

MStatus validateSelection(const MSelectionList& selList){
    // cout << "func length - " << selList.length() << endl;
    size_t selLength{selList.length()};

    if (selLength < 3){
        MGlobal::displayError(MString{"Invalid selection - select your ray origin, endpoint, and collision mesh, in that order"});
        return MS::kFailure;
    }
    return MS::kSuccess;
}

MVector rayIntersect::doIntersect(const MFnMesh& meshFn) const {
    MPointArray intersections{};
    const bool doesIntersect{meshFn.intersect(
        origin,
        dir,
        intersections,
        kMFnMeshPointTolerance,
        MSpace::kWorld
    )};
    MVector intersection{};
    if (!doesIntersect) { return MVector{0.0, 0.0, 0.0}; }
    return intersections[0];
}

MStatus rayIntersect::parseOriginArgs(){
    MStatus status{MStatus::kSuccess};

    if (argDataPtr->isFlagSet(kOriginXFlag)){
        status = argDataPtr->getFlagArgument(kOriginXFlag, 0, origin[0]);
    }
    if (argDataPtr->isFlagSet(kOriginYFlag)){
        status = argDataPtr->getFlagArgument(kOriginYFlag, 0, origin[1]);
    }
    if (argDataPtr->isFlagSet(kOriginZFlag)){
        status = argDataPtr->getFlagArgument(kOriginZFlag, 0, origin[2]);
    }

    return status;
}

MStatus rayIntersect::parseDirArgs(){
    MStatus status{MStatus::kSuccess};

    if (argDataPtr->isFlagSet(kDirectionXFlag)){
        status = argDataPtr->getFlagArgument(kDirectionXFlag, 0, dir[0]);
    }
    if (argDataPtr->isFlagSet(kDirectionYFlag)){
        status = argDataPtr->getFlagArgument(kDirectionYFlag, 0, dir[1]);
    }
    if (argDataPtr->isFlagSet(kDirectionZFlag)){
        status = argDataPtr->getFlagArgument(kDirectionZFlag, 0, dir[2]);
    }
    return status;
}

MSyntax rayIntersect::newSyntax(){
    MSyntax syntax;

    // origin
    syntax.addFlag(kOriginXFlag, kOriginXFlagLong, MSyntax::kDouble);
    syntax.addFlag(kOriginYFlag, kOriginYFlagLong, MSyntax::kDouble);
    syntax.addFlag(kOriginZFlag, kOriginZFlagLong, MSyntax::kDouble);
    // direction
    syntax.addFlag(kDirectionXFlag, kDirectionXFlagLong, MSyntax::kDouble);
    syntax.addFlag(kDirectionYFlag, kDirectionYFlagLong, MSyntax::kDouble);
    syntax.addFlag(kDirectionZFlag, kDirectionZFlagLong, MSyntax::kDouble);

    return syntax;
}

void* rayIntersect::creator(){
    return new rayIntersect;
}
