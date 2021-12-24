#ifndef EARTH_H_
#define EARTH_H_

#include <mingfx.h>
using namespace mingfx;


/** This class can draw a textured earth as either a plane or a sphere or
    somewhere inbetween in order to support morphing from one shape to another.
 */
class Earth {
public:
    Earth();
    virtual ~Earth();

    /// Load texture and define geometry.  Initializes the mesh to the planar
    /// version of the earth.  The searchPath is for finding the texture file.
    void Init(const std::vector<std::string>& search_path);

    /// Draw the Earth to screen using the current version of the mesh set with
    /// the last call to UpdateMesh.
    void Draw(const Matrix4& model_matrix, const Matrix4& view_matrix, const Matrix4& proj_matrix);

    /// Given latitude and longitude, calculate 3D position for the flat earth
    /// model that lies on a plane
    Point3 LatLongToPlane(double latitude, double longitude) const;

    /// Given latitude and longitude, calculate the 3D position for the spherical
    /// earth model.
    Point3 LatLongToSphere(double latitude, double longitude) const;

    /// This can be a helpful debugging aid when creating your triangle mesh.  It
    /// draws the triangles and normals for the current earth mesh.
    void DrawDebugInfo(const Matrix4& model_matrix, const Matrix4& view_matrix, const Matrix4& proj_matrix);

    void LerpHelper(const float location);

protected:

    // Stores the earth texture map
    Texture2D earth_tex_;

    // Stores the earth geometry as a renderable mesh
    Mesh earth_mesh_;

    // Renders meshes with texture and some simple shading
    DefaultShader shader_;
    DefaultShader::MaterialProperties earth_material_;

    // Used only for the DrawDebugInfo() routine
    QuickShapes quick_shapes_;
};


#endif
