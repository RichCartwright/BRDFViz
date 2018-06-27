#include "texture.hpp"

#include <png++/png.hpp>
#include <jpeglib.h>
#include <jerror.h>

#include <cmath>

#include "utils.hpp"
#include "out.hpp"

#include "glm.hpp"
#include <glm/gtx/wrap.hpp>

#include "stbi.hpp"

#include <OpenEXR/ImfRgbaFile.h>

FileTexture::FileTexture(int xsize, int ysize):
    xsize(xsize), ysize(ysize)
{
    data.resize(xsize*ysize);
}

void FileTexture::SetPixel(int x, int y, Color c)
{
    data[y*xsize + x] = c;
}

Color FileTexture::GetPixel(int x, int y) const
{
    return data[y*xsize + x];
}

Color FileTexture::GetPixelInterpolated(glm::vec2 pos, bool debug) const{
    (void)debug;

    float x = glm::repeat(pos.x) * xsize - 0.5f;
    float y = glm::repeat(pos.y) * ysize - 0.5f;
    float ix0f, iy0f;
    float fx = std::modf(x, &ix0f);
    float fy = std::modf(y, &iy0f);
    int ix0 = ix0f;
    int iy0 = iy0f;
    int ix1 = (ix0 != int(xsize) - 1)? ix0 + 1 : ix0;
    int iy1 = (iy0 != int(ysize) - 1)? iy0 + 1 : iy0;
    if(ix0 == -1) ix0 = 0;
    if(iy0 == -1) iy0 = 0;
    Color c00 = data[iy0 * xsize + ix0];
    Color c01 = data[iy0 * xsize + ix1];
    Color c10 = data[iy1 * xsize + ix0];
    Color c11 = data[iy1 * xsize + ix1];

    /*
    IFDEBUG std::cout << "x " << x << " y " << y << std::endl;
    IFDEBUG std::cout << "fx " << fx  << " fy " << fy << std::endl;
    IFDEBUG std::cout << "ix1 " << ix1  << " iy1 " << iy1 << std::endl;
    IFDEBUG std::cout << "xsize " << xsize  << " ysize " << ysize << std::endl;
    */

    fy = 1.0f - fy;
    fx = 1.0f - fx;

    Color c0s = fx * c00 + (1.0f - fx) * c01;
    Color c1s = fx * c10 + (1.0f - fx) * c11;

    /*
    IFDEBUG std::cout << c00 << std::endl;
    IFDEBUG std::cout << c01 << std::endl;
    IFDEBUG std::cout << c10 << std::endl;
    IFDEBUG std::cout << c11 << std::endl;
    */

    Color css = fy * c0s + (1.0f - fy) * c1s;

    return css;
}

float FileTexture::GetSlopeRight(glm::vec2 pos) const{
    int x = glm::repeat(pos.x) * xsize - 0.5f;
    int y = glm::repeat(pos.y) * ysize - 0.5f;
    int x2 = (x != int(xsize) - 1)? x + 1 : x;
    if(x == -1) x = 0;
    if(y == -1) y = 0;
    Color  here = data[y * xsize + x];
    Color there = data[y * xsize + x2];
    float a = ( here.r +  here.g +  here.b)/3;
    float b = (there.r + there.g + there.b)/3;
    return a-b;
};
float FileTexture::GetSlopeBottom(glm::vec2 pos) const{
    int x = glm::repeat(pos.x) * xsize - 0.5f;
    int y = glm::repeat(pos.y) * ysize - 0.5f;
    int y2 = (y != int(ysize) - 1)? y + 1 : y;
    if(x == -1) x = 0;
    if(y == -1) y = 0;
    Color  here = data[y * xsize + x];
    Color there = data[y2 * xsize + x];
    float a = ( here.r +  here.g +  here.b)/3;
    float b = (there.r + there.g + there.b)/3;
    return a-b;
}

inline float clamp( float f )
{
    return 0.5f * ( 1.0f + fabsf( f ) - fabsf( f - 1.0f ) );
}

bool FileTexture::Write(std::string path) const{
    std::string out_dir = Utils::GetDir(path);
    std::string out_file = Utils::GetFilename(path);

    auto fname = Utils::GetFileExtension(out_file);
    if(fname.second == "BMP" || fname.second == "bmp"){
        WriteToBMP(path);
    }else if(fname.second == "PNG" || fname.second == "png"){
        WriteToPNG(path);
    }else{
        std::cerr << "Sorry, output file format '" << fname.second << "' is not supported." << std::endl;
        return false;
    }
    return true;
}

void FileTexture::WriteToPNG(std::string path) const{

    png::image<png::rgb_pixel> image(xsize, ysize);

    for (png::uint_32 y = 0; y < image.get_height(); ++y){
        for (png::uint_32 x = 0; x < image.get_width(); ++x){
            auto c = data[y*xsize + x];
            auto px = png::rgb_pixel(255.0*clamp(c.r),
                                     255.0*clamp(c.g),
                                     255.0*clamp(c.b));
            image[y][x] = px;
        }
    }
    image.write(path);
}

void FileTexture::WriteToBMP(std::string path) const{
    // This procedure was contributed by Adam Malinowski and donated
    // to the public domain.

    unsigned int H = ysize;
    unsigned int W = xsize;

    std::ofstream ofbStream;
    ofbStream.open(path, std::ios::binary );
    if( !ofbStream.is_open() )
    {
        // TODO: report failure
        return;
    }

    char Header[54] = {
        'B', 'M',
        'x', 'x', 'x', 'x',
        0, 0, 0, 0, 54, 0, 0, 0, 40, 0, 0, 0,
        'x', 'x', 'x', 'x',
        'x', 'x', 'x', 'x',
        1, 0, 24, 0, 0, 0, 0, 0,
        'x', 'x', 'x', 'x',
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
    };

    unsigned int S = H * ( 3 * W + W % 4 );
    *((unsigned int*) ( Header +  2 ) ) = 54 + S;
    *((unsigned int*) ( Header + 18 ) ) = W;
    *((unsigned int*) ( Header + 22 ) ) = H;
    *((unsigned int*) ( Header + 34 ) ) = S;

    ofbStream.write( Header, 54 );

    for( int i = H-1; i >= 0; --i )
        {
            for( unsigned int j = 0; j < W; ++j )
                {
                    ofbStream << (char) ( 255 * clamp( data[ i * W + j ].b ) );
                    ofbStream << (char) ( 255 * clamp( data[ i * W + j ].g ) );
                    ofbStream << (char) ( 255 * clamp( data[ i * W + j ].r ) );
                }
            for( unsigned int j = 0; j < W % 4; ++j ) ofbStream << (char) 0;
        }

    ofbStream.close();
}

FileTexture* FileTexture::CreateNewFromPNG(std::string path){
    if(!Utils::GetFileExists(path)){
        std::cerr << "Failed to load texture '" << path << ", file does not exist." << std::endl;
        return nullptr;
    }
    png::image< png::rgb_pixel > image(path);
    unsigned int w = image.get_width(), h = image.get_height();

    out::cout(5) << "Opened image '" << path << "', " << w << "x" << h << std::endl;

    FileTexture* t = new FileTexture(w,h);
    for(unsigned int y = 0; y < h; y++){
        for(unsigned int x = 0; x < w; x++){
            auto pixel = image.get_pixel(x, y);
            Color c = Color(pixel.red/255.0f, pixel.green/255.0f, pixel.blue/255.0f).gammaDecode();
            t->SetPixel(x, y, c);
        }
    }
    return t;
}

FileTexture* FileTexture::CreateNewFromJPEG(std::string path){
    if(!Utils::GetFileExists(path)){
        std::cerr << "Failed to load texture '" << path << ", file does not exist." << std::endl;
        return nullptr;
    }

    struct jpeg_decompress_struct info;
    struct jpeg_error_mgr err;

    FILE* file = fopen(path.c_str(), "rb");  //open the file

    info.err = jpeg_std_error(& err);
    jpeg_create_decompress(& info);
    if(!file) {
        std::cout << "Failed to read file `" << path << "`, ignoring this texture." << std::endl;
        return nullptr;
    }


    jpeg_stdio_src(&info, file);
    jpeg_read_header(&info, TRUE);
    jpeg_start_decompress(&info);

    int w = info.output_width;
    int h = info.output_height;
    int channels = info.num_components;
    if(channels == 3){
        unsigned char* buf = new unsigned char[w * h * 3];
        unsigned char * rowptr[1];
        while (info.output_scanline < info.output_height){
            // Enable jpeg_read_scanlines() to fill our jdata array
            rowptr[0] = (unsigned char *)buf +
                3* info.output_width * info.output_scanline;
            jpeg_read_scanlines(&info, rowptr, 1);
        }
        jpeg_finish_decompress(&info);
        fclose(file);

        FileTexture* t = new FileTexture(w,h);
        for(int y = 0; y < h; y++){
            for(int x = 0; x < w; x++){
                int p = (y * w + x) * 3;
                t->SetPixel(x, h-y-1, Color(buf[p+0]/255.0f,
                                            buf[p+1]/255.0f,
                                            buf[p+2]/255.0f).gammaDecode());
            }
        }

        delete[] buf;
        return t;

    }else if(channels == 1){
        unsigned char* buf = new unsigned char[w * h];
        unsigned char * rowptr[1];
        while (info.output_scanline < info.output_height){
            // Enable jpeg_read_scanlines() to fill our jdata array
            rowptr[0] = (unsigned char *)buf +
                info.output_width * info.output_scanline;
            jpeg_read_scanlines(&info, rowptr, 1);
        }
        jpeg_finish_decompress(&info);
        fclose(file);

        FileTexture* t = new FileTexture(w,h);
        for(int y = 0; y < h; y++){
            for(int x = 0; x < w; x++){
                int p = (y * w + x);
                t->SetPixel(x, h-y-1, Color(buf[p+0]/255.0f,
                                            buf[p+0]/255.0f,
                                            buf[p+0]/255.0f).gammaDecode());
            }
        }

        delete[] buf;
        return t;

    }else{
        std::cout << "Unsupported number of channels in JPEG file `" << path << "`, only 3-channel and 1-channel files are supported" << std::endl;
        return nullptr;
    }

}


FileTexture* FileTexture::CreateNewFromHDR(std::string path){
    if(!Utils::GetFileExists(path)){
        std::cerr << "Failed to load texture '" << path << ", file does not exist." << std::endl;
        return nullptr;
    }

    int w,h,n;
    float *file_data = stbi_loadf(path.c_str(), &w, &h, &n, 0);
    if(n != 3){
        std::cerr << "Failed to load texture '" << path << "', it does not contain exactly 3 color components." << std::endl;
        return nullptr;
    }

    out::cout(5) << "Opened image '" << path << "', " << w << "x" << h << std::endl;

    FileTexture* t = new FileTexture(w,h);
    for(int y = 0; y < h; y++){
        for(int x = 0; x < w; x++){
            int p = y*w + x;
            Color c = Color(file_data[3*p + 0],
                            file_data[3*p + 1],
                            file_data[3*p + 2]);
            t->SetPixel(x, y, c);
        }
    }
    free(file_data);
    return t;
}


void FileTexture::FillStripes(unsigned int size, Color a, Color b){
    for(unsigned int y = 0; y < ysize; y++){
        for(unsigned int x = 0; x < xsize; x++){
            unsigned int d = (x+y)%(size*2);
            if(d < size) SetPixel(x, y, a);
            else         SetPixel(x, y, b);
        }
    }
}


EXRTexture::EXRTexture(int xsize, int ysize):
    xsize(xsize), ysize(ysize)
{
    data.resize(xsize*ysize);
    count.resize(xsize*ysize, 0);
}

void EXRTexture::AddPixel(int x, int y, Radiance c, unsigned int n)
{
    // Current implementation assumes single-thread access
    //std::lock_guard<std::mutex> lk(mx);
    data[y*xsize + x] += c;
    count[y*xsize + x] += n;
}
Radiance EXRTexture::GetPixel(int x, int y) const{
    //std::lock_guard<std::mutex> lk(mx);
    int n = y*xsize + x;
    if(count[n] == 0) return Radiance();
    return data[n]/count[n];
}

bool EXRTexture::Write(std::string path) const{
    Imf::RgbaOutputFile file(path.c_str(), xsize, ysize, Imf::WRITE_RGBA);
    // Create Rgba data
    Imf::Rgba* buffer = new Imf::Rgba[xsize*ysize];
    for(unsigned int y = 0; y < ysize; y++)
        for(unsigned int x = 0; x < xsize; x++){
            int n = y*xsize + x;
            auto q = GetPixel(x, y);
            buffer[n].a = 1.0;
            buffer[n].r = q.r;
            buffer[n].g = q.g;
            buffer[n].b = q.b;
        }

    file.setFrameBuffer(buffer, 1, xsize);
    file.writePixels(ysize);
    delete[] buffer;
    return true;
}

EXRTexture EXRTexture::Normalize(float val) const{
    EXRTexture out(xsize, ysize);
    out.data = data;
    out.count = count;

    if(val <= 0.0f){
        float m = 0.0f;
        for(unsigned int y = 0; y < ysize; y++)
            for(unsigned int x = 0; x < xsize; x++){
                auto q = GetPixel(x, y);
                m = std::max(m, q.r);
                m = std::max(m, q.g);
                m = std::max(m, q.b);
            }
        val = 1.0f/m;
    }

    for(unsigned int y = 0; y < ysize; y++)
        for(unsigned int x = 0; x < xsize; x++){
            out.data[y*xsize + x].r *= val;
            out.data[y*xsize + x].g *= val;
            out.data[y*xsize + x].b *= val;
        }
    return out;
}


void EXRTexture::Accumulate(const EXRTexture& other){
    qassert_true(xsize == other.xsize);
    qassert_true(ysize == other.ysize);
    for(unsigned int y = 0; y < ysize; y++){
        for(unsigned int x = 0; x < xsize; x++){
            data[y*xsize + x] += other.data[y*xsize + x];
            count[y*xsize + x] += other.count[y*xsize + x];
        }
    }
}
