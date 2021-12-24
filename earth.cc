#include "earth.h"
#include "config.h"

#include <vector>

 // for M_PI constant
#define _USE_MATH_DEFINES
#include <math.h>



Earth::Earth() {
}

Earth::~Earth() {
}

std::vector<Point3> planeVertex;
std::vector<Point3> sphereVertex;
std::vector<Vector3> planeNormal;
std::vector<Vector3> sphereNormal;

void Earth::Init(const std::vector<std::string>& search_path) {
    // init shader program
    shader_.Init();

    // init texture: you can change to a lower-res texture here if needed
    earth_tex_.InitFromFile(Platform::FindFile("earth-2k.png", search_path));

    // init geometry
    const int nslices = 10;
    const int nstacks = 10;

    std::vector<unsigned int> indices;
    std::vector<Point2> vertices;

    for (int i = 0; i <= nslices; i++) {
        for (int j = 0; j <= nstacks; j++) {

            float temp = 2 * M_PI * ((float)i / nslices) - M_PI;
            float temp2 = M_PI * ((float)j / nstacks) - M_PI_2;
            float temp3 = 180 * ((float) j / nstacks) - 90;
            float temp4 = 360 * ((float) i / nslices) - 180;

            vertices.push_back(Point2(((float)i / nslices), 1.0 - ((float)j / nstacks)));
            planeNormal.push_back(Vector3(0, 0, -1).ToUnit());
            planeVertex.push_back(Point3(temp, temp2, 0));
            Point3 sphereVector = LatLongToSphere(temp3, temp4);
            sphereNormal.push_back(sphereVector - Point3(0, 0, 0));
            sphereVertex.push_back(sphereVector);
        }
    }

    for (int i = 0; i < nslices; i++) {
        for (int j = 0; j < nstacks; j++) {
            int incrementStacks = nstacks + 1;
            indices.push_back( (unsigned int) ((incrementStacks * i) + j) );
            indices.push_back( (unsigned int) ((incrementStacks * (i+1)) + j) );
            indices.push_back( (unsigned int) ((incrementStacks * (i+1)) + (j + 1) ) );
            indices.push_back( (unsigned int) ((incrementStacks * i) + j) );
            indices.push_back( (unsigned int) ((incrementStacks * (i+1)) + (j + 1)) );
            indices.push_back( (unsigned int) ((incrementStacks * i) + (j + 1)) );
        }
    }

    earth_mesh_.SetTexCoords(0, vertices);
    earth_mesh_.SetIndices(indices);
    earth_mesh_.SetVertices(planeVertex);
    earth_mesh_.SetNormals(planeNormal);

    earth_mesh_.UpdateGPUMemory();
}

void Earth::LerpHelper(const float location) {
    if (location >= 1.0) {
        earth_mesh_.SetVertices(sphereVertex);
        earth_mesh_.SetNormals(sphereNormal);
    }
    if (location <= 0.0) {
        earth_mesh_.SetVertices(planeVertex);
        earth_mesh_.SetNormals(planeNormal);
    }
    else {
        std::vector<Point3> v;
        std::vector<Vector3> n;

        for (int i = 0; i < planeVertex.size(); i++) {

            float temp1 = GfxMath::Lerp(planeVertex[i].x(), sphereVertex[i].x(), location);
            float temp4 = GfxMath::Lerp(planeNormal[i].x(), sphereNormal[i].x(), location);

            float temp2 = GfxMath::Lerp(planeVertex[i].y(), sphereVertex[i].y(), location);
            float temp5 = GfxMath::Lerp(planeNormal[i].y(), sphereNormal[i].y(), location);

            float temp3 = GfxMath::Lerp(planeVertex[i].z(), sphereVertex[i].z(), location);
            float temp6 = GfxMath::Lerp(planeNormal[i].z(), sphereNormal[i].z(), location);

            v.push_back(Point3(temp1, temp2, temp3));
            n.push_back(Vector3(temp4, temp5, temp6));
        }
        earth_mesh_.SetVertices(v);
        earth_mesh_.SetNormals(n);
    }
    earth_mesh_.UpdateGPUMemory();
}


void Earth::Draw(const Matrix4& model_matrix, const Matrix4& view_matrix, const Matrix4& proj_matrix) {
    // Define a really bright white light.  Lighting is a property of the "shader"
    DefaultShader::LightProperties light;
    light.position = Point3(10, 10, 10);
    light.ambient_intensity = Color(1, 1, 1);
    light.diffuse_intensity = Color(1, 1, 1);
    light.specular_intensity = Color(1, 1, 1);
    shader_.SetLight(0, light);

    // Adust the material properties, material is a property of the thing
    // (e.g., a mesh) that we draw with the shader.  The reflectance properties
    // affect the lighting.  The surface texture is the key for getting the
    // image of the earth to show up.
    DefaultShader::MaterialProperties mat;
    mat.ambient_reflectance = Color(0.5, 0.5, 0.5);
    mat.diffuse_reflectance = Color(0.75, 0.75, 0.75);
    mat.specular_reflectance = Color(0.75, 0.75, 0.75);
    mat.surface_texture = earth_tex_;

    // Draw the earth mesh using these settings
    if (earth_mesh_.num_triangles() > 0) {
        shader_.Draw(model_matrix, view_matrix, proj_matrix, &earth_mesh_, mat);
    }
}

Point3 Earth::LatLongToSphere(double latitude, double longitude) const {
    float sphereX = cosf(GfxMath::ToRadians((float)latitude)) * sinf(GfxMath::ToRadians((float)longitude)) * (float)M_PI_2;
    float sphereY = sinf(GfxMath::ToRadians((float)latitude)) * (float)M_PI_2;
    float sphereZ = cosf(GfxMath::ToRadians((float)latitude)) * cosf(GfxMath::ToRadians((float)longitude)) * (float)M_PI_2;
    return Point3(sphereX, sphereY, sphereZ);
}

Point3 Earth::LatLongToPlane(double latitude, double longitude) const {
    float planeX = ((float)longitude / 180) * M_PI;
    float planeY = ((float)latitude / 90) * M_PI_2;
    return Point3(planeX, planeY, 0);
}

void Earth::DrawDebugInfo(const Matrix4& model_matrix, const Matrix4& view_matrix, const Matrix4& proj_matrix) {
    // This draws a cylinder for each line segment on each edge of each triangle in your mesh.
    // So it will be very slow if you have a large mesh, but it's quite useful when you are
    // debugging your mesh code, especially if you start with a small mesh.
    for (int t = 0; t < earth_mesh_.num_triangles(); t++) {
        std::vector<unsigned int> indices = earth_mesh_.read_triangle_indices_data(t);
        std::vector<Point3> loop;
        loop.push_back(earth_mesh_.read_vertex_data(indices[0]));
        loop.push_back(earth_mesh_.read_vertex_data(indices[1]));
        loop.push_back(earth_mesh_.read_vertex_data(indices[2]));
        quick_shapes_.DrawLines(model_matrix, view_matrix, proj_matrix,
            Color(1, 1, 0), loop, QuickShapes::LinesType::LINE_LOOP, 0.005f);
    }
}
