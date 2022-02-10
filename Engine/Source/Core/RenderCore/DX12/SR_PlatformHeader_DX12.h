#pragma once

#include "d3d12.h"
#include "d3d12shader.h"
#include <dxgi1_6.h>
#pragma comment(lib,"d3d12.lib")
#pragma comment(lib,"dxgi.lib")
#pragma comment(lib,"dxguid.lib")

//#ifdef USE_PIX
//#include "WinPixEventRuntime/pix3.h"
//#endif

// For COM interface (ComPtr)
#include <wrl.h>

template<class T>
using SR_ComPtr = Microsoft::WRL::ComPtr<T>; 
