#include "config.hpp"

#include <atomic>
#include <chrono>
#include <set>

#include "tracer.hpp"

class RenderDriver{
public:
    static void RenderFrame(const Scene& scene,
                            std::shared_ptr<Config> cfg,
                            const Camera& camera,
                            std::string output_file
                            );
private:
    static void FrameMonitorThread(RenderLimitMode render_limit_mode,
                                   unsigned int limit_rounds,
                                   unsigned int limit_minutes,
                                   unsigned int pixels_per_round);
    static void RenderRound(const Scene& scene,
                            std::shared_ptr<Config> cfg,
                            const Camera& camera,
                            const std::vector<RenderTask>& tasks,
                            unsigned int& seedcount,
                            const int seedstart,
                            unsigned int concurrency,
                            EXRTexture& total_ob
                            );

    static std::chrono::high_resolution_clock::time_point frame_render_start;
    static std::atomic<bool> stop_monitor;

    // Performance counters
    static std::atomic<int> rounds_done;
    static std::atomic<int> pixels_done;
    static std::atomic<unsigned int> rays_done;
    static void ResetCounters();

};
