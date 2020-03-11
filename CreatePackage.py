import shutil
import os

package_name = 'EffekseerForUE4/'
if os.path.exists(package_name):
    shutil.rmtree(package_name)
os.makedirs(package_name, exist_ok=False)

shutil.copytree('Config/', package_name + 'Config/')
shutil.copytree('Content/', package_name +'Content/')
shutil.copytree('Plugins/', package_name +'Plugins/')
shutil.copytree('Source/', package_name +'Source/')
shutil.copy('EffekseerForUE4.uproject', package_name)

shutil.copy('Help_En.html', package_name)
shutil.copy('Help_Ja.html', package_name)
shutil.copy('readme.txt', package_name)
shutil.copy('LICENSE', package_name)
