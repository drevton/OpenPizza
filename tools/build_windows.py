#!/usr/bin/env python3
"""Build and package the standalone Windows x64 game and its resource converters."""
import argparse
import hashlib
import json
import os
from pathlib import Path
import shutil
import struct
import subprocess
import sys
import tarfile
import tempfile
import urllib.request
import zipfile

ROOT = Path(__file__).resolve().parents[1]
SDL_VERSION = '2.32.10'
SDL_SHA256 = '83a5d74012311edc3c0d40ea6faecbe57ad692aa033fa5dc273cc937e3938ff2'
SDL_URL = f'https://www.libsdl.org/release/SDL2-devel-{SDL_VERSION}-mingw.tar.gz'


def zip_path(output):
  return output.parent / (output.name + '.zip')


def digest(path):
  return hashlib.sha256(path.read_bytes()).hexdigest()


def sdk(directory=None):
  if directory:
    directory = directory.resolve()
  else:
    cache = ROOT / 'build/windows-deps'
    cache.mkdir(parents=True, exist_ok=True)
    archive = cache / f'SDL2-devel-{SDL_VERSION}-mingw.tar.gz'
    if not archive.is_file():
      with urllib.request.urlopen(SDL_URL) as response, archive.with_suffix('.download').open('wb') as output:
        shutil.copyfileobj(response, output)
      archive.with_suffix('.download').replace(archive)
    if digest(archive) != SDL_SHA256:
      raise ValueError('SDL development archive checksum mismatch')
    directory = cache / f'SDL2-{SDL_VERSION}'
    if not directory.is_dir():
      with tempfile.TemporaryDirectory(prefix='sdl-', dir=cache) as work:
        with tarfile.open(archive) as source:
          source.extractall(work, filter='data')
        (Path(work) / directory.name).rename(directory)
  target = directory / 'x86_64-w64-mingw32'
  for path in (target / 'include/SDL2/SDL.h', target / 'lib/libSDL2.dll.a', target / 'bin/SDL2.dll',
      directory / 'LICENSE.txt'):
    if not path.is_file():
      raise ValueError('Incomplete SDL x64 SDK: ' + str(path))
  return directory


def pe_info(path):
  """Read architecture and DLL imports from the PE headers, without executing code."""
  data = path.read_bytes()
  if data[:2] != b'MZ':
    raise ValueError('Not a Windows executable: ' + str(path))
  pe = struct.unpack_from('<I', data, 60)[0]
  if data[pe:pe + 4] != b'PE\0\0':
    raise ValueError('Invalid PE signature')
  machine, count = struct.unpack_from('<HH', data, pe + 4)
  optional_size = struct.unpack_from('<H', data, pe + 20)[0]
  optional = pe + 24
  if machine != 0x8664 or struct.unpack_from('<H', data, optional)[0] != 0x20b:
    raise ValueError('The package requires x64 PE32+ binaries')
  sections = []
  for i in range(count):
    at = optional + optional_size + 40 * i
    size, rva, raw_size, raw = struct.unpack_from('<IIII', data, at + 8)
    sections.append((rva, max(size, raw_size), raw))
  def offset(rva):
    for start, size, raw in sections:
      if start <= rva < start + size:
        return raw + rva - start
    raise ValueError('PE RVA outside sections')
  rva = struct.unpack_from('<I', data, optional + 120)[0]
  imports = []
  if rva:
    at = offset(rva)
    while any(data[at:at + 20]):
      name = offset(struct.unpack_from('<I', data, at + 12)[0])
      imports.append(data[name:data.index(b'\0', name)].decode('ascii'))
      at += 20
  return dict(machine='x86_64',format='PE32+',imports=sorted(imports))


def audit_binary(path):
  return pe_info(path)


def package(output, sdl):
  if output.exists() or zip_path(output).exists():
    raise ValueError('Package and ZIP outputs must be new paths')
  output.parent.mkdir(parents=True, exist_ok=True)
  with tempfile.TemporaryDirectory(prefix='windows-package-', dir=output.parent) as work:
    target = Path(work) / output.name
    target.mkdir()
    binaries = {}
    for source in (ROOT / 'build/windows/PizzaTycoon.exe', ROOT / 'build/windows/pizza-native.exe',
        sdl / 'x86_64-w64-mingw32/bin/SDL2.dll'):
      binaries[source.name] = audit_binary(source)
      shutil.copy2(source, target / source.name)
    licenses = target / 'Licenses'
    shutil.copytree(ROOT / 'docs/licenses', licenses)
    shutil.copy2(ROOT / 'LICENSE', licenses / 'GPL-3.0.txt')
    shutil.copy2(ROOT / 'vendor/nuked-opl3/LICENSE', licenses / 'Nuked-OPL3.txt')
    shutil.copy2(sdl / 'LICENSE.txt', licenses / 'SDL2.txt')
    source = target / 'Source'
    source.mkdir()
    for name in ('src', 'include', 'vendor', 'tools', 'docs'):
      shutil.copytree(ROOT / name, source / name, ignore=shutil.ignore_patterns('__pycache__', '*.pyc'))
    for name in ('Makefile', 'LICENSE', 'README.md', 'GAME_DATA.md', 'BUILD_WINDOWS.md',
        'SOURCE_LAYOUT.md', '.clang-format', '.gitignore'):
      shutil.copy2(ROOT / name, source / name)
    shutil.copytree(source / 'tools', target / 'ResourceTools/tools')
    shutil.copy2(ROOT / 'README.md', target / 'README.txt')
    files = {p.relative_to(target).as_posix(): digest(p) for p in sorted(target.rglob('*')) if p.is_file()}
    (target / 'package.json').write_text(json.dumps(dict(platform='Windows x64', binaries=binaries,
        files=files, game_data='User-supplied English DOS V2.0 installation required'), indent=2) + '\n')
    target.rename(output)
  with zipfile.ZipFile(zip_path(output), 'x', zipfile.ZIP_DEFLATED) as archive:
    for path in sorted(output.rglob('*')):
      if path.is_file():
        archive.write(path, Path(output.name) / path.relative_to(output))
  return zip_path(output)


def main():
  parser=argparse.ArgumentParser(description=__doc__)
  parser.add_argument('--output',type=Path,default=ROOT/'build/OpenPizza-windows-x64')
  parser.add_argument('--sdl-dir',type=Path,help='SDL MinGW SDK root; otherwise download the pinned official SDK')
  parser.add_argument('-j','--jobs',type=int,default=4)
  options=parser.parse_args()
  if options.jobs<1:
    parser.error('--jobs must be positive')
  if options.output.exists() or zip_path(options.output).exists():
    parser.error('Choose a new output directory')
  library=sdk(options.sdl_dir)
  subprocess.run(['make','windows',f'-j{options.jobs}',f'PYTHON={sys.executable}',
      'WINDOWS_SDL='+str(library/'x86_64-w64-mingw32')],cwd=ROOT,check=True)
  print(package(options.output.resolve(),library),flush=True)


if __name__=='__main__':
  main()
