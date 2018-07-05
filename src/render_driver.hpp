#include "config.hpp"

#include <atomic>
#include <chrono>
#include <set>

#include "tracer.hpp"
#include <QObject>

class RenderDriver : public QObject
{
    Q_OBJECT
	    

public slots:
    void RenderFrame();

signals:
    void finished();
    void error(QString err);
    void started();
    void statusBarUpdate(QString status);
    void ReturnPathData(std::vector<double> PathData);
public:
    RenderDriver(  const Scene& _scene, 
		        std::shared_ptr<Config> _cfg, 
			const Camera& _camera, 
			std::string _output_file
		     ) : scene(_scene), cfg(_cfg), camera(_camera), output_file(_output_file) {};

    const Scene& scene;
    std::shared_ptr<Config> cfg;
    const Camera& camera;
    std::string output_file;

private:
    void FrameMonitorThread(RenderLimitMode render_limit_mode,
                                   unsigned int limit_rounds,
                                   unsigned int limit_minutes,
                                   unsigned int pixels_per_round);
    void RenderRound(const Scene& scene,
                            std::shared_ptr<Config> cfg,
                            const Camera& camera,
                            const std::vector<RenderTask>& tasks,
                            unsigned int& seedcount,
                            const int seedstart,
                            unsigned int concurrency,
                            EXRTexture& total_ob
                            );

    std::chrono::high_resolution_clock::time_point frame_render_start;
    std::atomic<bool> stop_monitor;

    // Performance counters
    std::atomic<int> rounds_done;
    std::atomic<int> pixels_done;
    std::atomic<unsigned int> rays_done;
    void ResetCounters();

};
