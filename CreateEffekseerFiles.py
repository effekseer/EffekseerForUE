import shutil
import os
from distutils.dir_util import copy_tree

copy_tree("../Effekseer/Dev/Cpp/Effekseer", "./Plugins/Effekseer/Source/Effekseer/Private/Effekseer")
copy_tree("../Effekseer/Dev/Cpp/EffekseerRendererCommon", "./Plugins/Effekseer/Source/Effekseer/Private/EffekseerRendererCommon")

copy_tree("../Effekseer/Dev/Cpp/EffekseerMaterial", "./Plugins/Effekseer/Source/EffekseerEd/Private/EffekseerMaterial")

os.remove("./Plugins/Effekseer/Source/Effekseer/Private/Effekseer/CMakeLists.txt")

os.remove("./Plugins/Effekseer/Source/Effekseer/Private/EffekseerRendererCommon/CMakeLists.txt")
os.remove("./Plugins/Effekseer/Source/Effekseer/Private/EffekseerRendererCommon/EffekseerRenderer.DDSTextureLoader.cpp")
os.remove("./Plugins/Effekseer/Source/Effekseer/Private/EffekseerRendererCommon/EffekseerRenderer.DDSTextureLoader.h")
os.remove("./Plugins/Effekseer/Source/Effekseer/Private/EffekseerRendererCommon/EffekseerRenderer.TGATextureLoader.cpp")
os.remove("./Plugins/Effekseer/Source/Effekseer/Private/EffekseerRendererCommon/EffekseerRenderer.TGATextureLoader.h")
os.remove("./Plugins/Effekseer/Source/Effekseer/Private/EffekseerRendererCommon/EffekseerRenderer.PngTextureLoader.cpp")
os.remove("./Plugins/Effekseer/Source/Effekseer/Private/EffekseerRendererCommon/EffekseerRenderer.PngTextureLoader.h")
