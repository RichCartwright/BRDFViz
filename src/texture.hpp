#ifndef __TEXTURE_HPP__
#define __TEXTURE_HPP__

#include <string>
#include <vector>
#include <mutex>

#include "radiance.hpp"

class ReadableTexture{
public:
    virtual Color GetPixel(int x, int y) const = 0;
    virtual Color GetPixelInterpolated(glm::vec2 pos, bool debug = false) const = 0;
    virtual float GetSlopeRight(glm::vec2 pos) const = 0;
    virtual float GetSlopeBottom(glm::vec2 pos) const = 0;
    inline Color operator[](const glm::vec2& pos) const {
        return GetPixelInterpolated(pos);
    }
    inline Color Get(const glm::vec2& pos) const {
        return GetPixelInterpolated(pos);
    }
    inline Spectrum GetSpectrum(const glm::vec2& pos) const {
        return Spectrum(GetPixelInterpolated(pos));
    }
    virtual bool Empty() const = 0;
};

class WritableTexture{
    virtual void SetPixel(int x, int y, Color c) = 0;
};

class FileTexture : public ReadableTexture,
                public WritableTexture{
public:
    FileTexture(int xsize, int ysize);

    bool Write(std::string path) const;
    void WriteToPNG(std::string path) const;
    void WriteToBMP(std::string path) const;
    virtual void SetPixel(int x, int y, Color c) override;
    virtual Color GetPixel(int x, int y) const override;

    // Finite differentes for bump maps
    virtual float GetSlopeRight(glm::vec2 pos) const override;
    virtual float GetSlopeBottom(glm::vec2 pos) const override;

    virtual Color GetPixelInterpolated(glm::vec2 pos, bool debug = false) const override;

    static FileTexture* CreateNewFromPNG(std::string path);
    static FileTexture* CreateNewFromJPEG(std::string path);
    static FileTexture* CreateNewFromHDR(std::string path);

    void FillStripes(unsigned int size, Color a, Color b);

    virtual bool Empty() const override {return false;}
private:
    // Fixed size is kept manually.
    std::vector<Color> data;
    unsigned int xsize, ysize;

    FileTexture();
};

class SolidTexture : public ReadableTexture{
public:
    SolidTexture(Color color) : color(color) {}
    virtual Color GetPixel(int, int) const override {return color;}
    virtual Color GetPixelInterpolated(glm::vec2, bool) const override {return color;}
    virtual float GetSlopeRight(glm::vec2) const override {return 0;}
    virtual float GetSlopeBottom(glm::vec2) const override {return 0;}
    virtual bool Empty() const override {return false;}
private:
    Color color;
};

class EmptyTexture : public SolidTexture{
public:
    EmptyTexture() : SolidTexture(Color(0,0,0)) {}
    virtual bool Empty() const override {return true;}
};


class EXRTexture{
public:
    EXRTexture(int xsize = 0, int ysize = 0);
    EXRTexture(const EXRTexture& other) :
        xsize(other.xsize), ysize(other.ysize),
        data(other.data),  count(other.count) {
    }
    EXRTexture(EXRTexture&& other){
        std::swap(xsize,other.xsize);
        std::swap(ysize,other.ysize);
        std::swap(data,other.data);
        std::swap(count,other.count);
    }
    EXRTexture& operator=(EXRTexture&& other){
        std::swap(xsize,other.xsize);
        std::swap(ysize,other.ysize);
        std::swap(data,other.data);
        std::swap(count,other.count);
        return *this;
    }
    bool Write(std::string path) const;
    void AddPixel(int x, int y, Radiance c, unsigned int n = 1);
    Radiance GetPixel(int x, int y) const;
    // A positive value will be applied as a scaling factor to the entire texture.
    // A negative value enables automatic scaling factor detection
    EXRTexture Normalize(float val) const;

    void Accumulate(const EXRTexture& other);

private:
    unsigned int xsize, ysize;
    std::vector<Radiance> data;
    std::vector<unsigned int> count;

    mutable std::mutex mx;
};

#endif // __TEXTURE_HPP__
