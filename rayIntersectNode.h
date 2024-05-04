
#include <maya/MPxNode.h>

#pragma once
class rayIntersectNode : MPxNode{
public:
    MStatus compute(const MPlug& plug, MDataBlock& data) override;
    static void* creator();
    static MStatus initialize();
    // input
    static MTypeId id;
    static MObject origin;
    static MObject dir;
    static MObject inputMesh;
    // output
    static MObject intersect;
private:
    MStatus getMeshDagPath(MDagPath& dagPath, MFnDependencyNode& nodeFn) const;
};

MVector doIntersect(const MDagPath& dagPath, const MVector& originVal, const MPoint& dirVal);
