
#include <iostream>
#include <vector>
#include <fstream>
#include <string>
#include <cmath>

class Color{
    public:
        float r, g , b;
        Color() : r(0), g(0), b(0) {}
        Color(float r_, float g_, float b_) : r(r_), g(g_), b(b_) {}

};

class point3D{
    public:
        float x, y , z;
        point3D() : x(0), y(0), z(0) {}
        point3D(float x_, float y_, float z_) : x(x_), y(y_), z(z_) {}
};

class vector3D{
    public:
        float x, y , z;
        vector3D() : x(0), y(0), z(0) {}
        vector3D(float x_, float y_, float z_) : x(x_), y(y_), z(z_) {}
        vector3D operator+(const vector3D &v) const {
            return vector3D(x + v.x, y + v.y, z + v.z);
        }
        vector3D operator-(const vector3D &v) const {
            return vector3D(x - v.x, y - v.y, z - v.z);
        }
        vector3D operator*(float scalar) const {
            return vector3D(x * scalar, y * scalar, z * scalar);
        }
        vector3D operator/(float scalar) const {
            return vector3D(x / scalar, y / scalar, z / scalar);    
        }
        friend std::ostream& operator<<(std::ostream& out, const vector3D& v) {
            out << "(" << v.x << ", " << v.y << ", " << v.z << ")";
            return out;
        }
};

class Image{
    public:
        int width, height;
        std::vector<Color> pixels;
        Image(int w, int h) : width(w), height(h), pixels(w * h) {}


        void savePPM(const std::string &filename) {
            std::ofstream file(filename);
            if (!file.is_open()) {
                std::cerr << "Error: Could not open file " << filename << std::endl;
                return;
            }
            file << "P3\n" << width << " " << height << "\n255\n";
            for (const auto& pixel : pixels) {
            // Convert float (0.0 to 1.0) to integer (0 to 255)
            // We multiply by 255.99 to ensure proper rounding when casting to int
            int r = static_cast<int>(pixel.r * 255.99f);
            int g = static_cast<int>(pixel.g * 255.99f);
            int b = static_cast<int>(pixel.b * 255.99f);

            // Write the RGB values separated by spaces
            file << r << " " << g << " " << b << "\n";
            }
            file.close();
            std::cout << "Image saved to " << filename << std::endl;
        };

};
struct MaterialProps
{
    Color Kd;
    Color Ks;
    float shininess;
};

class Texture_Material {
    public:
        virtual MaterialProps getProperties(const point3D& p) const = 0;
};

class Uniform_Material : public Texture_Material {
    public:
        MaterialProps props;
        Uniform_Material(const MaterialProps& p) : props(p) {}
        MaterialProps getProperties(const point3D& p) const override {
            return props;
        }
        virtual MaterialProps getProperties(const point3D& p) const override {
            return props;
        }
};

class Object {
    public:
        Texture_Material* material;
        Object(Texture_Material* mat) : material(mat) {} 
        virtual ~Object() {}
        virtual float intersect(const point3D& rayOrigin, const vector3D& rayDir) const = 0;
        virtual vector3D getNormalAt(const point3D& p) const = 0;
        virtual MaterialProps getTexture(const point3D& p) const {
        return material->getProperties(p);
    }   
};

class Sphere: public Object {
    public:
        point3D center;
        float radius;
        Sphere(const point3D& c, float r, Texture_Material* mat) 
            : Object(mat), center(c), radius(r) {}
        float intersect(const point3D& rayOrigin, const vector3D& rayDir) const override {
            vector3D oc(rayOrigin.x - center.x, rayOrigin.y - center.y, rayOrigin.z - center.z);

            float a = rayDir.x * rayDir.x + rayDir.y * rayDir.y + rayDir.z * rayDir.z;
            float b = 2.0f * (oc.x * rayDir.x + oc.y * rayDir.y + oc.z * rayDir.z);
            float c = oc.x * oc.x + oc.y * oc.y + oc.z * oc.z - radius * radius;
            float delta = b * b - 4 * a * c;

            if (delta < 0) {
                return -1.0f; 
            } else {
                return (-b - sqrt(delta)) / (2.0f * a); 
            }
        }

        vector3D getNormalAt(const point3D& p) const override {
            return vector3D(p.x - center.x, p.y - center.y, p.z - center.z) / radius;
        }   
};

class Light {
    public:
        virtual ~Light() {}

        virtual vector3D getDirection(const point3D& p) const = 0;

        virtual Color getIntensity(const point3D& p) const = 0;
};

class PointLight : public Light {
    public:
        point3D position;
        Color intensity;

        PointLight(const point3D& pos, const Color& inten) 
            : position(pos), intensity(inten) {}

        //p is the position of the point being lit(our destination point)
        //position is the position of the light source
        vector3D getDirection(const point3D& p) const override {
            return vector3D(position.x - p.x, position.y - p.y, position.z - p.z);
        }

        Color getIntensity(const point3D& p) const override {
            return intensity;
        }
};

class Camera {
    public:
        point3D position;
        vector3D forward;
        vector3D up;
        vector3D right;
        float z_min;      
        float fov_alpha;  
        float fov_beta;

        Camera(point3D C, point3D P, vector3D worldUp, float z, float alpha, float beta) 
        : position(C), z_min(z), fov_alpha(alpha), fov_beta(beta) 
        {
            // 1. Calculate Forward: Vector from C to P
            forward = (P - C).normalize(); 

            // 2. Calculate Right: Cross Product (Forward x WorldUp)
            // Note: We normalize the result to ensure length is 1.0
            right = forward.cross(worldUp).normalize();

            // 3. Re-Calculate Up: Cross Product (Right x Forward)
            // We re-calculate 'up' instead of using 'worldUp' to ensure 
            // the camera system is perfectly perpendicular (orthogonal).
            up = right.cross(forward).normalize();
        }
};

int main(int argc, char const *argv[])
{
    Image img(256, 256);

    int x = 50;
    int y = 50;
    img.pixels[y * img.width + x] = Color(0.5f, 0.23f, 0.1f); 

    img.savePPM("output.ppm");
}
