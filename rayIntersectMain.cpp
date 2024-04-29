
#include <maya/MFnPlugin.h>

#include <rayIntersectCmd.h>
#include <rayIntersectNode.h>

MStatus initializePlugin( MObject obj){
    MStatus status;
    MFnPlugin plugin(obj, "Bears", "1.0", "Any");
    status = plugin.registerCommand(
        "rayIntersect", 
        rayIntersect::creator, 
        rayIntersect::newSyntax
    );
    if (!status){ status.perror("registerCommand"); return status; }
    status = plugin.registerNode(
        "rayIntersectNode",
        rayIntersectNode::id,
        rayIntersectNode::creator,
        rayIntersectNode::initialize
    );
    if (!status){status.perror("registerNode"); return status;}
    return status;
}

MStatus uninitializePlugin( MObject obj){
    MStatus status;
    MFnPlugin plugin(obj);
    status = plugin.deregisterCommand("rayIntersect");
    if (!status){ status.perror("registerCommand"); return status; }
    status = plugin.deregisterNode(rayIntersectNode::id);
    if (!status){status.perror("deregisterNode"); return status; }
    return status;
}