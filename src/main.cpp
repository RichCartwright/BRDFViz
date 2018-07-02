//    Standard stuff
#include <iostream>
#include <sstream>
#include <getopt.h>
//    RGK
#include "global_config.hpp"
#include "scene.hpp"
#include "texture.hpp"
#include "config.hpp"
#include "utils.hpp"
#include "camera.hpp"
#include "out.hpp"
#include "sampler.hpp"
#include "render_driver.hpp"
//    VTK   
#include "VTKIncludes.hpp"
#include "VTKWindow.hpp"

void usage(const char* prog) __attribute__((noreturn));
void usage(const char* prog)
{
    std::cout << "Usage: " << prog << " [OPTIONS]... [FILE] \n";
    exit(0);
}

int main(int argc, char** argv)
{
    // Recognize command-line arguments
    bool collect_data = false;
    std::string directory = "";

    // Get the input file name from command line
    std::vector<std::string> infiles;
    while(optind < argc)
    {
        infiles.push_back(argv[optind]);
        optind++;
    }
    if(infiles.size() < 1)
    {
        std::cout << "ERROR: Missing FILE argument." << std::endl;
        usage(argv[0]);
    }
    else if(infiles.size() > 1)
    {
        std::cout << "ERROR: Working with multiple config files is not yet supported." << std::endl;
        usage(argv[0]);
    }
    
    std::string configfile = infiles[0];
    // Load render config file
    std::shared_ptr<Config> cfg;
    std::string cfg_ext;
    std::tie(std::ignore, cfg_ext) = Utils::GetFileExtension(configfile);
    try{
        if(cfg_ext == "rtc")
        {
            cfg = ConfigRTC::CreateFromFile(configfile);
        }
        else if(cfg_ext == "json")
        {
            cfg = ConfigJSON::CreateFromFile(configfile);
        }
        else
        {
            std::cout << "Config file format \"" << cfg_ext << "\" not recognized" << std::endl;
            return 1;
        }
    }
    catch(ConfigFileException& ex)
    {
        std::cout << "Failed to load config file: " << ex.what() << std::endl;
        return 1;
    }
	
    if(collect_data)
    {
	    cfg->collect_data = true;
	    std::stringstream ss;

	    // Lets just hardcore this into the BUILD/data folder for now.
	    // Might be worth passing in a directory so we can get data into
	    // some portable media. 
	    // Rotation step is just for rotation collection, if rotation is off
	    // the data file will just be d0.csv
	    ss << "./data/pathData.csv";
	    cfg->CreateCSV(ss.str().c_str());
    }
    
    // Prepare output file name
    if(directory != "") directory += "/";
    std::string output_file = directory + cfg->output_file;

    // Prepare the scene
    Scene scene;
    try
    {
        cfg->InstallMaterials(scene);
        cfg->InstallScene(scene);
        cfg->InstallLights(scene);
        cfg->InstallSky(scene);
        scene.MakeThinglassSet(cfg->thinglass);
    }
    catch(ConfigFileException& ex)
    {
        std::cout << "Failed to load data from config file: " << ex.what() << std::endl;
        return 1;
    }

    QSurfaceFormat::setDefaultFormat(QVTKOpenGLWidget::defaultFormat());

    QScopedPointer<QApplication> app(new QApplication(argc, argv));

    VTKWindow qvtkWindow;

    qvtkWindow.show();

    scene.Commit();

    // Prepare camera.
    Camera camera = cfg->GetCamera(0.0f);

    // The config file is not parsed anymore from this point on. This
    // is a good moment to warn user about e.g. unused keys
    cfg->PerformPostCheck();

    std::string base_output_file = output_file;

    output_file = base_output_file;
    Camera c = camera;
    RenderDriver::RenderFrame(scene, cfg, c, output_file);

    // TODO - Remove this once a GUI is setup. Best to leave in for now
    if(cfg->collect_data)
    {
        fclose(cfg->dataFile);
    }

    return app->exec();
}
