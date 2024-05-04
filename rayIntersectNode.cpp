
#include <maya/MDagPath.h>
#include <maya/MFnDependencyNode.h>
#include <maya/MFnMesh.h>
#include <maya/MFnMeshData.h>
#include <maya/MFnNumericAttribute.h>
#include <maya/MFnTypedAttribute.h>
#include <maya/MGlobal.h>
#include <maya/MObject.h>
#include <maya/MPointArray.h>
#include <maya/MPxNode.h>

#include <rayIntersectNode.h>

// init statics
MTypeId rayIntersectNode::id(0x70081166);
MObject rayIntersectNode::origin;
MObject rayIntersectNode::dir;
MObject rayIntersectNode::intersect;
MObject rayIntersectNode::inputMesh;

MStatus rayIntersectNode::compute(const MPlug& plug, MDataBlock& data){
    MObject thisNode{plug.node()};
    MFnDependencyNode nodeFn{thisNode};
    if (plug == intersect){
        MStatus status;
        // FETCH INPUT
        MDataHandle inputOrigin = data.inputValue(origin, &status);
        MDataHandle inputDirection = data.inputValue(dir, &status);
        MDataHandle inMesh = data.inputValue(inputMesh, &status);
        if (!status){
            MGlobal::displayError("Error getting input data");
            return status;
        }
        MDataHandle outputIntersection = data.outputValue(intersect, &status);
        if (!status){
            MGlobal::displayError("Error getting output data");
            return status;
        }
        // GET VALS
        MVector originVal = inputOrigin.asVector();
        MVector dirVal = inputDirection.asVector();
        MObject meshVal = inMesh.asMesh();  // asMeshTransformed simply does not work
        // GET DAG PATH
        MDagPath dagPath;
        if (!getMeshDagPath(dagPath, nodeFn)){ return MS::kFailure; }
        // DO RAY INTERSECT
        MVector intersection{doIntersect(dagPath, originVal, dirVal)};
        // SET INTERSECTION
        outputIntersection.set(intersection);
        status = data.setClean(plug);
        if (!status){
            MGlobal::displayError("Error cleaning meshSurface plug");
            return status;
        }
    } else { return MS::kUnknownParameter; }
    return MS::kSuccess;
}

void* rayIntersectNode::creator(){
    return new rayIntersectNode();
}

MStatus rayIntersectNode::getMeshDagPath(
    MDagPath& dagPath, 
    MFnDependencyNode& nodeFn) 
const {
    MStatus status;

    MPlug inMeshPlug{nodeFn.findPlug(inputMesh, true, &status)};
    if (!status){
        MGlobal::displayError(MString{"Failed to find inputMesh plug"});
        return status;
    }
    MPlugArray inputs;
    inMeshPlug.connectedTo(inputs, true, false, &status);
    if (!status){
        MGlobal::displayError(MString{"Failed to retrieve node connections"});
        return status;
    }
    if (inputs.length() == 0)
        return MS::kFailure;
    MPlug parentPlug{inputs[0]};
    MObject parentNode{parentPlug.node()};
    MFnDagNode dagNodeFn{parentNode};
    status = dagNodeFn.getPath(dagPath);
    if (!status){
        MGlobal::displayError(MString{"Failed to retrieve dag path from object"});
    }
    return status;
}

MStatus rayIntersectNode::initialize(){
    MFnTypedAttribute tAttr;
    MStatus status;

    inputMesh = tAttr.create(
        "inputMesh", 
        "inMesh", 
        MFnData::kMesh,
        MObject::kNullObj
    );

    tAttr.setWritable(true);
    tAttr.setKeyable(false);
    tAttr.setStorable(true);
    tAttr.setReadable(true);
    tAttr.setCached(false);
    status = addAttribute(inputMesh);

    MFnNumericAttribute nAttr;

    origin = nAttr.create("origin", "orig", MFnNumericData::k3Double);
    nAttr.setWritable(true);
    nAttr.setKeyable(true);
    nAttr.setStorable(true);

    dir = nAttr.create("direction", "dir", MFnNumericData::k3Double);
    nAttr.setWritable(true);
    nAttr.setKeyable(true);
    nAttr.setStorable(true);

    intersect = nAttr.create("intersection", "intersect", MFnNumericData::k3Double);
    nAttr.setWritable(false);
    nAttr.setKeyable(false);
    nAttr.setStorable(false);
    nAttr.setReadable(true);

    // add attributes
    status = addAttribute(origin);
    if (!status){status.perror("addAttribute"); return status;}
    status = addAttribute(dir);
    if (!status){status.perror("addAttribute"); return status;}
    status = addAttribute(intersect);
    if (!status){status.perror("addAttribute"); return status;}

    // setup relationships
    status = attributeAffects(origin, intersect);
    if (!status){status.perror("attributeAffects"); return status;}
    status = attributeAffects(dir, intersect);
    if (!status){status.perror("attributeAffects"); return status;}
    status = attributeAffects(inputMesh, intersect);
    if (!status){status.perror("attributeAffects"); return status;}

    return status; // MS::kSuccess;
}

MVector doIntersect(const MDagPath& dagPath, const MVector& originVal, const MPoint& dirVal){
    MFnMesh meshFn{dagPath};
    MPointArray intersections{};
    const bool doesIntersect{meshFn.intersect(
        originVal,
        MPoint{dirVal - originVal},
        intersections,
        kMFnMeshPointTolerance,
        MSpace::kWorld
    )};
    MVector intersection{};
    if (!doesIntersect) { return MVector{0.0, 0.0, 0.0}; }
    return intersections[0];
}
