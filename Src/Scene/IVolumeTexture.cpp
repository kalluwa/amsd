//created by kalluwa
//2015/2/10

#include "IVolumeTexture.h"

namespace kk
{
namespace scene
{
	
IVolumeTexture::IVolumeTexture(video::IVideoDriver* driver)
:ISceneNode(driver)
{
	Driver = driver;
};

}
}

