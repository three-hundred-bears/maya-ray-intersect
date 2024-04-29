
#include <maya/MPxNode.h>

#pragma once
class rayIntersectNode : MPxNode{
public:
    MStatus compute(const MPlug& plug, MDataBlock& data) override;

    static void* creator();
    static MStatus initialize();

    static MTypeId id;
    static MObject origin;
    static MObject dir;
    static MObject intersect;
    static MObject inputMesh;
};
