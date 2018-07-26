#include <pipeline/FFL_Pipeline.hpp>
#include <utils/FFL_File.hpp>
#include <thread/FFL_Mutex.hpp>
namespace player {
	class FFLPlayerTrack : public FFL::PipelineTrackStorage {
	public:
		FFLPlayerTrack();
		virtual ~FFLPlayerTrack();
	public:
		virtual void write(const char* data);
	private:
		FFL::File* mTrackFile;
		FFL::CMutex mLock;

	};
}
