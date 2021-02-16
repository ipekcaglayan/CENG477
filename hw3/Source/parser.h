#ifndef __HW1__PARSER__
#define __HW1__PARSER__

#include <string>
#include <vector>
#include <math.h>

namespace parser
{
    //Notice that all the structures are as simple as possible
    //so that you are not enforced to adopt any style or design.
    struct Vec3f
    {
        float x, y, z;
        Vec3f()
        {
            x = 0;
            y = 0;
            z = 0;
        }
        Vec3f(float x, float y, float z)
        {
            this->x = x;
            this->y = y;
            this->z = z;
        }
        Vec3f operator+(const Vec3f rhs) const
        {
            return Vec3f(x + rhs.x, y + rhs.y, z + rhs.z);
        }
        Vec3f operator-(const Vec3f rhs) const
        {
            return Vec3f(x - rhs.x, y - rhs.y, z - rhs.z);
        }
        Vec3f CrossProduct(const Vec3f rhs) const
        {
            return Vec3f(y * rhs.z - z * rhs.y, z * rhs.x - x * rhs.z,
                         x * rhs.y - y * rhs.x);
        }
        Vec3f &operator/=(float rhs)
        {
            x /= rhs;
            y /= rhs;
            z /= rhs;
            return *this;
        }
        Vec3f operator*(float rhs) const { return Vec3f(x * rhs, y * rhs, z * rhs); }

        Vec3f operator/(float rhs) const
        {
            Vec3f result(x / rhs, y / rhs, z / rhs);
            return result;
        }
        float Length() const { return sqrt(*this * *this); }

        const Vec3f Normalized() const { return *this / this->Length(); }
        void operator=(const Vec3f rhs)
        {
            x = rhs.x;
            y = rhs.y;
            z = rhs.z;
        }
        Vec3f &operator+=(const Vec3f rhs)
        {
            x += rhs.x;
            y += rhs.y;
            z += rhs.z;
            return *this;
        }
        float operator*(const Vec3f rhs) const
        {
            return x * rhs.x + y * rhs.y + z * rhs.z;
        }
    };

    struct Vec3i
    {
        int x, y, z;
    };

    struct Vec4f
    {
        float x, y, z, w;
    };

    struct Camera
    {
        Vec3f position;
        Vec3f gaze;
        Vec3f up;
        Vec4f near_plane;
        float near_distance;
        float far_distance;
        int image_width, image_height;
    };

    struct PointLight
    {
        Vec3f position;
        Vec3f intensity;
        bool status;
    };

    struct Material
    {
        Vec3f ambient;
        Vec3f diffuse;
        Vec3f specular;
        float phong_exponent;
    };

    struct Transformation
    {
        std::string transformation_type;
        int id;
    };

    struct Face
    {
        int v0_id;
        int v1_id;
        int v2_id;
    };

    struct Mesh
    {
        int material_id;
        std::vector<Face> faces;
        std::vector<Transformation> transformations;
        std::string mesh_type;
    };

    struct Scene
    {
        //Data
        Vec3i background_color;
        int culling_enabled;
        int culling_face;
        Camera camera;
        Vec3f ambient_light;
        std::vector<PointLight> point_lights;
        std::vector<Material> materials;
        std::vector<Vec3f> vertex_data;
        std::vector<Vec3f> translations;
        std::vector<Vec3f> scalings;
        std::vector<Vec4f> rotations;
        std::vector<Mesh> meshes;
        float *vertex_normals;
        float *vertex_pos;
        int face_number;

        //Functions
        void loadFromXml(const std::string &filepath);
    };
} // namespace parser

#endif