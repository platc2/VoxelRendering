"""
This script generates a texture atlas from the zip downloaded from https://opengameart.org/content/free-low-poly-game-asset-3d-blocks
Unpack the zip file into the unpacked/ subdirectory where this script is located.
You need the "convert" command from the ImageMagick package installed.
This script probably only workd on Linux/Unix.
"""
import os
import shutil
import re

# unpack zip from https://opengameart.org/content/free-low-poly-game-asset-3d-blocks to this directory:
path = './unpacked/'
temp_dir = './temp/'
if os.path.exists(temp_dir):
    shutil.rmtree(temp_dir)

os.mkdir(temp_dir)
os.system(f'cp unpacked/*/tex/*.gif {temp_dir}')

files = os.listdir(temp_dir)
files = [os.path.join(temp_dir, f) for f in files]
files = sorted(files, key=lambda f: f.lower())

os.system('convert ' + ' '.join(files) + ' +append ../textures/texture_atlas.gif')

shutil.rmtree(temp_dir)

# https://stackoverflow.com/a/1176023
def camel2snake(name):
    s1 = re.sub('(.)([A-Z][a-z]+)', r'\1_\2', name)
    return re.sub('([a-z0-9])([A-Z])', r'\1_\2', s1).upper()

with open('../../include/TextureAtlas.h', 'w') as f:
    f.write('#ifndef TEXTURE_ATLAS_H\n')
    f.write('#define TEXTURE_ATLAS_H\n')
    f.write('\n')
    f.write('enum TextureAtlas {\n')
    for i, t in enumerate(files):
        name = os.path.basename(t)
        name = name.split('_')[0]
        name = camel2snake(name)
        name = name.replace('0', '_0')
        name = name.replace('ICE_01', 'ICE')  # there is only one ice type
        name = name.replace('LAVA_01', 'LAVA')  # there is only one lava type
        f.write(f'    {name} = {i + 1},\n')
    f.write('};\n')
    f.write('\n')
    f.write(f'const int TEXTURE_ATLAS_SIZE = {len(files)};\n')
    f.write('\n')
    f.write('#endif // !TEXTURE_ATLAS_H\n')
